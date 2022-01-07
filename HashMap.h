#pragma once
//  HighSpeedTrainTicketClient
//  Copyright(C) 2022 ��Խ
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
#define HASHMAP_INIT_SIZE 4 //HASHMAP ��С��ʼ��С
typedef bool (*HashMap_IsKeyEqualFunc)(void* objkey, void* searchkey);
typedef uint64_t (*HashMap_HashKeyFunc)(void* key);//����ָ��Ԫ�� Key �� HASH �ĺ���
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
} HASHMAP;
//����һ���µ� hashmap
//@param ��ʼ��С
//@param hash func
//@param �Դ���Ĳ�������hash�ĺ���ָ��
//@param �ж����� key �Ƿ���ȵĺ���ָ��
//@param ͨ��Ԫ�ػ�ȡִ��key��ָ��ĺ���ָ��
//@param ����Ԫ�صĺ���ָ��
HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_HashKeyFunc parhash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc);
//����һ�� hashmap
//@param Ҫ������ hashmap
void HashMap_Free(HASHMAP *map);
//���� hashmap ���Ƿ������ָ�� key ��Ԫ��
bool HashMap_ContainKey(HASHMAP* map, void* key);
//��ȡ hashmap ���Ƿ������ָ�� key ��Ԫ��
void* HashMap_GetByKey(HASHMAP* map, void* key);
/// <summary>
/// �� hashmap ��ɾ��ָ����Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="item">Ҫɾ����Ԫ��</param>
/// <returns>��ɾ��Ԫ�ص�ָ��</returns>
void* HashMap_RemoveItem(HASHMAP* map, void* item);
//�� hashmap ��ɾ��ָ�� key ��Ԫ��
void* HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item);
//�� hashmap ��ɾ������ָ�� key ��Ԫ��
void* HashMap_RemoveByKey(HASHMAP* map, void* key);
void HashMap_CheckAndResize(HASHMAP* map);
//��ȡָ�� key ��Ԫ��ָ��
//@param hashmap
//@param ָ���� key
//@param �Ƿ��������Ҳ���ָ�� key ��ָ��ʱԤ���µ�Ԫ��λ����ʹ�÷���ֵ��Ϊ��
//@return ָ��Ԫ��ָ���ָ��
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd);
//�ú��������ڻ�ȡָ�� key ������Ԫ��
//@param hashmap
//@param ָ���� key
//@param lastnode
//@param �� lastnode Ϊ NULL ʱ *maxposscount ������Ϊ�ù�ϣ���� key Ϊ�� key �������ܵ�Ԫ������
//@return �� lastnode ��Ϊ NULL ʱ������ hashcode ���׸� key Ϊ�� key ��Ԫ�أ����򷵻�lastnode����һ�� key Ϊ�� key ��Ԫ��
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE* lastnode, size_t* maxposscount);
//���¼����ϣ����Ԫ�صĹ�ϣֵ
void HashMap_Rehash(HASHMAP* map);
//�ڹ�ϣ��������µ�Ԫ��
void HashMap_Add(HASHMAP* map, void* item);
inline void HashMap_NoFree(void* obj){}
inline void* HashMap_GetKeyNone(void* obj) { return obj; }

