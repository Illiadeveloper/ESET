#include "FileSearcher.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <mutex>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

FileSearcher::FileSearcher(std::string pattern)
    : pattern(pattern), pool() {}

void FileSearcher::search(const fs::path& path) {
  std::error_code ec;

  if (fs::is_regular_file(path, ec)) {
    process_file(path);
    return;
  } else if (ec) {
    std::println(stderr, "Error accessing {}: {}", path.string(), ec.message());
    return;
  }

  ec.clear();
  if (fs::is_directory(path, ec)) {
    process_directory(path);
    return;
  } else if (ec) {
    std::println(stderr, "Error accessing {}: {}", path.string(), ec.message());
  } else {
    std::println(stderr, "Error: {} is not a file or directory", path.string());
  }
}

void FileSearcher::process_file(const fs::path& file) {
  pool.enqueue([file, this]() { search_in_file(file); });
}

void FileSearcher::process_directory(const fs::path& dir) {
  std::error_code ec;
  fs::recursive_directory_iterator it(dir, ec);
  if (ec) {
    std::println(stderr, "Error accessing directory {}: {}", dir.string(),
                 ec.message());
    return;
  }
  for (fs::recursive_directory_iterator end; it != end; it.increment(ec)) {
    if (ec) {
      std::println(stderr, "Error: {}", ec.message());
      ec.clear();
      continue;
    }

    if (it->is_regular_file()) {
      process_file(it->path());
    }
  }
}

void FileSearcher::search_in_file(const fs::path& file) {
  std::ifstream in(file, std::ios::binary);

  if (!in) {
    std::println(stderr, "Can't open file: {}", file.string());
    return;
  }

  size_t overlap = pattern.size() - 1;

  char buffer[BUFFER_SIZE];
  std::string tail;
  size_t totalRead = 0;

  while (in) {
    in.read(buffer, static_cast<std::streamsize>(BUFFER_SIZE));
    const std::streamsize bytesRead = in.gcount();
    if (bytesRead == 0) break;

    std::string chunk = tail + std::string(buffer, bytesRead);
    const size_t searchStart =
        tail.size() >= overlap ? tail.size() - overlap : 0;

    size_t pos = searchStart;
    while ((pos = chunk.find(pattern, pos)) != std::string::npos) {
      const size_t globalPos = totalRead + pos - tail.size();

      size_t prefixStart = (pos >= CONTEXT_SIZE) ? pos - CONTEXT_SIZE : 0;
      std::string prefix =
          escape_context(chunk.substr(prefixStart, pos - prefixStart));

      size_t suffixStart = pos + pattern.length();
      size_t suffixEnd = std::min(suffixStart + CONTEXT_SIZE, chunk.size());

      std::string suffix =
          escape_context(chunk.substr(suffixStart, suffixEnd - suffixStart));

      report_match(file, globalPos, std::move(prefix), std::move(suffix));
      ++pos;
    }

    size_t keep = std::min(pattern.size() - 1 + CONTEXT_SIZE, chunk.size());
    tail = chunk.substr(chunk.size() - keep);

    totalRead += bytesRead;
  }
}

std::string FileSearcher::escape_context(std::string_view s) {
  std::string result;
  result.reserve(s.size() * 2);
  for (char c : s) {
    if (c == '\t')
      result += "\\t";
    else if (c == '\n')
      result += "\\n";
    else
      result += c;
  }
  return result;
}

void FileSearcher::report_match(const fs::path& file, size_t pos,
                                std::string prefix, std::string suffix) {
  std::lock_guard<std::mutex> lock(matchMutex);
  std::println("{}({}): {}...{}", file.string(), pos, prefix, suffix);
}
