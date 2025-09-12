#!/usr/bin/env bash
# Simple webserv tester (OK/KO) — easily extensible
set -o pipefail

HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-8080}"
SERVER_BIN="${SERVER_BIN:-}"        # e.g. ./webserv
SERVER_CONF="${SERVER_CONF:-}"      # e.g. config/webserv.conf
CASES_DIR="${CASES_DIR:-$(dirname "$0")/cases.d}"

BASE_URL="http://${HOST}:${PORT}"
TMPDIR="$(mktemp -d)"
trap '[[ -n "${SERVER_PID:-}" ]] && kill "${SERVER_PID}" 2>/dev/null; rm -rf "$TMPDIR"' EXIT

GREEN="\033[32m"; RED="\033[31m"; DIM="\033[2m"; BOLD="\033[1m"; RESET="\033[0m"
ok() {   printf "${GREEN}[OK]${RESET} %s\n" "$1"; }
ko() {   printf "${RED}[KO]${RESET} %s\n" "$1"; }
info() { printf "${DIM}%s${RESET}\n" "$1"; }

usage() {
  cat <<USAGE
Usage: $0 [--host HOST] [--port PORT] [--server-bin PATH --config FILE] [--cases DIR]
Examples:
  $0 --server-bin ./webserv --config config/webserv.conf
  HOST=127.0.0.1 PORT=8080 $0
USAGE
}
while [[ $# -gt 0 ]]; do
  case "$1" in
    --host) HOST="$2"; shift 2;;
    --port) PORT="$2"; shift 2;;
    --server-bin) SERVER_BIN="$2"; shift 2;;
    --config) SERVER_CONF="$2"; shift 2;;
    --cases) CASES_DIR="$2"; shift 2;;
    -h|--help) usage; exit 0;;
    *) echo "Unknown arg: $1"; usage; exit 1;;
  esac
done
BASE_URL="http://${HOST}:${PORT}"

RESP_STATUS=""; RESP_HEADERS_FILE=""; RESP_BODY_FILE=""
http_request() {
  local method="$1" path="$2"
  RESP_HEADERS_FILE="$TMPDIR/headers.$$"
  RESP_BODY_FILE="$TMPDIR/body.$$"
  RESP_STATUS="$(curl -sS -o "$RESP_BODY_FILE" -D "$RESP_HEADERS_FILE" -w "%{http_code}" -X "$method" "${BASE_URL}${path}")" || return 1
}
assert_status() { # METHOD PATH EXPECTED
  local method="$1" path="$2" expected="$3"
  http_request "$method" "$path" || return 1
  [[ "$RESP_STATUS" == "$expected" ]]
}
assert_header_contains() { # METHOD PATH HEADER SUBSTR
  local method="$1" path="$2" header="$3" substr="$4"
  http_request "$method" "$path" || return 1
  awk -v IGNORECASE=1 -v pref="^${header}:" '$0 ~ pref { sub(/^[^:]*:[[:space:]]*/, "", $0); print; exit }' "$RESP_HEADERS_FILE" | grep -qi -- "$substr"
}
assert_body_contains() { # METHOD PATH SUBSTR
  local method="$1" path="$2" substr="$3"
  http_request "$method" "$path" || return 1
  grep -qi -- "$substr" "$RESP_BODY_FILE"
}

declare -a TEST_FUNCS=()
declare -a TEST_DESCS=()
add_test() { TEST_DESCS+=("$1"); TEST_FUNCS+=("$2"); }

start_server_if_needed() {
  if [[ -n "$SERVER_BIN" ]]; then
    [[ -n "$SERVER_CONF" ]] || { echo "When using --server-bin you must also pass --config"; exit 1; }
    info "Starting server: $SERVER_BIN $SERVER_CONF"
    "$SERVER_BIN" "$SERVER_CONF" &
    SERVER_PID=$!
    for _ in {1..50}; do
      curl -sS -o /dev/null "${BASE_URL}/" && return 0
      sleep 0.1
    done
    echo "Server did not become ready at ${BASE_URL}/"; exit 1
  else
    info "Assuming server already running at ${BASE_URL}"
  fi
}

if [[ -d "$CASES_DIR" ]]; then
  for f in "$CASES_DIR"/*.sh; do [[ -e "$f" ]] && source "$f"; done
else
  info "No cases dir found at $CASES_DIR. Running built-in smoke tests."
  add_test "GET / returns 200" test_root_200
  test_root_200() { assert_status GET / 200; }
fi

start_server_if_needed

total=0; passed=0; failed=0
for i in "${!TEST_FUNCS[@]}"; do
  total=$((total+1)); desc="${TEST_DESCS[$i]}"; func="${TEST_FUNCS[$i]}"
  if $func; then ok "$desc"; passed=$((passed+1))
  else
    ko "$desc"
    [[ -n "$RESP_STATUS" ]] && info "  status=${RESP_STATUS}"
    [[ -f "$RESP_HEADERS_FILE" ]] && info "  header: $(head -1 "$RESP_HEADERS_FILE")"
    [[ -f "$RESP_BODY_FILE" ]] && info "  body: $(head -c 160 "$RESP_BODY_FILE" | tr '\n' ' ' ; echo ' ...')"
    failed=$((failed+1))
  fi
done

echo -e "${BOLD}Summary:${RESET} ${passed}/${total} passed, ${failed} failed"
[[ $failed -eq 0 ]] || exit 1
