#pragma once

#include "seal/seal.h"
#include <iomanip>
#include <iostream>

inline void print_parameters(std::string uuid, const seal::SEALContext &context) {
  std::cout << "| Creating " << uuid << std::endl;

  auto &context_data = *context.key_context_data();
  std::string scheme_name;
  switch (context_data.parms().scheme()) {
    case seal::scheme_type::bfv:
      scheme_name = "BFV";
      break;
    case seal::scheme_type::ckks:
      scheme_name = "CKKS";
      break;
    default:
      throw std::invalid_argument("unsupported scheme");
  }

  std::cout << "| Encryption parameters :" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

  std::cout << "|   coeff_modulus size: ";
  std::cout << context_data.total_coeff_modulus_bit_count() << " (";
  auto coeff_modulus = context_data.parms().coeff_modulus();
  std::size_t coeff_modulus_size = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_modulus_size - 1; i++) {
    std::cout << coeff_modulus[i].bit_count() << " + ";
  }
  std::cout << coeff_modulus.back().bit_count();
  std::cout << ") bits" << std::endl;
}

template <typename T>
inline void print_vector(std::vector<T> vec, std::size_t print_size = 4, int prec = 3) {
  std::ios old_fmt(nullptr);
  old_fmt.copyfmt(std::cout);

  std::size_t slot_count = vec.size();

  std::cout << std::fixed << std::setprecision(prec);
  std::cout << std::endl;
  if (slot_count <= 2 * print_size) {
    std::cout << "    [";
    for (std::size_t i = 0; i < slot_count; i++) {
      std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
    }
  } else {
    vec.resize(std::max(vec.size(), 2 * print_size));
    std::cout << "    [";
    for (std::size_t i = 0; i < print_size; i++) {
      std::cout << " " << vec[i] << ",";
    }
    if (vec.size() > 2 * print_size) {
      std::cout << " ...,";
    }
    for (std::size_t i = slot_count - print_size; i < slot_count; i++) {
      std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
    }
  }
  std::cout << std::endl;
  std::cout.copyfmt(old_fmt);
}

inline void print_line(int line_number) {
  std::cout << "Line " << std::setw(3) << line_number << " --> ";
}
