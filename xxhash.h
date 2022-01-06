//  HighSpeedTrainTicketClient
//	Copyright(C) 2022 “Û‘Ω
//
//	This program is free software : you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program. If not, see <https://www.gnu.org/licenses/>.
#pragma once
#include <stdint.h>
#include <stdbool.h>
// temporarily store up to 31 bytes between multiple add() calls   
#define XXHASH_MAXBUFFERSIZE64 31+1

typedef struct XXHash64
{
	uint64_t      state[4];
	unsigned char buffer[XXHASH_MAXBUFFERSIZE64];
	size_t      bufferSize;
	size_t      totalLength;
} XXHASH64;

// temporarily store up to 15 bytes between multiple add() calls   
#define XXHASH_MAXBUFFERSIZE32 15+1

typedef struct XXHash32
{
	uint32_t      state[4];
	unsigned char buffer[XXHASH_MAXBUFFERSIZE32];
	size_t      bufferSize;
	size_t      totalLength;
} XXHASH32;

//create new XXHash (64 bit)
XXHASH64 CreateXXHash64(uint64_t seed);
/// add a chunk of bytes   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@return false if parameters are invalid / zero **/
bool xxhash_add64(XXHASH64* hash, const void* input, size_t length);
// get current hash   
/** @return 64 bit XXHash **/
uint64_t xxhash_hash64(XXHASH64* hash);
/// combine constructor, add() and hash() in one static function (C style)   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@param  seed your seed value, e.g. zero is a valid seed
@return 64 bit XXHash
**/
uint64_t xxhash_hash64_once(const void* input, size_t length, uint64_t seed);

//create new XXHash (32 bit)
XXHASH32 CreateXXHash32(uint32_t seed);

/// add a chunk of bytes   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@return false if parameters are invalid / zero **/
bool xxhash_add32(XXHASH32* hash, const void* input, size_t length);
// get current hash   
/** @return 32 bit XXHash **/
uint32_t xxhash_hash32(XXHASH32* hash);
/// combine constructor, add() and hash() in one static function (C style)   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@param  seed your seed value, e.g. zero is a valid seed
@return 32 bit XXHash
**/
uint32_t xxhash_hash32_once(const void* input, size_t length, uint32_t seed);
