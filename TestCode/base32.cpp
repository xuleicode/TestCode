#include "base32.h"
#include <string>
//全局常量
static const char *base32char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
static const char  padding_char = '=';
static const unsigned int encode_number = 8;
static const unsigned int source_number = 5;
//获取char的索引
static char getCharIndex(char c)
{
	for (unsigned int i = 0; i < strlen(base32char); ++i) {
		if (c == *(base32char + i)) {
			return i & 0x1F; //0001 1111
		}
	}
	return -1;
}
int encode_base32(const char* pstr_source, char* pstr_dest) {
	if (pstr_source == nullptr) {
		return -1;
	}
	if (pstr_dest == nullptr) {
		return -2;
	}
	const size_t len = strlen(pstr_source);
	unsigned int i = 0, j = 0;
	unsigned char index = 0;

	//操作无符号char的移位，其实不用 与 mask了,默认的填充其实是填充0。 如果有有符号的，左边以符号位（即最高为）补齐，否则以0补齐 。避免编译器实现不同，还是手动mask一遍吧。

	for (; i < len; i += source_number) { //source_number个一组，出encode_number个编码
		index = (pstr_source[i] >> 3) & 0x1F;			//  = 14 / >> 5 / & 2 / [] 1 / () 1 / 运算符优先级问题，一定要添加（）
		pstr_dest[j++] = base32char[index];//1/8
		index = (pstr_source[i] << 2) & 0x1C;
		if (i + 1 < len) {//2-3/8
			index |= (pstr_source[i + 1] >> 6) & 0x03;  // != 14 / >> 5 / & 2 / [] 1 / () 1 /
			pstr_dest[j++] = base32char[index];//2/8
			index = (pstr_source[i + 1] >> 1) & 0x1F;
			pstr_dest[j++] = base32char[index];//3/8
		}
		else {
			pstr_dest[j++] = base32char[index];//2/8
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			break;
		}
		index = (pstr_source[i+1] << 4) & 0x10;
		if (i + 2 < len) {//4/8
			index |= (pstr_source[i + 2] >> 4) & 0x0F;
			pstr_dest[j++] = base32char[index];//4/8
		}
		else {
			pstr_dest[j++] = base32char[index];//4/8
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			break;
		}
		index = (pstr_source[i + 2] << 1) & 0x1E;
		if (i + 3 < len) {//5-6/8
			index |= (pstr_source[i + 3] >> 7) & 0x01;
			pstr_dest[j++] = base32char[index];//5/8
			index = (pstr_source[i + 3] >> 2) & 0x1F;
			pstr_dest[j++] = base32char[index];//6/8
		}
		else {
			pstr_dest[j++] = base32char[index];//5/8
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			break;
		}
		index = (pstr_source[i + 3] << 3) & 0x18;
		if (i + 4 < len) {//7-8/8
			index |= (pstr_source[i + 4] >> 5) & 0x07;
			pstr_dest[j++] = base32char[index];//7/8
			index = (pstr_source[i + 4]) & 0x1F;
			pstr_dest[j++] = base32char[index];//8/8
		}
		else {
			pstr_dest[j++] = base32char[index];//7/8
			pstr_dest[j++] = padding_char;
			break;
		}
	}
	pstr_dest[j] = '\0';
	return 0;
}

int encode_base32(const std::string& str_source, std::string& str_dest) {
	if (str_source.empty()) {
		return -1;
	}
	unsigned len = encode_number + (str_source.length() - 1) * encode_number / source_number + 1;
	char *p = new char[len];
	if (p == nullptr) {
		return -2;
	}
	int nret = encode_base32(str_source.c_str(), p);
	str_dest = p;
	delete p;
	return nret;
}

int decode_base32(const char* pstr_source, char* pstr_dest){
	unsigned len = strlen(pstr_source);

	if (len == 0 || len % encode_number != 0) {
		return -1;
	}

	int i = 0, j = 0;
	unsigned char tempchar;
	for (; i < len; i += encode_number) {//encode_number个一组解析出3个源码
		unsigned char index0 = getCharIndex(pstr_source[i]);
		unsigned char index1 = getCharIndex(pstr_source[i + 1]);

		tempchar = (index0 << 3) & 0xF8; //mask 1111 1000 //  = 14 / >> 5 / & 2 / [] 1 / () 1 /  优先级决定了需要()
		//操作无符号char的移位，其实不用 与 mask了,默认的填充其实是填充0。 如果有有符号的，左边以符号位（即最高为）补齐，否则以0补齐 。避免编译器实现不同，还是手动mask一遍吧。
		if (pstr_source[i + 1] != padding_char) {
			tempchar |= (index1 >> 2) & 0x07;
			pstr_dest[j++] = tempchar;		//1/5
		}
		else {
			pstr_dest[j++] = tempchar;//1/5
			break;
		}
		unsigned char index2 = getCharIndex(pstr_source[i + 2]);
		tempchar = (index1 << 6) & 0xC0;
		if (pstr_source[i + 2] != padding_char) {
			tempchar |= (index2 << 1) & 0x3E;
		}
		else {
			pstr_dest[j++] = tempchar;//2/5
			break;
		}
		unsigned char index3 = getCharIndex(pstr_source[i + 3]);
		if (pstr_source[i + 3] != padding_char) {
			tempchar |= (index3 >> 4) & 0x01;
			pstr_dest[j++] = tempchar;//2/5
		}
		else {
			pstr_dest[j++] = tempchar;//2/5
			break;
		}
		unsigned char index4 = getCharIndex(pstr_source[i + 4]);
		tempchar = (index3 << 4) & 0xF0;
		if (pstr_source[i + 4] != padding_char) {
			tempchar |= (index4 >> 1) & 0x0F;
			pstr_dest[j++] = tempchar;//3/5
		}
		else {
			pstr_dest[j++] = tempchar;//3/5
			break;
		}

		unsigned char index5 = getCharIndex(pstr_source[i + 5]);
		tempchar = (index4 << 7) & 0x80;
		if (pstr_source[i + 5] != padding_char) {
			tempchar |= (index5 << 2) & 0x7C;
		}
		else {
			pstr_dest[j++] = tempchar;//4/5
			break;
		}
		unsigned char index6 = getCharIndex(pstr_source[i + 6]);
		if (pstr_source[i + 6] != padding_char) {
			tempchar |= (index6 >> 3) & 0x03;
			pstr_dest[j++] = tempchar;//4/5
		}
		else {
			pstr_dest[j++] = tempchar;//4/5
			break;
		}

		unsigned char index7 = getCharIndex(pstr_source[i + 7]);
		tempchar = (index6 << 5) & 0xE0;
		if (pstr_source[i + 7] != padding_char) {
			tempchar |= index7 & 0x1F;
			pstr_dest[j++] = tempchar;//5/5
		}
		else {
			pstr_dest[j++] = tempchar;//5/5
			break;
		}
	}
	pstr_dest[j] = '\0';

	return 0;
}

int decode_base32(const std::string& str_source, std::string& str_dest)
{
	if (str_source.length() == 0 || str_source.length() % encode_number != 0) {
		return -1;
	}
	unsigned int nlen = str_source.length() / encode_number * 3 + 1;
	char *pdest = new char[nlen + 1];
	if (pdest == nullptr) {
		return -2;
	}
	int ret = decode_base32(str_source.c_str(), pdest);
	str_dest = pdest;
	return ret;
}
