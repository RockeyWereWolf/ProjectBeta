#pragma once
#include <cstdint>
#include <string>

static int64_t parseSeed(const std::string &str) {

  try {
    size_t idx;
    int64_t val = std::stoll(str, &idx);
    if (idx == str.length())
      return val;
  } catch (...) {
  }

  int64_t h = 0;
  for (char c : str) {
    h = 31 * h + c;
  }
  return h;
}
