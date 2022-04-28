#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

inline void
save_ct(std::string fp, Ciphertext ct) {
    std::cout << "Writing to " << fp << std::endl;
    fs::create_directories(fs::path(fp).parent_path());

    std::filebuf fb;
    std::ostream os(&fb);
    fb.open(fs::path(fp), std::ios::out);
    ct.save(os);
    fb.close();
}
