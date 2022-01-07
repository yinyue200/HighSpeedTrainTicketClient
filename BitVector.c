//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 殷越
//
//  This program is free software : you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.
#include "vector.h"
#include "BitVector.h"
int BitVector_GetRequired64BitsUnitCount(int size)
{
	return size / 64 + 1;
}
BITVECTOR BitVector_CreateBitVector(int size)
{
	BITVECTOR vector;
	vector.size = size;
	int intcount = BitVector_GetRequired64BitsUnitCount(size);
	vector_initwithcap(&vector.vector, intcount);
	for (int i = 0; i < intcount; i++)
	{
		vector_add_uint64_t(&vector, 0);
	}
	return vector;
}
bool BitVector_GetBit(BITVECTOR* vector, int pos)
{
	int index = pos / 64;
	int bitindex = pos % 64;

	uint64_t value = vector_get_uint64_t(&vector->vector, pos);

	value = value >> bitindex;
	value &= 0X0000000000000001ul;//取最后一位

	return value;
}

void BitVector_SetBit(BITVECTOR* vector, int pos, bool bit)
{
	int index = pos / 64;
	int bitindex = pos % 64;

	uint64_t setbitmask = 0x1ull << bitindex;
	if (bit)
	{
		*vector_getPointer_uint64_t(&vector->vector, index) |= setbitmask;
	}
	else
	{
		setbitmask = ~setbitmask;
		*vector_getPointer_uint64_t(&vector->vector, index) &= setbitmask;
	}
}

void BitVector_Free(BITVECTOR* vector)
{
	vector_free_uint64_t(&vector->vector);
}

void BitVector_AddBit(BITVECTOR* vector, bool bit)
{
	int osize = vector->size;
	vector->size++;
	int nsize = BitVector_GetRequired64BitsUnitCount(vector->size);
	if (nsize > vector_total_uint64_t(&vector->vector))
	{
		vector_add_uint64_t(&vector->vector, 0);
	}
	BitVector_SetBit(osize, bit, vector);
}

void BitVector_AndBitVector(BITVECTOR* vector, BITVECTOR* right)
{
	int size = min(vector_total(&vector->vector), vector_total(&right->vector));
	for (int i = 0; i < size; i++)
	{
		(*vector_getPointer_uint64_t(&vector->vector, i)) &= vector_get_uint64_t(&right->vector, i);
	}
}
void BitVector_OrBitVector(BITVECTOR* vector, BITVECTOR* right)
{
	int size = min(vector_total(&vector->vector), vector_total(&right->vector));
	for (int i = 0; i < size; i++)
	{
		(*vector_getPointer_uint64_t(&vector->vector, i)) |= vector_get_uint64_t(&right->vector, i);
	}
}
