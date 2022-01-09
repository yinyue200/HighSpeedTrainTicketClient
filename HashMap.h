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
typedef bool (*HashMap_IsKeyEqualFunc)(void* objkey, void* searchkey);
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
    HashMap_HashKeyFunc parHashKeyFunc;
    HashMap_HashKeyFunc hashKeyFunc;
    HashMap_IsKeyEqualFunc equalFunc;
    HashMap_GetKeyFunc getKeyFunc;
    HashMap_FreeItemFunc delKeyFunc;
    /// <summary>
    /// 值越大扩容越慢
    /// </summary>
    int coefficient;
} HASHMAP;
/// <summary>
/// 创建一个新的 hashmap
/// </summary>
/// <param name="size">初始大小</param>
/// <param name="hash">对元素计算hash的函数指针</param>
/// <param name="parhash">对传入的参数计算hash的函数指针</param>
/// <param name="equal">判断两个 key 是否相等的函数指针</param>
/// <param name="getkey">通过元素获取执行key的指针的函数指针</param>
/// <param name="delfunc">析构元素的函数指针</param>
/// <returns>创建的 hashmap</returns>
HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_HashKeyFunc parhash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc);
HASHMAP HashMap_SetCoefficient(HASHMAP* map, int coefficient);
/// <summary>
/// 析构一个 hashmap
/// </summary>
/// <param name="map">要析构的 hashmap</param>
void HashMap_Free(HASHMAP *map);
/// <summary>
/// 返回 hashmap 中是否包含有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
bool HashMap_ContainKey(HASHMAP* map, void* key);
/// <summary>
/// 获取 hashmap 中是否包含有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void* HashMap_GetByKey(HASHMAP* map, void* key);
/// <summary>
/// 从 hashmap 中删除指定的元素
/// </summary>
/// <param name="map"></param>
/// <param name="item">要删除的元素</param>
void HashMap_RemoveItem(HASHMAP* map, void* item);
/// <summary>
/// 从 hashmap 中删除指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <param name="item"></param>
void HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item);
/// <summary>
/// 从 hashmap 中删除所有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
void HashMap_RemoveByKey(HASHMAP* map, void* key);
void HashMap_CheckAndResize(HASHMAP* map);
/// <summary>
/// 获取指定 key 的元素指针
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">指定的 key</param>
/// <param name="allowadd">是否允许在找不到指定 key 的指针时预留新的元素位置以使得返回值不为空</param>
/// <returns>指向元素指针的指针</returns>
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd);
/// <summary>
/// 该函数可用于获取指定 key 的所有元素
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">指定的 key</param>
/// <param name="lastnode">lastnode</param>
/// <param name="maxposscount">当 lastnode 为 NULL 时 *maxposscount 将设置为该哈希表中 key 为该 key 的最多可能的元素数量</param>
/// <returns>当 lastnode 不为 NULL 时，返回 hashcode 中首个 key 为该 key 的元素，否则返回lastnode的下一个 key 为该 key 的元素</returns>
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE* lastnode, size_t* maxposscount);
/// <summary>
/// 重新计算哈希表中元素的哈希值
/// </summary>
/// <param name="map"></param>
void HashMap_Rehash(HASHMAP* map);
/// <summary>
/// 在哈希表中添加新的元素
/// </summary>
/// <param name="map"></param>
/// <param name="item"></param>
void HashMap_Add(HASHMAP* map, void* item);
/// <summary>
/// 什么都不做
/// </summary>
/// <param name="obj"></param>
inline void HashMap_NoFree(void* obj){}
/// <summary>
/// 返回唯一的参数
/// </summary>
/// <param name="obj"></param>
/// <returns>与 obj 相同</returns>
inline void* HashMap_GetKeyNone(void* obj) { return obj; }

