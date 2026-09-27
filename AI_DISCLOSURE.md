# AI disclosure

## Tool and dates

- Tool: Cursor, model Grok 4.7
- Date consulted: September 26, 2026

The handout says not to paste the assignment in as the whole prompt. The code was not produced that way. The first message asked for an explanation of how to do the assignment. That reply fixed the parsing rules before any source file existed. A second message asked which files to write. The third message asked for the files.

## Prompts

These are the messages sent in the session, in order.

1. `@ipv4_extraction_assignment.md read this. explain how you would do the assignment`

2. `what files would be written`

3. `go ahead and write them all`

The planning reply, not a second code-generation prompt, is what specified the rules the implementation follows:

- A candidate is a maximal contiguous run of digits, `.`, and `:`. Every other character is a separator and is skipped.
- The run is accepted only if it matches the grammar from the first character through the last. A valid address is not cut out of a longer run, and a bad port rejects the address in that same run.
- Each octet is 1–3 digits, value 0–255. The port is 1–5 digits, value 0–65535. A leading zero is allowed only when the component is the single digit `0`.
- Digit values are accumulated with `value * 10 + (c - '0')`. No conversion or address library is used.
- The leftmost valid run is the one returned. A rejected run does not stop the scan.
- `END` quits only when it is the entire line.

## What was generated, and what was changed

| File | Who wrote it |
|---|---|
| `extract_ipv4.cpp` | Generated in full by Grok 4.7 in this session |
| `tests.txt` | Generated in full by Grok 4.7 in this session |
| `AI_DISCLOSURE.md` | Generated in full by Grok 4.7 in this session |

No part of the source was written by hand outside this session, and no hand edit was applied after the files were generated. The only change after the first write was a comment in `tests.txt` noting that one input is three spaces, so that case is visible when the file is read. The input itself was not changed.

Nothing in the C++ was rewritten after the test run. The first compiled version matched every case.

## How the output was checked

Compiled with:

```text
c++ -std=c++17 -Wall -Wextra -Wpedantic -o extract_ipv4 extract_ipv4.cpp
```

That command produced no warnings. Every case in `tests.txt` was then fed to the program. All 55 matched, including the sample transcript in the handout. Decimal values in the test file were computed separately as `(A<<24)|(B<<16)|(C<<8)|D` and were not copied out of the program's own output.

Cases that this style of parser usually gets wrong were included on purpose: a trailing dot (`192.168.1.1.`), a leading zero (`192.168.01.1`), a port past 65535 (`1.2.3.4:99999`, which must reject the address too), a colon with no port, a second colon, and a separator that splits one run into two (`192a168.1.1.1`).

## Verification

The source was reviewed against the grammar above while it was written, and again by running `tests.txt`. No known bug remains in the cases the handout states and the extra cases in the test file.

Two behaviors are decisions, because the handout does not spell them out. They are implemented as follows and covered by tests:

- If a line contains two valid addresses, the leftmost one is returned. `1.2.3.4 and 5.6.7.8` yields `1.2.3.4`. The line is not rejected.
- If an earlier run is invalid, scanning continues. `1.2.3.4:99999 8.8.8.8` yields `8.8.8.8`.

## Limitations

- `END` must be the whole line. `end`, `End`, `END `, and ` END` do not quit. `end` is covered by a test and is reported as invalid input.
- `+`, `-`, `/`, and letters are separators. `+1.2.3.4` and `192.168.1.1/24` are accepted, because the sign and the slash are not part of a run.
- A space is a separator. `192.168.1.1 :80` is accepted with port `none`. `192.168.1.1:` is rejected, because the colon is inside the run and the port is missing.
- End of file without the line `END` leaves the loop and does not print `Program terminated.`
