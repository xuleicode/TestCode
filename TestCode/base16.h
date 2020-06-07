
#ifndef BASE_16_H
#define BASE_16_H
#include <string>

int encode_base16(const char* pstr_source, char* pstr_dest);
int decode_base16(const char* pstr_source, char* pstr_dest);
int encode_base16(const std::string& str_source, std::string& str_dest);
int decode_base16(const std::string& str_source, std::string& str_dest);

#endif //BASE_16_H
