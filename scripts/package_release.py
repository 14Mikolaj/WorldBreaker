#!/usr/bin/env python3
"""Package a built x64 game with only its runtime data and fresh save slots."""

import argparse
import hashlib
from pathlib import Path
import re
import shutil
import struct
import sys
import tarfile
import tempfile
import zipfile


REPOSITORY = Path(__file__).resolve().parent.parent


def require_file(path):
    if path.is_symlink() or not path.is_file():
        raise ValueError("Required regular file is missing or a symlink: {}".format(path))
    return path


def validate_executable(path, platform):
    """Reject an accidental 32-bit, ARM, or wrong-platform release."""
    require_file(path)
    with path.open("rb") as executable:
        header = executable.read(64)
        if platform == "linux-x64":
            if (len(header) < 64 or header[:4] != b"\x7fELF"
                    or header[4:7] != b"\x02\x01\x01"
                    or struct.unpack_from("<H", header, 18)[0] != 62):
                raise ValueError("Linux release requires a little-endian ELF64 x86-64 executable")
            if struct.unpack_from("<H", header, 16)[0] not in (2, 3):
                raise ValueError("Linux input is not an executable or PIE executable")
        else:
            if len(header) < 64 or header[:2] != b"MZ":
                raise ValueError("Windows release requires a PE32+ AMD64 executable")
            executable.seek(struct.unpack_from("<I", header, 60)[0])
            pe_header = executable.read(26)
            if (len(pe_header) < 26 or pe_header[:4] != b"PE\x00\x00"
                    or struct.unpack_from("<H", pe_header, 4)[0] != 0x8664
                    or struct.unpack_from("<H", pe_header, 24)[0] != 0x20b):
                raise ValueError("Windows release requires a PE32+ AMD64 executable")
            flags = struct.unpack_from("<H", pe_header, 22)[0]
            if not flags & 0x0002 or flags & 0x2000:
                raise ValueError("Windows input is not an executable program")


def copy_file(source, target, executable=False):
    require_file(source)
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, target)
    target.chmod(0o755 if executable else 0o644)


def copy_runtime_data(game_dir, destination):
    source = game_dir / "lib"
    locations = sorted((source / "locations").rglob("*.loc"))
    skills = sorted((source / "save" / "default").glob("*.skill"))
    if not locations:
        raise ValueError("No location files found in {}".format(source / "locations"))
    for character_class in ("bruiser", "rogue", "scholar", "mage"):
        require_file(source / "save" / "default" / ("skills_" + character_class + ".skill"))
    resources = locations + skills + [
        source / "locations" / "_WorldMap.txt",
        source / "items" / "item_list.il",
        source / "debug" / "debug_location.txt",
    ]
    for resource in resources:
        # Never dereference a runtime-data directory into unrelated local files.
        relative = resource.relative_to(source)
        if any(parent.is_symlink() for parent in resource.parents if parent != source.parent):
            raise ValueError("Runtime data must not use symlinked directories: {}".format(resource))
        copy_file(resource, destination / "lib" / relative)

    # Existing saves in the build directory are deliberately never copied.
    saves = destination / "lib" / "save"
    for number in range(1, 4):
        slot = saves / str(number)
        slot.mkdir(parents=True)
        (slot / "player.sav").write_bytes(b"NEW CHARACTER\n")
        (slot / "inv.inv").write_bytes(b"")
        (slot / "skills.skill").write_bytes(b"")
    (saves / "graveyard.txt").write_bytes(b"")


def create_archive(package, output_dir, platform):
    if platform == "windows-x64":
        archive = output_dir / (package.name + ".zip")
        with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED,
                             compresslevel=9) as output:
            for path in sorted(package.rglob("*")):
                if path.is_file():
                    output.write(path, path.relative_to(package.parent).as_posix())
    else:
        archive = output_dir / (package.name + ".tar.gz")
        with tarfile.open(archive, "w:gz") as output:
            for path in [package] + sorted(package.rglob("*")):
                info = output.gettarinfo(str(path), path.relative_to(package.parent).as_posix())
                # Package metadata should not disclose a build machine's user.
                info.uid = info.gid = 0
                info.uname = info.gname = ""
                info.mode = 0o755 if path.is_dir() or path.name in ("WorldBreaker", "play.sh") else 0o644
                if path.is_file():
                    with path.open("rb") as content:
                        output.addfile(info, content)
                else:
                    output.addfile(info)
    checksum = hashlib.sha256(archive.read_bytes()).hexdigest()
    checksum_path = archive.with_name(archive.name + ".sha256")
    checksum_path.write_text("{}  {}\n".format(checksum, archive.name), encoding="ascii")
    return archive, checksum_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game-dir", required=True, type=Path,
                        help="CMake runtime directory containing WorldBreaker and lib/")
    parser.add_argument("--platform", required=True, choices=("windows-x64", "linux-x64"))
    parser.add_argument("--version", required=True, help="Release version without the v prefix, e.g. 1.0.0")
    parser.add_argument("--output-dir", default=Path("dist"), type=Path)
    args = parser.parse_args()
    if not re.fullmatch(r"[0-9]+\.[0-9]+\.[0-9]+(?:-[0-9A-Za-z.-]+)?", args.version):
        parser.error("--version must look like 1.0.0 or 1.0.0-rc.1")

    executable_name = "WorldBreaker.exe" if args.platform == "windows-x64" else "WorldBreaker"
    executable = args.game_dir / executable_name
    try:
        validate_executable(executable, args.platform)
        with tempfile.TemporaryDirectory(prefix="worldbreaker-release-") as temporary:
            package = Path(temporary) / ("WorldBreaker-v{}-{}".format(args.version, args.platform))
            package.mkdir()
            copy_file(executable, package / executable_name, executable=True)
            copy_runtime_data(args.game_dir, package)
            for name in ("LICENSE", "README.md"):
                copy_file(REPOSITORY / name, package / name)
            copy_file(REPOSITORY / "packaging" / "PLAY.txt", package / "PLAY.txt")
            launcher = "play.bat" if args.platform == "windows-x64" else "play.sh"
            copy_file(REPOSITORY / "packaging" / launcher, package / launcher,
                      executable=args.platform == "linux-x64")
            if args.platform == "windows-x64":
                content = (package / launcher).read_bytes().replace(b"\r\n", b"\n")
                (package / launcher).write_bytes(content.replace(b"\n", b"\r\n"))
            args.output_dir.mkdir(parents=True, exist_ok=True)
            archive, checksum = create_archive(package, args.output_dir, args.platform)
        print("Created {}".format(archive))
        print("Created {}".format(checksum))
    except (OSError, ValueError, struct.error) as error:
        print("Packaging failed: {}".format(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
