# File Search Application (C++)

## Overview

This application searches for a given string inside files and directories recursively.

It supports large files, multithreaded processing, and safe error handling.

The output format is:

```
<file>(<position>): <prefix>…<suffix>
```

Where:
- `<file>` — name of the file containing the match
- `<position>` — zero-based offset of the match
- `<prefix>` — up to 3 characters before the match
- `<suffix>` — up to 3 characters after the match

Special characters are escaped:
- Tab → `\t`
- New line → `\n`

---
## Thread Pool

The number of worker threads is based on:
```
std::thread::hardware_concurrency()
```

Each thread processes files independently from the shared queue.

---

## Build Instructions

### Requirements
- C++23
- CMake ≥ 3.20

### Build

```bash
git clone https://github.com/Illiadeveloper/ESET 
cd ESET
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Usage

```bash
./file_searcher <path> <search_string>
```

### Example

```bash
./file_searcher ./data "hello"
```

---

## Error Handling

- Missing arguments → prints usage message
- Invalid path → prints error and exits
- File access errors → skipped safely with warning
- All exceptions are caught at the top level to prevent crashes
