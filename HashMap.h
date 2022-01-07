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
/// <summary>
/// ����һ���µ� hashmap
/// </summary>
/// <param name="size">��ʼ��С</param>
/// <param name="hash">��Ԫ�ؼ���hash�ĺ���ָ��</param>
/// <param name="parhash">�Դ���Ĳ�������hash�ĺ���ָ��</param>
/// <param name="equal">�ж����� key �Ƿ���ȵĺ���ָ��</param>
/// <param name="getkey">ͨ��Ԫ�ػ�ȡִ��key��ָ��ĺ���ָ��</param>
/// <param name="delfunc">����Ԫ�صĺ���ָ��</param>
/// <returns>������ hashmap</returns>
HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_HashKeyFunc parhash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc);
/// <summary>
/// ����һ�� hashmap
/// </summary>
/// <param name="map">Ҫ������ hashmap</param>
void HashMap_Free(HASHMAP *map);
/// <summary>
/// ���� hashmap ���Ƿ������ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
bool HashMap_ContainKey(HASHMAP* map, void* key);
/// <summary>
/// ��ȡ hashmap ���Ƿ������ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void* HashMap_GetByKey(HASHMAP* map, void* key);
/// <summary>
/// �� hashmap ��ɾ��ָ����Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="item">Ҫɾ����Ԫ��</param>
/// <returns>��ɾ��Ԫ�ص�ָ��</returns>
void* HashMap_RemoveItem(HASHMAP* map, void* item);
/// <summary>
/// �� hashmap ��ɾ��ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <param name="item"></param>
/// <returns></returns>
void* HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item);
/// <summary>
/// �� hashmap ��ɾ������ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void* HashMap_RemoveByKey(HASHMAP* map, void* key);
void HashMap_CheckAndResize(HASHMAP* map);
/// <summary>
/// ��ȡָ�� key ��Ԫ��ָ��
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">ָ���� key</param>
/// <param name="allowadd">�Ƿ��������Ҳ���ָ�� key ��ָ��ʱԤ���µ�Ԫ��λ����ʹ�÷���ֵ��Ϊ��</param>
/// <returns>ָ��Ԫ��ָ���ָ��</returns>
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd);
/// <summary>
/// �ú��������ڻ�ȡָ�� key ������Ԫ��
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">ָ���� key</param>
/// <param name="lastnode">lastnode</param>
/// <param name="maxposscount">�� lastnode Ϊ NULL ʱ *maxposscount ������Ϊ�ù�ϣ���� key Ϊ�� key �������ܵ�Ԫ������</param>
/// <returns>�� lastnode ��Ϊ NULL ʱ������ hashcode ���׸� key Ϊ�� key ��Ԫ�أ����򷵻�lastnode����һ�� key Ϊ�� key ��Ԫ��</returns>
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE* lastnode, size_t* maxposscount);
/// <summary>
/// ���¼����ϣ����Ԫ�صĹ�ϣֵ
/// </summary>
/// <param name="map"></param>
void HashMap_Rehash(HASHMAP* map);
/// <summary>
/// �ڹ�ϣ��������µ�Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="item"></param>
void HashMap_Add(HASHMAP* map, void* item);
/// <summary>
/// ʲô������
/// </summary>
/// <param name="obj"></param>
inline void HashMap_NoFree(void* obj){}
/// <summary>
/// ����Ψһ�Ĳ���
/// </summary>
/// <param name="obj"></param>
/// <returns>�� obj ��ͬ</returns>
inline void* HashMap_GetKeyNone(void* obj) { return obj; }

