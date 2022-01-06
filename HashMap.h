#pragma once
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

#include <stdint.h>
#include <stdbool.h>
#include "vector.h"
#define HASHMAP_INIT_SIZE 4
typedef bool (*HashMap_IsKeyEqualFunc)(void* left, void* right);
typedef uint64_t (*HashMap_HashKeyFunc)(void* key);//计算指定元素 Key 的 HASH 的函数
typedef void* (*HashMap_GetKeyFunc)(void* obj);
typedef void (*HashMap_FreeItemFunc)(void* obj);
typedef struct HashMapNode
{
    void* value;
    uint64_t hashvalue;
    struct HashMapNode* next;
} HASHMAPNODE;
typedef struct HashMapNodeBasic
{
    HASHMAPNODE node;
    size_t used;
} HASHMAPNODEBASIC;
typedef struct HashMap
{
    HASHMAPNODEBASIC* item;
    size_t listsize;
    size_t count;
    HashMap_HashKeyFunc hashKeyFunc;
    HashMap_IsKeyEqualFunc equalFunc;
    HashMap_GetKeyFunc getKeyFunc;
    HashMap_FreeItemFunc delKeyFunc;
} HASHMAP;
HASHMAP HashMap_Create(size_t size,HashMap_HashKeyFunc hash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc);
void HashMap_Free(HASHMAP *map);
bool HashMap_ContainKey(HASHMAP* map, void* key);
void* HashMap_GetByKey(HASHMAP* map, void* key);
void* HashMap_RemoveByKey(HASHMAP* map, void* key);
void HashMap_CheckAndResize(HASHMAP* map);
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd);
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE* lastnode, size_t* maxposscount);
void HashMap_Rehash(HASHMAP* map);
void HashMap_Add(HASHMAP* map, void* item);
inline void HashSet_NoFree(void* obj){}

