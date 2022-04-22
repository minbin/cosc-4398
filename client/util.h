#pragma once

std::string
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

inline void
print_json(std::string out_fp, std::vector<double> ans) {
    std::cout << "Writing to " << fs::path(out_fp) / "result.json" << std::endl;
    fs::create_directories(fs::path(out_fp));
    std::ofstream out(fs::path(out_fp) / "result.json");
    if (out.is_open()) {
        out << "{\n  \"res\": [";
        for (int i=0; i < ans.size(); i++) {
          out << " " << ans[i] << ((i != ans.size() - 1) ? "," : " ]\n");
        }
        out << "}";
        out.close();
    }
}
