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

#include "HashMap.h"
#include "common.h"

void HashMap_PlaceItem(HASHMAP* map, HASHMAPNODE* node, size_t placeto)
{
    HASHMAPNODEBASIC* item = &map->item[placeto];
    if (item->used)
    {
        //哈希表有冲突，做头插法
        HASHMAPNODE* nnode = malloc(sizeof(HASHMAPNODE));
        if (nnode)
        {
            memcpy(nnode, &item->node, sizeof(HASHMAPNODE));
            item->node = *node;
            item->node.next = nnode;

            item->used++;
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
    else
    {
        //无冲突
        item->node = *node;
        item->used = 1;
        item->node.next = NULL;
    }
}

bool HashMap_RePlaceItem(HASHMAP* map, HASHMAPNODE *node, size_t nowplace)
{
    uint64_t hash = node->hashvalue;
    size_t newplace = hash % map->listsize;
    if (newplace != nowplace)
    {
        HashMap_PlaceItem(map, node, newplace);
        return true;
    }
    else
    {
        return false;
    }
}

void HashMap_RePlace(HASHMAP* map, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        HASHMAPNODEBASIC* firstnode = &map->item[i];
        if (firstnode->used)
        {
            while (HashMap_RePlaceItem(map, &firstnode->node, i))
            {
                HASHMAPNODE *next = map->item[i].node.next;
                if (next == NULL)
                {
#if _DEBUG
                    if (firstnode->used != 1)//此时firstnode->used应该为1
                    {
                        UnrecoveryableFailed();
                    }
#endif
                    firstnode->used = 0;
                    break;
                }
                else
                {
                    firstnode->node = *next;
                    free(next);
                }

#if _DEBUG
                free(NULL);
#endif
                
                firstnode->used--;
            }
            //此时 firstnode 已不可能发生 replace
            HASHMAPNODE* onode = &firstnode->node;
            HASHMAPNODE* node = firstnode->node.next;
            while (node)
            {
                if (HashMap_RePlaceItem(map, node, i))
                {
                    onode->next = node->next;
                    free(node);
                    node = onode->next;

                    firstnode->used--;//不会减到0

#if _DEBUG
                    if (firstnode->used == 0)
                    {
                        ;
                    }
#endif
                }
                else
                {
                    onode = node;
                    node = node->next;
                }
            }
        }
    }
}

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
HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_HashKeyFunc parhash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc)
{
    size = max(size, HASHMAP_INIT_SIZE);
    HASHMAP map;
    map.count = 0;
    map.item = malloc(sizeof(HASHMAPNODEBASIC) * size);
    if (map.item == NULL)
    {
        UnrecoveryableFailed();
    }
    else
    {
        memset(map.item, 0, sizeof(HASHMAPNODEBASIC) * size);
        map.listsize = size;
        map.equalFunc = equal;
        map.getKeyFunc = getkey;
        map.delKeyFunc = delfunc;
        map.hashKeyFunc = hash;
        map.parHashKeyFunc = parhash;
        map.coefficient = 1;
    }
    return map;
}
void HashMap_SetCoefficient(HASHMAP* map, int coefficient)
{
    map->coefficient = coefficient;
}
/// <summary>
/// 析构一个 hashmap
/// </summary>
/// <param name="map">要析构的 hashmap</param>
void HashMap_Free(HASHMAP* map)
{

    for (int i = 0; i < map->listsize; i++)
    {
        HASHMAPNODEBASIC* firstnode = &map->item[i];
        if (firstnode->used)
        {
            map->delKeyFunc(firstnode->node.value);
            HASHMAPNODE* node = firstnode->node.next;
            while (node)
            {
                HASHMAPNODE* nnode = node->next;

                map->delKeyFunc(node->value);
                free(node);

                node = nnode;
            }
        }
    }

    free(map->item);
}

/// <summary>
/// 返回 hashmap 中是否包含有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
bool HashMap_ContainKey(HASHMAP* map, void* key)
{
    return HashMap_GetPointerByKey(map, key, false) != NULL;
}

/// <summary>
/// 获取 hashmap 中是否包含有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void* HashMap_GetByKey(HASHMAP* map, void* key)
{
    void** ret = HashMap_GetPointerByKey(map, key, false);
    if (ret == NULL)
        return NULL;
    return *ret;
}
/// <summary>
/// 删除一个元素
/// </summary>
/// <param name="map">哈希表</param>
/// <param name="key">要删除元素的key</param>
/// <param name="item">要删除的元素</param>
/// <param name="checkitem">是否使用item参数</param>
void HashMap_Remove_Inner(HASHMAP* map, void* key, void* item,bool checkitem,bool checkkey)
{
    uint64_t hash = checkkey ? map->parHashKeyFunc(key) : map->hashKeyFunc(item);
    size_t place = hash % map->listsize;
    HASHMAPNODEBASIC* firstnode = &map->item[place];
    if (firstnode->used)
    {
        HASHMAPNODE* onode;
        HASHMAPNODE* node = &firstnode->node;
        do
        {
            onode = node;
            if (node->hashvalue == hash && (checkkey==false || map->equalFunc(map->getKeyFunc(node->value), key)) && (checkitem==false||item == node->value))
            {
                //node is the node to be del
                if (node == &firstnode->node)
                {
                    map->delKeyFunc(firstnode->node.value);

                    HASHMAPNODE* secondnode = firstnode->node.next;

                    if (secondnode == NULL)
                    {
                        firstnode->used = 0;
                        goto endfordel;
                    }
                    else
                    {
                        memcpy(&firstnode->node, secondnode, sizeof(HASHMAPNODE));
                        free(secondnode);

                        firstnode->used--;
                    }

                    map->count--;

                    goto skipround;
                }
                else
                {
                    onode->next = node->next;
                    HASHMAPNODE* tobedel = node;
                    node = node->next;
                    map->delKeyFunc(tobedel->value);
                    free(tobedel);

                    map->count--;

                    firstnode->used--;

                    goto skipround;
                }
            }
            node = node->next;
        skipround:;
        } while (node);
    endfordel:;
        //return false;
    }
    else
    {
        //return false;
    }
}
/// <summary>
/// 从 hashmap 中删除指定的元素
/// </summary>
/// <param name="map"></param>
/// <param name="item">要删除的元素</param>
void HashMap_RemoveItem(HASHMAP* map, void* item)
{
    HashMap_Remove_Inner(map, NULL, item, true, false);
}
/// <summary>
/// 从 hashmap 中删除指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <param name="item"></param>
void HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item)
{
    HashMap_Remove_Inner(map, key, item, true, true);
}
/// <summary>
/// 从 hashmap 中删除所有指定 key 的元素
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void HashMap_RemoveByKey(HASHMAP* map, void* key)
{
    HashMap_Remove_Inner(map, key, NULL, false, true);
}
void HashMap_SetNode(HASHMAPNODE* node, uint64_t hash)
{
    node->hashvalue = hash;
}
void HashMap_CheckAndResize(HASHMAP* map)
{
    if (map->count + 1 > map->listsize * map->coefficient)
    {
        //扩容
        size_t olistsize = map->listsize;
        map->listsize *= 2;
        HASHMAPNODEBASIC* nblock = realloc(map->item, sizeof(HASHMAPNODEBASIC) * map->listsize);
        if (nblock)
        {
            void* newposstart = ((char*)nblock) + olistsize * sizeof(HASHMAPNODEBASIC);
            memset(newposstart, 0, olistsize * sizeof(HASHMAPNODEBASIC));
            map->item = nblock;
            HashMap_RePlace(map, olistsize);
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
}

/// <summary>
/// 获取指定 key 的元素指针
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">指定的 key</param>
/// <param name="allowadd">是否允许在找不到指定 key 的指针时预留新的元素位置以使得返回值不为空</param>
/// <returns>指向元素指针的指针</returns>
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd)
{
    if (allowadd)
    {
        HashMap_CheckAndResize(map);
    }
    uint64_t hash = map->parHashKeyFunc(key);
    size_t place = hash % map->listsize;
    HASHMAPNODEBASIC* firstnode = &map->item[place];
    if (firstnode->used)
    {
        HASHMAPNODE* onode;
        HASHMAPNODE* node = &firstnode->node;
        do
        {
            onode = node;
            if (node->hashvalue == hash && map->equalFunc(map->getKeyFunc(node->value), key))
            {
                return &node->value;
            }
            node = node->next;
        } while (node);
        if (allowadd)
        {
            //没有找到
            HASHMAPNODE* nnode = malloc(sizeof(HASHMAPNODE));
            if (nnode)
            {
                //做尾插法
                onode->next = nnode;
                nnode->next = NULL;
                HashMap_SetNode(nnode, hash);

                firstnode->used++;

                map->count++;

                nnode->value = NULL;
                nnode->next = NULL;

                return &nnode->value;
            }
            else
            {
                UnrecoveryableFailed();
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        if (allowadd)
        {
            map->count++;

            firstnode->used = 1;
            HashMap_SetNode(&firstnode->node, hash);
            return &firstnode->node.value;
        }
        else
        {
            return NULL;
        }
    }
}

/// <summary>
/// 该函数可用于获取指定 key 的所有元素
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">指定的 key</param>
/// <param name="lastnode">lastnode</param>
/// <param name="maxposscount">当 lastnode 为 NULL 时 *maxposscount 将设置为该哈希表中 key 为该 key 的最多可能的元素数量</param>
/// <returns>当 lastnode 不为 NULL 时，返回 hashcode 中首个 key 为该 key 的元素，否则返回lastnode的下一个 key 为该 key 的元素</returns>
HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE *lastnode, size_t *maxposscount)
{
    if (lastnode == NULL)
    {
        uint64_t hash = map->parHashKeyFunc(key);
        size_t place = hash % map->listsize;
        HASHMAPNODEBASIC* tobefind = &map->item[place];
        *maxposscount = tobefind->used;
        if (tobefind->used)
        {
            HASHMAPNODE* onode;
            HASHMAPNODE* node = &tobefind->node;
            do
            {
                onode = node;
                if (node->hashvalue == hash && map->equalFunc(map->getKeyFunc(node->value), key))
                {
                    return node;
                }
                node = node->next;
            } while (node);
            *maxposscount = 0;
            return NULL;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        int64_t hash = lastnode->hashvalue;
        HASHMAPNODE* node = lastnode->next;
        while (node)
        {
            if (node->hashvalue == hash && map->equalFunc(map->getKeyFunc(node->value), key))
            {
                return node;
            }
            node = node->next;
        }
        return NULL;
    }
}
/// <summary>
/// 重新计算该结点的哈希值
/// </summary>
/// <param name="map">哈希表</param>
/// <param name="node">要计算哈希值的结点</param>
void HashMap_RehashNode(HASHMAP* map, HASHMAPNODE* node)
{
    node->hashvalue = map->hashKeyFunc(map->getKeyFunc(node->value));
}
/// <summary>
/// 重新计算哈希表中元素的哈希值
/// </summary>
/// <param name="map"></param>
void HashMap_Rehash(HASHMAP* map)
{
    for (size_t i = 0; i < map->listsize; i++)
    {
        HASHMAPNODEBASIC* firstnode = &map->item[i];
        if (firstnode->used)
        {
            HashMap_RehashNode(map, &firstnode->node);
            HASHMAPNODE* node = firstnode->node.next;
            while (node)
            {
                HashMap_RehashNode(map, node);
                node = node->next;
            }
        }
    }
    HashMap_RePlace(map, map->listsize);
}
/// <summary>
/// 在哈希表中添加新的元素
/// </summary>
/// <param name="map"></param>
/// <param name="item"></param>
void HashMap_Add(HASHMAP* map, void* item)
{
    HashMap_CheckAndResize(map);
    uint64_t hash = map->hashKeyFunc(map->getKeyFunc(item));
    size_t place = hash % map->listsize;
    HASHMAPNODEBASIC* firstnode = &map->item[place];
    if (firstnode->used)
    {
        HASHMAPNODE* nnnode = malloc(sizeof(HASHMAPNODE));
        if (nnnode)
        {
            HashMap_SetNode(nnnode, hash);
            nnnode->value = item;
            nnnode->next = firstnode->node.next;
            firstnode->node.next = nnnode;

            firstnode->used++;
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
    else
    {
        HashMap_SetNode(&firstnode->node, hash);
        firstnode->node.value = item;
        firstnode->node.next = NULL;//好像不要这一句也可以
        firstnode->used = 1;
    }
    map->count++;
}