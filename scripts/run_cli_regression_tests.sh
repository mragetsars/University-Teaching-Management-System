#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

if [[ "${UTMS_SKIP_BUILD:-0}" != "1" ]]; then
  make -j2 >/dev/null
fi

OUTPUT_FILE="$(mktemp)"
trap 'rm -f "$OUTPUT_FILE"' EXIT

./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --cli >"$OUTPUT_FILE" <<'EOF'
POST login ? id 0 password UT_account
POST course_offer ? course_id 1 professor_id 810420432 capacity 70 time Sunday:13-15 exam_date 1403/4/4 class_number 2
POST logout ?
POST login ? id 810420432 password halalalaylalalay
POST ta_form ? course_id 1 message "TA for project support"
POST logout ?
POST login ? id 810102342 password meoow
POST ta_request ? professor_id 810420432 form_id 1
POST logout ?
POST login ? id 810420432 password halalalaylalalay
POST close_ta_form ? id 1
accept
GET notification ?
EOF

grep -q 'We have received 1 requests for the teaching assistant position' "$OUTPUT_FILE"
grep -q '810102342 Nesa 4:' "$OUTPUT_FILE"
grep -q 'OK' "$OUTPUT_FILE"

printf 'UTMS CLI regression tests passed.\n'
