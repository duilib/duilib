## 2024-05-24 - [CRITICAL] Insecure string formatting and copying functions
**Vulnerability:** Found uses of insecure `strcpy` and `wsprintf` functions in `DuiLib/Utils/XUnzip.cpp`, which can lead to buffer overflow vulnerabilities when processing zipped files or handling filenames.
**Learning:** Legacy codebase used unbounded string copy (`strcpy`) and formatting functions (`wsprintf`). Although the target buffers are explicitly sized (e.g. `char name[MAX_PATH]`), the copy and format operations did not specify those bounds.
**Prevention:** Use Win32 bounded string copy functions like `lstrcpynA` for explicit ANSI copies up to a set length, and standard C bounded format functions like `_snprintf` to ensure buffers are never overwritten.
