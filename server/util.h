#pragma once

#include <filesystem>
#include <random>

namespace fs = std::filesystem;

inline std::string
get_uuid() {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    std::uniform_int_distribution<int> dist(0, 15);

    const char *v = "0123456789abcdef";
    const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    std::string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i]) res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

inline double
random_float() {
  std::mt19937 generator (333);
  std::uniform_real_distribution<double> dis(0.0, 1.0);
  return dis(generator);
}
