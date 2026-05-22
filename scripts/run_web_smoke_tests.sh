#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

if [[ "${UTMS_SKIP_BUILD:-0}" != "1" ]]; then
  make clean >/dev/null
  make -j2 >/dev/null
fi
PORT="${UTMS_TEST_PORT:-5057}"
BASE_URL="http://127.0.0.1:${PORT}"
COOKIE_JAR="$(mktemp)"
LOG_FILE="$(mktemp)"
BODY_FILE="$(mktemp)"
HEADERS_FILE="$(mktemp)"
PNG_FILE="$(mktemp --suffix=.png)"
STATE_FILE="$(mktemp --suffix=.utms-state)"
SERVER_PID=""
cleanup() {
  if [[ -n "${SERVER_PID}" ]] && kill -0 "${SERVER_PID}" 2>/dev/null; then
    kill "${SERVER_PID}" 2>/dev/null || true
    wait "${SERVER_PID}" 2>/dev/null || true
  fi
  rm -f "$COOKIE_JAR" "$LOG_FILE" "$BODY_FILE" "$HEADERS_FILE" "$PNG_FILE" "$STATE_FILE"
}
trap cleanup EXIT

start_server() {
  : >"$LOG_FILE"
  ./out.utms data/data_majors.csv data/data_students.csv data/data_courses.csv data/data_professors.csv --web --port "$PORT" --state "$STATE_FILE" >"$LOG_FILE" 2>&1 &
  SERVER_PID=$!
  for _ in {1..30}; do
    if curl -sS "${BASE_URL}/" > /dev/null 2>&1; then
      return 0
    fi
    sleep 0.2
  done
  echo "server did not become ready" >&2
  cat "$LOG_FILE" >&2 || true
  return 1
}

stop_server() {
  if [[ -n "${SERVER_PID}" ]] && kill -0 "${SERVER_PID}" 2>/dev/null; then
    kill "${SERVER_PID}" 2>/dev/null || true
    wait "${SERVER_PID}" 2>/dev/null || true
  fi
  SERVER_PID=""
}

start_server

curl -fsS -c "$COOKIE_JAR" -d 'username=0&password=UT_account' "${BASE_URL}/" -o /dev/null
curl -fsS -b "$COOKIE_JAR" \
  -d 'course_id=1&professor_id=810420432&capacity=70&time=Sunday%3A13-15&exam_date=1403%2F4%2F4&class_number=2' \
  "${BASE_URL}/courses/add" >"$BODY_FILE"
grep -q 'OK' "$BODY_FILE"

curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/logout" -o /dev/null
curl -fsS -c "$COOKIE_JAR" -d 'username=810102612&password=ImtheproblemItsme' "${BASE_URL}/" -o /dev/null
curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/courses" >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"

curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/api/courses" >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"
curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/api/me" >"$BODY_FILE"
grep -q 'Elahe' "$BODY_FILE"

curl -s -D "$HEADERS_FILE" -o /dev/null "${BASE_URL}/home"
grep -q 'Location: /' "$HEADERS_FILE"

base64 -d >"$PNG_FILE" <<'PNG'
iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMB/azf7S8AAAAASUVORK5CYII=
PNG
curl -fsS -b "$COOKIE_JAR" \
  -F 'title=Picture Post' -F 'message=Upload smoke test' -F "image=@${PNG_FILE};type=image/png" \
  "${BASE_URL}/post/new" >"$BODY_FILE"
grep -q 'OK' "$BODY_FILE"

curl -fsS -b "$COOKIE_JAR" -d 'id=1' "${BASE_URL}/my_courses/add" >"$BODY_FILE"
grep -q 'OK' "$BODY_FILE"
curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/my_courses" >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"

grep -q -- '------- Request --------' "$LOG_FILE"
grep -q -- '------- Response -------' "$LOG_FILE"
grep -q -- 'password: <redacted>' "$LOG_FILE"

stop_server
rm -f "$COOKIE_JAR"
COOKIE_JAR="$(mktemp)"
start_server
curl -fsS -c "$COOKIE_JAR" -d 'username=810102612&password=ImtheproblemItsme' "${BASE_URL}/" -o /dev/null
curl -fsS -b "$COOKIE_JAR" "${BASE_URL}/my_courses" >"$BODY_FILE"
grep -q 'Advanced Programming' "$BODY_FILE"

printf 'UTMS web smoke tests passed.\n'
