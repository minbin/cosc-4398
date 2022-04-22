#pragma once

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct keys_t {
    seal::PublicKey pk;
    seal::RelinKeys rk;
    seal::GaloisKeys gk;
};

inline keys_t
load_keys(const fs::path in_fp, const seal::SEALContext context) {
    keys_t keys;
    std::ifstream is;

    is.open(in_fp / "pk", std::ifstream::binary);
    keys.pk.unsafe_load(context, is);
    is.close();

    is.open(in_fp / "rk", std::ifstream::binary);
    keys.rk.unsafe_load(context, is);
    is.close();

    is.open(in_fp / "gk", std::ifstream::binary);
    keys.gk.unsafe_load(context, is);
    is.close();

    std::cout << "Keys loaded from " << in_fp << std::endl;
    return keys;
}

inline void
save_keys(const fs::path out_fp, keys_t keys) {
    std::cout << "Saving keys to " << out_fp << std::endl;
    fs::create_directories(out_fp);
    std::filebuf fb;
    std::ostream os(&fb);

    fb.open(out_fp / "pk", std::ios::out);
    keys.pk.save(os);
    fb.close();

    fb.open(out_fp / "rk", std::ios::out);
    keys.rk.save(os);
    fb.close();

    fb.open(out_fp / "gk", std::ios::out);
    keys.gk.save(os);
    fb.close();
}
