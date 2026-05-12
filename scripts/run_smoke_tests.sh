#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

make clean >/dev/null
make -j2 >/dev/null

./utms.out data_majors.csv data_students.csv data_courses.csv data_professors.csv --cli \
  < tests/smoke_input.txt \
  > /tmp/utms_smoke_output.txt

diff -u tests/smoke_expected.txt /tmp/utms_smoke_output.txt
printf 'UTMS smoke tests passed.\n'
