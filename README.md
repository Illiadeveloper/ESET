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

## Architecture

The application is structured into several components:

- **FileSearcher**
  - Responsible for scanning files and finding matches
  - Uses streaming to avoid loading entire files into memory

- **ThreadPool**
  - Manages a pool of worker threads
  - Tasks are distributed via a synchronized queue

The design ensures scalability and efficient CPU utilization.

---

## Performance Considerations

- Files are processed using streaming to handle very large inputs
- No full file loading into memory
- Multi-threading improves performance on multi-core systems
- I/O is the main bottleneck, so threading is designed to overlap disk operations

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
./file_searcher ./data.txt "hello"
```

---

### Example Output
```bash
data.txt(45): abc...jkr
```

---

## Error Handling

- Missing arguments → prints usage message
- Invalid path → prints error and exits
- File access errors → skipped safely with warning
- All exceptions are caught at the top level to prevent crashes
