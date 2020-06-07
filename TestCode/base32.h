
#ifndef BASE_32_H
#define BASE_32_H
#include <string>

int encode_base32(const char* pstr_source, char* pstr_dest);
int decode_base32(const char* pstr_source, char* pstr_dest);
int encode_base32(const std::string& str_source, std::string& str_dest);
int decode_base32(const std::string& str_source, std::string& str_dest);

#endif //BASE_32_H
