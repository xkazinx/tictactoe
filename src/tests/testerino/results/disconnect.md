# disconnect: test results

Written by tt.js after every `run` of `disconnect.tt` (`--no-results` skips it). Known failures and why: the routine's KNOWN header and the project's known-gaps test plan.

## Last run

- When: 2026-11-06 01:25, tictactoe at f545046 + uncommitted changes
- Run: `node tt.js run disconnect.tt`
- Result: 72 passed, 0 failed, 32.4 s

## Failing in the last run

(none)

## Passing again since the run before

- `client:7  start client_1 cwd=D:\code\tictactoe\deploy exe=D:\code\tictactoe\deploy\tictactoe.exe timeout=30000 -- testerino=1 port=6990 background title=t1  -> start failed: tictactoe.exe (pid 18728) ended before it was ready`

## History

| When | Checkout | Commit | Parameters | Result |
|---|---|---|---|---|
| 2026-11-06 01:25 | tictactoe | f545046 + uncommitted changes | (defaults) | 72 passed, 0 failed, 32.4 s |
| 2026-11-06 01:22 | tictactoe | f545046 + uncommitted changes | (defaults) | 3 passed, 1 failed (start failed at line client:7), 5.2 s |
