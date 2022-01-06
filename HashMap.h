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
#define HASHMAP_INIT_SIZE 4 //HASHMAP 最小初始大小
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
//创建一个新的 hashmap
//@param 初始大小
//@param 判断两个 key 是否相等的函数指针
//@param 通过元素获取执行key的指针的函数指针
//@param 析构元素的函数指针
HASHMAP HashMap_Create(size_t size,HashMap_HashKeyFunc hash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc);
//析构一个 hashmap
//@param 要析构的 hashmap
void HashMap_Free(HASHMAP *map);
//返回 hashmap 中是否包含有指定 key 的元素
bool HashMap_ContainKey(HASHMAP* map, void* key);
//获取 hashmap 中是否包含有指定 key 的元素
void* HashMap_GetByKey(HASHMAP* map, void* key);
//从 hashmap 中删除所有指定 key 的元素
void* HashMap_RemoveByKey(HASHMAP* map, void* key);
void HashMap_CheckAndResize(HASHMAP* map);
//获取指定 key 的元素指针
//@param hashmap
//@param 指定的 key
//@param 是否允许在找不到指定 key 的指针时预留新的元素位置以使得返回值不为空
//@return 指向元素指针的指针
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd);
//该函数可用于获取指定 key 的所有元素
//@param hashmap
//@param 指定的 key
//@param lastnode
//@param 当 lastnode 为 NULL 时 *maxposscount 将设置为该哈希表中 key 为该 key 的最多可能的元素数量
//@return 当 lastnode 不为 NULL 时，返回 hashcode 中首个 key 为该 key 的元素，否则返回lastnode的下一个 key 为该 key 的元素
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE* lastnode, size_t* maxposscount);
//重新计算哈希表中元素的哈希值
void HashMap_Rehash(HASHMAP* map);
//在哈希表中添加新的元素
void HashMap_Add(HASHMAP* map, void* item);
inline void HashSet_NoFree(void* obj){}

