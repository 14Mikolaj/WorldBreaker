#!/usr/bin/env python3
"""Exercise a built game in disposable copies, never in a player's save directory."""

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


CLASSES = {
    "Bruiser": (1, [7, 9, 6, 5], [
        "Greater Slam", "Patch Up", "Spirit Slash", "Flames of Fury",
        "Bulker Down", "Recouperate",
    ]),
    "Rogue": (2, [6, 6, 8, 7], [
        "Hidden Blade", "Barrage Flurry", "Prepare", "Rough Patches",
        "Assassinate", "Blend",
    ]),
    "Scholar": (3, [5, 8, 7, 7], [
        "Mark Enemy", "Minor Heal", "Power Attack", "Focus Mind",
        "Conceal", "Aspect of Drakii",
    ]),
    "Mage": (4, [5, 7, 6, 9], [
        "Energy Pulse", "Mistic Siphon", "Concentrate", "Soul Shield",
        "Flame Cylinder", "Greater Heal",
    ]),
}


class GameSmokeTests(unittest.TestCase):
    game_dir = None
    timeout = 10
    require_launcher = False

    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="worldbreaker smoke ")
        self.addCleanup(temporary.cleanup)
        self.runtime = Path(temporary.name) / "game"
        shutil.copytree(self.game_dir, self.runtime)
        name = "WorldBreaker.exe" if os.name == "nt" else "WorldBreaker"
        self.executable = self.runtime / name
        self.assertTrue(self.executable.is_file(), str(self.executable))

    def run_game(self, *inputs, command=None, cwd=None):
        commands = "\n".join(str(value) for value in inputs)
        if inputs:
            commands += "\n"
        # A broken input loop can generate a lot of output. Capture it on disk,
        # enforce a timeout, and only load bounded diagnostics into memory.
        with tempfile.TemporaryFile() as output_file:
            try:
                result = subprocess.run(
                    command or [str(self.executable)], input=commands.encode("utf-8"),
                    cwd=cwd or self.runtime, stdout=output_file,
                    stderr=subprocess.STDOUT, timeout=self.timeout, check=False,
                )
            except subprocess.TimeoutExpired:
                output_file.seek(0, os.SEEK_END)
                output_file.seek(max(0, output_file.tell() - 4000))
                self.fail("Game timed out:\n" + output_file.read().decode("utf-8", "replace"))
            output_file.seek(0, os.SEEK_END)
            size = output_file.tell()
            output_file.seek(max(0, size - 1024 * 1024))
            output = output_file.read().decode("utf-8", "replace").replace("\r\n", "\n")
        self.assertLess(size, 1024 * 1024, "Excessive output; possible input loop")
        self.assertEqual(result.returncode, 0, output[-4000:])
        self.assertNotIn("OPEN ERROR", output, output[-4000:])
        self.assertNotIn("Invalid input", output, output[-4000:])
        return output

    def save_lines(self, filename="player.sav", slot=1):
        return (self.runtime / "lib" / "save" / str(slot) / filename).read_text().splitlines()

    def create_character(self, class_name, slot=1):
        class_number, stats, skills = CLASSES[class_name]
        name = "Smoke " + class_name
        # Create, spend the one extra point on CON, inspect stats and skills,
        # save explicitly, return to the main menu, and exit.
        output = self.run_game(
            1, slot, name, 1, class_number, 1, 2,
            1, 1, 1, 3, 5, 3, 5, 6, 4,
        )
        self.assertIn("Level 1 " + class_name, output)
        self.assertIn("Noobsville [Lv 2]", output)
        self.assertIn("Player saved to slot " + str(slot), output)
        for skill in skills:
            self.assertIn(skill, output)
        saved = self.save_lines(slot=slot)
        self.assertEqual(len(saved), 24)
        self.assertEqual(saved[:4], [name, class_name, "0", "1"])
        self.assertEqual([int(value) for value in saved[8:12]], stats)
        self.assertEqual(saved[4:8], [str(stats[1] * 10)] * 2 + [str(stats[3] * 5)] * 2)
        self.assertEqual(saved[12:15], ["0", "10", "Noobsville"])
        self.assertEqual(saved[15:], ["None"] * 9)
        saved_skills = self.save_lines("skills.skill", slot)
        self.assertEqual(len(saved_skills), 24)
        self.assertEqual(saved_skills[::4], skills)
        return saved

    def test_fresh_menu_and_three_empty_slots(self):
        for slot in range(1, 4):
            self.assertEqual(self.save_lines(slot=slot), ["NEW CHARACTER"])
            self.assertEqual(self.save_lines("inv.inv", slot), [])
            self.assertEqual(self.save_lines("skills.skill", slot), [])
        self.assertEqual((self.runtime / "lib/save/graveyard.txt").read_text(), "")
        output = self.run_game(1, 4, 4)
        self.assertIn("MAIN MENU", output)
        self.assertIn("CHARACTER SELECT", output)
        for slot in range(1, 4):
            self.assertIn(str(slot) + "-->NEW CHARACTER", output)

    def test_packaged_launcher_from_another_directory(self):
        launcher = self.runtime / ("play.bat" if os.name == "nt" else "play.sh")
        if not launcher.is_file() and not self.require_launcher:
            self.skipTest("Launchers are added when packaging the release")
        self.assertTrue(launcher.is_file(), "Release is missing " + launcher.name)
        if os.name == "nt":
            command = [os.environ.get("COMSPEC", "cmd.exe"), "/d", "/c", str(launcher)]
        else:
            self.assertTrue(os.access(launcher, os.X_OK), "Launcher is not executable")
            command = [str(launcher)]
        # The temporary path contains spaces. Selecting characters also proves
        # that the launcher set the working directory needed for game data.
        output = self.run_game(1, 4, 4, command=command, cwd=self.runtime.parent)
        self.assertIn("CHARACTER SELECT", output)
        for slot in range(1, 4):
            self.assertIn(str(slot) + "-->NEW CHARACTER", output)

    def test_end_of_input_exits_without_looping(self):
        # EOF at the menu, slot selection, name entry/confirmation, class
        # selection/confirmation, and stat allocation must all terminate.
        for inputs in [
            (), (1,), (1, 1), (1, 1, "EOF Test"),
            (1, 1, "EOF Test", 1), (1, 1, "EOF Test", 1, 1),
            (1, 1, "EOF Test", 1, 1, 1),
        ]:
            with self.subTest(inputs=inputs):
                self.run_game(*inputs)
                self.assertEqual(self.save_lines(), ["NEW CHARACTER"])

    def test_create_bruiser(self):
        self.create_character("Bruiser", slot=1)

    def test_create_rogue(self):
        self.create_character("Rogue", slot=2)

    def test_create_scholar(self):
        self.create_character("Scholar", slot=3)

    def test_create_mage(self):
        self.create_character("Mage", slot=1)

    def test_npc_dialogue_map_and_travel(self):
        self.create_character("Bruiser")
        output = self.run_game(1, 1, 2, 1, 1, 5, 4, 3, 2, 5, 6, 4)
        for expected in [
            "Available NPCs in Noobsville", "Tommy", "Pancake", "Teeto",
            "Shady guy", "I am Tommy, a helpful guy.", "YsloviaCity",
            "Noobsville - RookieTown", "1-->Aiomi Forest", "2-->Rookie Town",
            "You have travelled to Rookie Town.",
        ]:
            self.assertIn(expected, output)
        self.assertEqual(self.save_lines()[14], "Rookie Town")

    def test_inventory_and_skills_survive_save_and_reload(self):
        original_player = self.create_character("Bruiser")
        original_skills = self.save_lines("skills.skill")
        # Seed real items in the disposable save to avoid depending on random
        # combat drops. The game must load, inspect, and save them itself.
        inventory = self.runtime / "lib/save/1/inv.inv"
        inventory.write_text("Wooden Stick\nRusty Ring\n")
        for round_trip in range(2):
            with self.subTest(round_trip=round_trip):
                output = self.run_game(
                    1, 1, 1, 1, 4, 1, 3, 2, 3, 9, 3, 5, 3, 5, 6, 4,
                )
                self.assertIn("INSPECT\n\nWooden Stick", output)
                self.assertIn("Slot: Hand", output)
                self.assertIn("INSPECT\n\nRusty Ring", output)
                self.assertIn("Slot: Accessory", output)
                for skill in CLASSES["Bruiser"][2]:
                    self.assertIn(skill, output)
                self.assertEqual(self.save_lines(), original_player)
                self.assertEqual(self.save_lines("inv.inv"), ["Wooden Stick", "Rusty Ring"])
                self.assertEqual(self.save_lines("skills.skill"), original_skills)

    def test_escape_combat_and_reload(self):
        original_player = self.create_character("Bruiser")
        # A fresh CON-boosted Bruiser has 90 HP, enough to survive escaping any
        # of Noobsville's level 1-3 enemies regardless of their random stats.
        output = self.run_game(1, 1, 1, 2, 4, 3, 5, 6, 4)
        self.assertIn("4-->Escape", output)
        self.assertTrue(any(enemy in output for enemy in [
            "Baby Spider", "Rabid Rat", "Small Slime",
        ]), output)
        self.assertIn("damage.", output)
        self.assertGreaterEqual(output.count("INVESTIGATE\n"), 2)
        saved = self.save_lines()
        hp = int(saved[4])
        self.assertGreater(hp, 0)
        self.assertLess(hp, int(original_player[4]))
        self.assertEqual(saved[:4], original_player[:4])
        self.assertEqual(saved[5:], original_player[5:])
        output = self.run_game(1, 1, 1, 1, 1, 5, 3, 6, 4)
        self.assertIn("HP  : " + str(hp) + " / 90", output)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--game-dir", type=Path,
                        help="Fresh runtime directory containing WorldBreaker and lib/")
    source.add_argument("--archive", type=Path,
                        help="Trusted release ZIP or tar.gz generated by package_release.py")
    parser.add_argument("--timeout", type=float, default=10,
                        help="Maximum seconds per game process (default: 10)")
    arguments = parser.parse_args()
    if arguments.timeout <= 0:
        parser.error("--timeout must be greater than zero")
    GameSmokeTests.timeout = arguments.timeout
    with tempfile.TemporaryDirectory(prefix="worldbreaker archive ") as extraction:
        if arguments.archive:
            shutil.unpack_archive(str(arguments.archive.resolve()), extraction)
            executable = "WorldBreaker.exe" if os.name == "nt" else "WorldBreaker"
            matches = list(Path(extraction).rglob(executable))
            if len(matches) != 1:
                parser.error("Archive must contain exactly one " + executable)
            GameSmokeTests.game_dir = matches[0].parent
            GameSmokeTests.require_launcher = True
        else:
            GameSmokeTests.game_dir = arguments.game_dir.resolve()
        unittest.main(argv=[__file__], verbosity=2)
