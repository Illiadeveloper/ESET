#pragma once

#include <filesystem>
#include <mutex>
#include <string>

#include "ThreadPool.h"

namespace fs = std::filesystem;
constexpr size_t BUFFER_SIZE = 4096;
constexpr size_t NUMBER_OF_THREADS = 4;
constexpr size_t CONTEXT_SIZE = 3;

class FileSearcher {
 public:
  explicit FileSearcher(std::string pattern);

  void search(const fs::path& path);

 private:
  void process_file(const fs::path& file);
  void process_directory(const fs::path& dir);
  void search_in_file(const fs::path& file);
  static std::string escape_context(std::string_view s);
  void report_match(const fs::path& file, size_t pos, std::string prefix,
                    std::string suffix);

 private:
  std::mutex matchMutex;

  std::string pattern;
  ThreadPool pool;
};
