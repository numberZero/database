#pragma once
#include <cstdint>

std::size_t hash_bytes(void const *data, std::size_t count);
std::size_t hash_string(char const *data);
