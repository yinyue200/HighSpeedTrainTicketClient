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

//xxHash
//Copyright(C) 2016 Stephan Brumme
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this softwareand associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "xxhash.h"

static const uint64_t xxhash_64_Prime1 = 11400714785074694791ULL;
static const uint64_t xxhash_64_Prime2 = 14029467366897019727ULL;
static const uint64_t xxhash_64_Prime3 = 1609587929392839161ULL;
static const uint64_t xxhash_64_Prime4 = 9650029242287828579ULL;
static const uint64_t xxhash_64_Prime5 = 2870177450012600261ULL;

static const uint32_t xxhash_32_Prime1 = 2654435761U;
static const uint32_t xxhash_32_Prime2 = 2246822519U;
static const uint32_t xxhash_32_Prime3 = 3266489917U;
static const uint32_t xxhash_32_Prime4 = 668265263U;
static const uint32_t xxhash_32_Prime5 = 374761393U;

// rotate bits, should compile to a single CPU instruction (ROL)
static inline uint64_t xxhash_rotateLeft64(uint64_t x, unsigned char bits) 
{ 
	return (x << bits) | (x >> (64 - bits)); 
}

// process a single 64 bit value   
static inline uint64_t xxhash_processSingle64(uint64_t previous, uint64_t input)
{     
	return xxhash_rotateLeft64(previous + input * xxhash_64_Prime2, 31) * xxhash_64_Prime1;
}
// process a block of 4x4 bytes, this is the main part of the XXHash32 algorithm
static inline void xxhash_process64(const void* data, uint64_t* state0, uint64_t* state1, uint64_t* state2, uint64_t* state3)
{ 
	const uint64_t* block = (const uint64_t*)data;
	*state0 = xxhash_processSingle64(*state0, block[0]);
	*state1 = xxhash_processSingle64(*state1, block[1]);
	*state2 = xxhash_processSingle64(*state2, block[2]);
	*state3 = xxhash_processSingle64(*state3, block[3]);
}
//create new XXHash (64 bit)
XXHASH64 CreateXXHash64(uint64_t seed)
{ 
	XXHASH64 xxhash;
	xxhash.state[0] = seed + xxhash_64_Prime1 + xxhash_64_Prime2;
	xxhash.state[1] = seed + xxhash_64_Prime2;
	xxhash.state[2] = seed;
	xxhash.state[3] = seed - xxhash_64_Prime1;
	xxhash.bufferSize = 0;
	xxhash.totalLength = 0;
	return xxhash;
}
/// add a chunk of bytes   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@return false if parameters are invalid / zero **/   
bool xxhash_add64(XXHASH64* hash, const void* input, size_t length)
{     
	// no data ?     
	if (!input || length == 0)       
		return false;     
	hash->totalLength += length;
	// byte-wise access     
	const unsigned char* data = (const unsigned char*)input;     
	// unprocessed old data plus new data still fit in temporary buffer ?     
	if (hash->bufferSize + length < XXHASH_MAXBUFFERSIZE64)
	{       
		// just add new data       
		while (length-- > 0)         
			hash->buffer[hash->bufferSize++] = *data++;
		return true;     
	}     
	// point beyond last byte     
	const unsigned char* stop      = data + length;     
	const unsigned char* stopBlock = stop - XXHASH_MAXBUFFERSIZE64;
	// some data left from previous update ?     
	if (hash->bufferSize > 0)
	{       
		// make sure temporary buffer is full (16 bytes)       
		while (hash->bufferSize < XXHASH_MAXBUFFERSIZE64)
			hash->buffer[hash->bufferSize++] = *data++;
		// process these 32 bytes (4x8)       
		xxhash_process64(hash->buffer, &hash->state[0], &hash->state[1], &hash->state[2], &hash->state[3]);
	}     
	// copying state to local variables helps optimizer A LOT     
	uint64_t s0 = hash->state[0], s1 = hash->state[1], s2 = hash->state[2], s3 = hash->state[3];
	// 32 bytes at once     
	while (data <= stopBlock)     
	{       
		// local variables s0..s3 instead of state[0]..state[3] are much faster       
		xxhash_process64(data, &s0, &s1, &s2, &s3);
		data += 32;     
	}     
	// copy back     
	hash->state[0] = s0;
	hash->state[1] = s1;
	hash->state[2] = s2;
	hash->state[3] = s3;

	// copy remainder to temporary buffer     
	hash->bufferSize = stop - data;
	for (uint64_t i = 0; i < hash->bufferSize; i++)
		hash->buffer[i] = data[i];// done     
	return true;   
}

// get current hash   
/** @return 64 bit XXHash **/   
uint64_t xxhash_hash64(XXHASH64* hash)
{
	// fold 256 bit state into one single 64 bit value     
	uint64_t result;     
	if (hash->totalLength >= XXHASH_MAXBUFFERSIZE64)
	{
		result = xxhash_rotateLeft64(hash->state[0],  1) +
				 xxhash_rotateLeft64(hash->state[1],  7) +
			     xxhash_rotateLeft64(hash->state[2], 12) +
			     xxhash_rotateLeft64(hash->state[3], 18);
		result = (result ^ xxhash_processSingle64(0, hash->state[0])) * xxhash_64_Prime1 + xxhash_64_Prime4;
		result = (result ^ xxhash_processSingle64(0, hash->state[1])) * xxhash_64_Prime1 + xxhash_64_Prime4;
		result = (result ^ xxhash_processSingle64(0, hash->state[2])) * xxhash_64_Prime1 + xxhash_64_Prime4;
		result = (result ^ xxhash_processSingle64(0, hash->state[3])) * xxhash_64_Prime1 + xxhash_64_Prime4;
	}
	else     
	{ 
		// internal state wasn't set in add(), therefore original seed is still stored in state2       
		result = hash->state[2] + xxhash_64_Prime5;
	}     
	result += hash->totalLength;
	// process remaining bytes in temporary buffer     
	const unsigned char* data = hash->buffer;
	// point beyond last byte     
	const unsigned char* stop = data + hash->bufferSize;
	// at least 8 bytes left ? => eat 8 bytes per step     
	for (; data + 8 <= stop; data += 8)       
		result = xxhash_rotateLeft64(result ^ xxhash_processSingle64(0, *(uint64_t*)data), 27) * xxhash_64_Prime1 + xxhash_64_Prime4;
	// 4 bytes left ? => eat those     
	if (data + 4 <= stop)     
	{  
		result = xxhash_rotateLeft64(result ^ (*(uint32_t*)data) * xxhash_64_Prime1,   23) * xxhash_64_Prime2 + xxhash_64_Prime3;
		data  += 4;     
	}     
	// take care of remaining 0..3 bytes, eat 1 byte per step     
	while (data != stop)       
		result = xxhash_rotateLeft64(result ^ (*data++) * xxhash_64_Prime5, 11) * xxhash_64_Prime1;
	// mix bits
	result ^= result >> 33;     
	result *= xxhash_64_Prime2;
	result ^= result >> 29;     
	result *= xxhash_64_Prime3;
	result ^= result >> 32;     
	return result;
}
/// combine constructor, add() and hash() in one static function (C style)   
/** @param  input  pointer to a continuous block of data       
@param  length number of bytes       
@param  seed your seed value, e.g. zero is a valid seed      
@return 64 bit XXHash 
**/   
uint64_t xxhash_hash64_once(const void* input, size_t length, uint64_t seed)
{     
	XXHASH64 hasher = CreateXXHash64(seed);
	xxhash_add64(&hasher, input, length);
	return xxhash_hash64(&hasher);
}

//create new XXHash (32 bit)
XXHASH32 CreateXXHash32(uint32_t seed)
{
	XXHASH32 xxhash;
	xxhash.state[0] = seed + xxhash_32_Prime1 + xxhash_32_Prime2;
	xxhash.state[1] = seed + xxhash_32_Prime2;
	xxhash.state[2] = seed;
	xxhash.state[3] = seed - xxhash_32_Prime1;
	xxhash.bufferSize = 0;
	xxhash.totalLength = 0;
	return xxhash;
}

/// combine constructor, add() and hash() in one static function (C style)   
/** @param  input  pointer to a continuous block of data
@param  length number of bytes
@param  seed your seed value, e.g. zero is a valid seed
@return 32 bit XXHash
**/
uint32_t xxhash_hash32_once(const void* input, size_t length, uint32_t seed)
{
	XXHASH32 hasher = CreateXXHash32(seed);
	xxhash_add32(&hasher, input, length);
	return xxhash_hash32(&hasher);
}

/// rotate bits, should compile to a single CPU instruction (ROL)
static inline uint32_t xxhash_rotateLeft32(uint32_t x, unsigned char bits)
{
	return (x << bits) | (x >> (32 - bits));
}

/// process a block of 4x4 bytes, this is the main part of the XXHash32 algorithm
static inline void xxhash_process32(const void* data, uint32_t* state0, uint32_t* state1, uint32_t* state2, uint32_t* state3)
{
	const uint32_t* block = (const uint32_t*)data;
	*state0 = xxhash_rotateLeft32(*state0 + block[0] * xxhash_32_Prime2, 13) * xxhash_32_Prime1;
	*state1 = xxhash_rotateLeft32(*state1 + block[1] * xxhash_32_Prime2, 13) * xxhash_32_Prime1;
	*state2 = xxhash_rotateLeft32(*state2 + block[2] * xxhash_32_Prime2, 13) * xxhash_32_Prime1;
	*state3 = xxhash_rotateLeft32(*state3 + block[3] * xxhash_32_Prime2, 13) * xxhash_32_Prime1;
}


/// add a chunk of bytes
  /** @param  input  pointer to a continuous block of data
	  @param  length number of bytes
	  @return false if parameters are invalid / zero **/
bool xxhash_add32(XXHASH32* hash, const void* input, size_t length)
{
	// no data ?
	if (!input || length == 0)
		return false;

	hash->totalLength += length;
	// byte-wise access
	const unsigned char* data = (const unsigned char*)input;

	// unprocessed old data plus new data still fit in temporary buffer ?
	if (hash->bufferSize + length < XXHASH_MAXBUFFERSIZE32)
	{
		// just add new data
		while (length-- > 0)
			hash->buffer[hash->bufferSize++] = *data++;
		return true;
	}

	// point beyond last byte
	const unsigned char* stop = data + length;
	const unsigned char* stopBlock = stop - XXHASH_MAXBUFFERSIZE32;

	// some data left from previous update ?
	if (hash->bufferSize > 0)
	{
		// make sure temporary buffer is full (16 bytes)
		while (hash->bufferSize < XXHASH_MAXBUFFERSIZE32)
			hash->buffer[hash->bufferSize++] = *data++;

		// process these 16 bytes (4x4)
		xxhash_process32(hash->buffer, &hash->state[0], &hash->state[1], &hash->state[2], &hash->state[3]);
	}

	// copying state to local variables helps optimizer A LOT
	uint32_t s0 = hash->state[0], s1 = hash->state[1], s2 = hash->state[2], s3 = hash->state[3];
	// 16 bytes at once
	while (data <= stopBlock)
	{
		// local variables s0..s3 instead of state[0]..state[3] are much faster
		xxhash_process32(data, &s0, &s1, &s2, &s3);
		data += 16;
	}
	// copy back
	hash->state[0] = s0; hash->state[1] = s1; hash->state[2] = s2; hash->state[3] = s3;

	// copy remainder to temporary buffer
	hash->bufferSize = stop - data;
	for (unsigned int i = 0; i < hash->bufferSize; i++)
		hash->buffer[i] = data[i];

	// done
	return true;
}

/// get current hash
/** @return 32 bit XXHash **/
uint32_t xxhash_hash32(XXHASH32* hash)
{
	uint32_t result = (uint32_t)hash->totalLength;

	// fold 128 bit state into one single 32 bit value
	if (hash->totalLength >= XXHASH_MAXBUFFERSIZE32)
		result += xxhash_rotateLeft32(hash->state[0], 1) +
				  xxhash_rotateLeft32(hash->state[1], 7) +
		          xxhash_rotateLeft32(hash->state[2], 12) +
		          xxhash_rotateLeft32(hash->state[3], 18);
	else
		// internal state wasn't set in add(), therefore original seed is still stored in state2
		result += hash->state[2] + xxhash_32_Prime5;

	// process remaining bytes in temporary buffer
	const unsigned char* data = hash->buffer;
	// point beyond last byte
	const unsigned char* stop = data + hash->bufferSize;

	// at least 4 bytes left ? => eat 4 bytes per step
	for (; data + 4 <= stop; data += 4)
		result = xxhash_rotateLeft32(result + *(uint32_t*)data * xxhash_32_Prime3, 17) * xxhash_32_Prime4;

	// take care of remaining 0..3 bytes, eat 1 byte per step
	while (data != stop)
		result = xxhash_rotateLeft32(result + (*data++) * xxhash_32_Prime5, 11) * xxhash_32_Prime1;

	// mix bits
	result ^= result >> 15;
	result *= xxhash_32_Prime2;
	result ^= result >> 13;
	result *= xxhash_32_Prime3;
	result ^= result >> 16;
	return result;
}