
#ifndef PORT_H_
#define PORT_H_

char * strstr_s(const char * string, size_t slen, const char * find);

char * strchr_s(const char * string, size_t slen, int ch);

char * strncpy_s_(char * _Dst, size_t dlen, const char * src, size_t slen);
#endif