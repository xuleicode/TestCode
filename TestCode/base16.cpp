#include "base16.h"
#include <string>
//ȫ�ֳ���
static const char *base16char = "ABCDEFGHIJKLMNOP";
static const char  padding_char = '=';
static const unsigned int encode_number = 2;
static const unsigned int source_number = 1;
//��ȡchar������
static char getCharIndex(char c)
{
	for (unsigned int i = 0; i < strlen(base16char); ++i) {
		if (c == *(base16char + i)) {
			return i & 0x0F; //0000 1111
		}
	}
	return -1;
}
int encode_base16(const char* pstr_source, char* pstr_dest) {
	if (pstr_source == nullptr) {
		return -1;
	}
	if (pstr_dest == nullptr) {
		return -2;
	}
	const size_t len = strlen(pstr_source);
	unsigned int i = 0, j = 0;
	unsigned char index = 0;

	//�����޷���char����λ����ʵ���� �� mask��,Ĭ�ϵ������ʵ�����0�� ������з��ŵģ�����Է���λ�������Ϊ�����룬������0���� �����������ʵ�ֲ�ͬ�������ֶ�maskһ��ɡ�

	for (; i < len; i += source_number) { //source_number��һ�飬��encode_number������
		index = (pstr_source[i] >> 4) & 0x0F;			//  = 14 / >> 5 / & 2 / [] 1 / () 1 / ��������ȼ����⣬һ��Ҫ��ӣ���
		pstr_dest[j++] = base16char[index];//1/2
		index = (pstr_source[i] ) & 0x0F;
		pstr_dest[j++] = base16char[index];//1/2
	}
	pstr_dest[j] = '\0';
	return 0;
}

int encode_base16(const std::string& str_source, std::string& str_dest) {
	if (str_source.empty()) {
		return -1;
	}
	unsigned len = encode_number + (str_source.length() - 1) * encode_number / source_number + 1;
	char *p = new char[len];
	if (p == nullptr) {
		return -2;
	}
	int nret = encode_base16(str_source.c_str(), p);
	str_dest = p;
	delete p;
	return nret;
}

int decode_base16(const char* pstr_source, char* pstr_dest){
	unsigned len = strlen(pstr_source);

	if (len == 0 || len % encode_number != 0) {
		return -1;
	}

	int i = 0, j = 0;
	unsigned char tempchar;
	for (; i < len; i += encode_number) {//encode_number��һ�������3��Դ��
		unsigned char index0 = getCharIndex(pstr_source[i]);
		unsigned char index1 = getCharIndex(pstr_source[i + 1]);

		tempchar = (index0 << 4) & 0xF0; //mask 1111 1000 //  = 14 / >> 5 / & 2 / [] 1 / () 1 /  ���ȼ���������Ҫ()
	    tempchar |= (index1) & 0x0F;
		pstr_dest[j++] = tempchar;		//1/5
	}
	pstr_dest[j] = '\0';

	return 0;
}

int decode_base16(const std::string& str_source, std::string& str_dest)
{
	if (str_source.length() == 0 || str_source.length() % encode_number != 0) {
		return -1;
	}
	unsigned int nlen = str_source.length() / encode_number * 3 + 1;
	char *pdest = new char[nlen + 1];
	if (pdest == nullptr) {
		return -2;
	}
	int ret = decode_base16(str_source.c_str(), pdest);
	str_dest = pdest;
	return ret;
}
