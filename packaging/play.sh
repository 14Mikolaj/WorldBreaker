#!/bin/sh
CDPATH= cd -P "$(dirname "$0")" || exit 1
exec ./WorldBreaker "$@"
