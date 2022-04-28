#pragma once

#include <filesystem>
#include <fstream>

using namespace seal;
namespace fs = std::filesystem;

std::vector<std::vector<double>>
load_queries(std::string in_fp) {
    std::vector<std::vector<double>> queries;
    std::cout << "Loading queries from " << in_fp << std::endl;
    std::ifstream is(in_fp);
    std::string delim = ",";
    if (is.is_open()) {
        std::string s;
        while(std::getline(is, s)) {
            size_t pos = 0;
            std::vector<double> tmp;
            while((pos = s.find(delim)) != std::string::npos) {
                tmp.push_back(std::stof(s.substr(0, pos)));
                s.erase(0, pos + delim.length());
            }
            tmp.push_back(std::stof(s));
            queries.push_back(tmp);
        }
    } else {
        std::cout << "Could not open " << in_fp << std::endl;
    }
    is.close();

    return queries;
}

Ciphertext
load_ct(std::string in_fp, const SEALContext context) {
    Ciphertext ct;
    std::cout << "Loading ciphertext from " << in_fp << std::endl;
    std::ifstream is(in_fp, std::ifstream::binary);
    ct.unsafe_load(context, is);
    is.close();
    return ct;
}
