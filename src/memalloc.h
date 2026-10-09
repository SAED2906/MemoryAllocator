#pragma once

#include <cstddef>

void global_free();

void *malloc(size_t size);

void free(void *block);

void *calloc(size_t num, size_t nsize);

void *realloc(void *block, size_t size);