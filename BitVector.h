#pragma once
#include "vector.h"
#include <stdbool.h>
typedef struct BitVector
{
	vector vector;
	size_t size;
} BITVECTOR;
BITVECTOR BitVector_CreateBitVector(int size);
void BitVector_AddBit(BITVECTOR* vector, bool bit);
bool BitVector_GetBit(BITVECTOR* vector, int pos);
void BitVector_SetBit(BITVECTOR* vector, int pos, bool value);
void BitVector_Free(BITVECTOR* vector);
void BitVector_AndBitVector(BITVECTOR* vector, BITVECTOR* right);
void BitVector_OrBitVector(BITVECTOR* vector, BITVECTOR* right);
