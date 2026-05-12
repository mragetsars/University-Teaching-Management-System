#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

make clean >/dev/null
make -j2 >/dev/null

COOKIE_JAR="$(mktemp)"
LOG_FILE="$(mktemp)"
BODY_FILE="$(mktemp)"
SERVER_PID=""
cleanup() {
  if [[ -n "${SERVER_PID}" ]] && kill -0 "${SERVER_PID}" 2>/dev/null; then
    kill "${SERVER_PID}" 2>/dev/null || true
  fi
  rm -f "$COOKIE_JAR" "$LOG_FILE" "$BODY_FILE"
}
trap cleanup EXIT

./out.utms data_majors.csv data_students.csv data_courses.csv data_professors.csv --web >"$LOG_FILE" 2>&1 &
SERVER_PID=$!
sleep 1

curl -fsS -c "$COOKIE_JAR" -d 'username=0&password=UT_account' http://127.0.0.1:5000/ -o /dev/null
curl -fsS -b "$COOKIE_JAR" \
  -d 'course_id=1&professor_id=810420432&capacity=70&time=Sunday%3A13-15&exam_date=1403%2F4%2F4&class_number=2' \
  http://127.0.0.1:5000/courses/add >"$BODY_FILE"
grep -q 'OK' "$BODY_FILE"

curl -fsS -b "$COOKIE_JAR" http://127.0.0.1:5000/logout -o /dev/null
curl -fsS -c "$COOKIE_JAR" -d 'username=810102612&password=ImtheproblemItsme' http://127.0.0.1:5000/ -o /dev/null
curl -fsS -b "$COOKIE_JAR" http://127.0.0.1:5000/courses >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"

curl -fsS -b "$COOKIE_JAR" -d 'id=1' http://127.0.0.1:5000/my_courses/add >"$BODY_FILE"
grep -q 'OK' "$BODY_FILE"
curl -fsS -b "$COOKIE_JAR" http://127.0.0.1:5000/my_courses >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"

printf 'UTMS web smoke tests passed.\n'
