#pragma once
#ifndef sha256_h
#define sha256_h

#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include <stdlib.h>
//#include<endian.h>
void sha256(const unsigned char* data, size_t len, unsigned char* out);

#endif /* sha256_h */
