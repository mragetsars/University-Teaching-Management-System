#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

if [[ "${UTMS_SKIP_BUILD:-0}" != "1" ]]; then
  make clean >/dev/null
  make -j2 >/dev/null
fi
./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --cli \
  < tests/smoke_input.txt \
  > /tmp/utms_smoke_output.txt

diff -u tests/smoke_expected.txt /tmp/utms_smoke_output.txt
printf 'UTMS smoke tests passed.\n'
