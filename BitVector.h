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
#pragma once
#include "vector.h"
#include <stdbool.h>
typedef struct BitVector
{
	vector data;
	size_t size;
} BITVECTOR;
BITVECTOR BitVector_CreateBitVector(int size);
void BitVector_AddBit(BITVECTOR* vector, bool bit);
bool BitVector_GetBit(BITVECTOR* vector, int pos);
void BitVector_SetBit(BITVECTOR* vector, int pos, bool value);
/// <summary>
/// 析构 BITVECTOR
/// </summary>
/// <param name="vector">要析构的 BITVECTOR</param>
void BitVector_Free(BITVECTOR* vector);
/// <summary>
/// vector&=right
/// </summary>
/// <param name="vector"></param>
/// <param name="right"></param>
void BitVector_AndBitVector(BITVECTOR* vector, BITVECTOR* right);
/// <summary>
/// vector|=right
/// </summary>
/// <param name="vector"></param>
/// <param name="right"></param>
void BitVector_OrBitVector(BITVECTOR* vector, BITVECTOR* right);
