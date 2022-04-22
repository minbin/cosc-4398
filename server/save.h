#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int
save_ct(std::string out_fp, Ciphertext ct_result) {
    std::cout << "Writing to " << out_fp << std::endl;
    fs::create_directories(fs::path(out_fp).parent_path());
    if (!fs::is_directory(fs::path(out_fp).parent_path()) || !fs::exists(fs::path(out_fp).parent_path())) { // Check if src folder exists
        std::cout << "Creating " << fs::path(out_fp).parent_path() << std::endl;
        fs::create_directory(fs::path(out_fp).parent_path()); // create src folder
    }

    std::filebuf fb;
    std::ostream os(&fb);
    fb.open(fs::path(out_fp), std::ios::out);
    ct_result.save(os);
    fb.close();

    return 1;
}
