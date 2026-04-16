#include <cstdlib>
#include <optional>
#include <print>
#include <string>

#include "FileSearcher.h"

struct Args {
  fs::path path;
  std::string pattern;
};

std::optional<Args> parse_args(int argc, char* argv[]) {
  if (argc != 3) {
    std::println(stderr, "Usage: ./program <path> <search_string>");
    return std::nullopt;
  }

  Args args;
  args.path = argv[1];
  args.pattern = argv[2];

  if (args.pattern.size() > 128) {
    std::println(stderr, "Error: pattern too long (max 128 characters)");
    return std::nullopt;
  }

  if (!fs::exists(args.path)) {
    std::println(stderr, "Error: path does not exist");
    return std::nullopt;
  }

  if (args.pattern.empty()) {
    std::println(stderr, "Error: pattern cannot be empty");
    return std::nullopt;
  }
  return args;
}

int run(Args args) {
  FileSearcher searcher(args.pattern);
  searcher.search(args.path);
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  std::optional<Args> args = parse_args(argc, argv);
  if (!args) return EXIT_FAILURE;
  return run(args.value());
}
