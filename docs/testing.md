# Testing and Quality Checks

## Build

```bash
make clean && make
```

## Test suite

```bash
make test
```

This runs:

- CLI smoke tests
- CLI regression tests for the TA form lifecycle
- Web smoke tests covering login, authorization, API routes, PNG upload, enrollment, logging, and optional state persistence

## Individual scripts

```bash
./scripts/run_smoke_tests.sh
./scripts/run_cli_regression_tests.sh
./scripts/run_web_smoke_tests.sh
```

## Sanitizer build

```bash
make sanitize
```

This enables AddressSanitizer and UndefinedBehaviorSanitizer for local memory and UB debugging.

## Optional formatting and linting

```bash
make format
make lint
```

These targets use `clang-format` and `clang-tidy` when they are available on the host system.
