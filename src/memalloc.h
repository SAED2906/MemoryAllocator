#pragma once

#include <cstddef>
#include <source_location>
#include <iostream>
#include <cstdlib>

void global_free();

void *malloc1(size_t size, const std::source_location location = std::source_location::current());

void free1(void *block);

// void *calloc(size_t num, size_t nsize);

// void *realloc(void *block, size_t size);