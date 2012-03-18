#ifndef ZLIB_WRAPPER_H
#define ZLIB_WRAPPER_H

#include <string>

/** Decompress an STL string using zlib and return the original data. */
std::string decompress_string(const std::string& str);

#endif
