
#ifndef BASE_64_H
#define BASE_64_H
#include <string>

int encode_base64(const char* pstr_source, char* pstr_dest);
int decode_base64(const char* pstr_source, char* pstr_dest);
int encode_base64(const std::string& str_source, std::string& str_dest);
int decode_base64(const std::string& str_source, std::string& str_dest);

#endif //BASE_64_H
