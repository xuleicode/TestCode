#include "base64.h"
#include <string>
//全局常量
const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char  padding_char = '=';
//获取char的索引
static char getCharIndex(char c)
{
	for (unsigned int i = 0; i < strlen(base64char); ++i) {
		if (c == *(base64char + i)) {
			return i & 0x3F; //0011 1111
		}
	}
	return -1;
}
int encode_base64(const char* pstr_source, char* pstr_dest) {
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

	for (; i < len; i += 3) { //3个一组，出4个编码
		index = (pstr_source[i] >> 2) & 0x3F;			//  = 14 / >> 5 / & 2 / [] 1 / () 1 / 运算符优先级问题，一定要添加（）
		pstr_dest[j++] = base64char[index];
		index = (pstr_source[i] << 4) & 0x30;
		if (i + 1 < len) {
			index |= (pstr_source[i + 1] >> 4) & 0x0F;  // != 14 / >> 5 / & 2 / [] 1 / () 1 /
			pstr_dest[j++] = base64char[index];
		}
		else {
			pstr_dest[j++] = base64char[index];
			pstr_dest[j++] = padding_char;
			pstr_dest[j++] = padding_char;
			break;
		}
		index = (pstr_source[i + 1] << 2) & 0x3C;
		if (i + 2 < len) {
			index |= (pstr_source[i + 2] >> 6) & 0x03;
			pstr_dest[j++] = base64char[index];
			index = pstr_source[i + 2] & 0x3F;
			pstr_dest[j++] = base64char[index];
		}
		else {
			pstr_dest[j++] = base64char[index];
			pstr_dest[j++] = padding_char;
		}
	}
	pstr_dest[j] = '\0';
	return 0;
}

int encode_base64(const std::string& str_source, std::string& str_dest) {
	if (str_source.empty()) {
		return -1;
	}
	unsigned len = 4 + (str_source.length() - 1) * 4 / 3 + 1;
	char *p = new char[len];
	if (p == nullptr) {
		return -2;
	}
	int nret = encode_base64(str_source.c_str(), p);
	str_dest = p;
	delete p;
	return nret;
}

int decode_base64(const char* pstr_source, char* pstr_dest){
	unsigned len = strlen(pstr_source);

	if (len == 0 || len % 4 != 0) {
		return -1;
	}

	int i = 0, j = 0;
	unsigned char tempchar;
	for (; i < len; i += 4) {//4个一组解析出3个源码
		char index0 = getCharIndex(pstr_source[i]);
		char index1 = getCharIndex(pstr_source[i + 1]);

		tempchar = (index0 << 2) & 0xFC; //mask 1111 1100 //  = 14 / >> 5 / & 2 / [] 1 / () 1 /  优先级决定了需要()
		//操作无符号char的移位，其实不用 与 mask了,默认的填充其实是填充0。 如果有有符号的，左边以符号位（即最高为）补齐，否则以0补齐 。避免编译器实现不同，还是手动mask一遍吧。
		//1/3
		if (pstr_source[i + 1] != padding_char) {
			tempchar |= (index1 >> 4) & 0x03;
			pstr_dest[j++] = tempchar;
		}
		else {
			pstr_dest[j++] = tempchar;
			break;
		}
		//1/3
		char index2 = getCharIndex(pstr_source[i + 2]);
		tempchar = (index1 << 4) & 0xF0;
		if (pstr_source[i + 2] != padding_char) {
			tempchar |= (index2 >> 2) & 0x0F;
			pstr_dest[j++] = tempchar;
		}
		else {
			pstr_dest[j++] = tempchar;
			break;
		}
		//1/3
		tempchar = (index2 << 6) & 0xC0;
		char index3 = getCharIndex(pstr_source[i + 3]);
		if (pstr_source[i + 3] != padding_char) {
			tempchar |= index3 & 0x3F;
			pstr_dest[j++] = tempchar;
		}
		else {
			pstr_dest[j++] = tempchar;
			break;
		}

	}
	pstr_dest[j] = '\0';

	return 0;
}

int decode_base64(const std::string& str_source, std::string& str_dest)
{
	if (str_source.length() == 0 || str_source.length() % 4 != 0) {
		return -1;
	}
	unsigned int nlen = str_source.length() / 4 * 3 + 1;
	char *pdest = new char[nlen + 1];
	if (pdest == nullptr) {
		return -2;
	}
	int ret = decode_base64(str_source.c_str(), pdest);
	str_dest = pdest;
	return ret;
}
