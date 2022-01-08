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

#include "HashMap.h"
#include "common.h"

void HashMap_PlaceItem(HASHMAP* map, HASHMAPNODE* node, size_t placeto)
{
    HASHMAPNODEBASIC* item = &map->item[placeto];
    if (item->used)
    {
        //��ϣ���г�ͻ����ͷ�巨
        HASHMAPNODE* nnode = malloc(sizeof(HASHMAPNODE));
        if (nnode)
        {
            memcpy(nnode, &item->node, sizeof(HASHMAPNODE));
            item->node = *node;
            node->next = nnode;

            item->used++;
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
    else
    {
        //�޳�ͻ
        item->node = *node;
        item->used = 1;
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
                    if (firstnode->used != 1)//��ʱfirstnode->usedӦ��Ϊ1
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


                firstnode->used--;
            }
            //��ʱ firstnode �Ѳ����ܷ��� replace
            HASHMAPNODE* onode = &firstnode->node;
            HASHMAPNODE* node = firstnode->node.next;
            while (node)
            {
                if (HashMap_RePlaceItem(map, node, i))
                {
                    onode->next = node->next;
                    free(node);
                    node = onode->next;

                    firstnode->used--;
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
/// ����һ���µ� hashmap
/// </summary>
/// <param name="size">��ʼ��С</param>
/// <param name="hash">��Ԫ�ؼ���hash�ĺ���ָ��</param>
/// <param name="parhash">�Դ���Ĳ�������hash�ĺ���ָ��</param>
/// <param name="equal">�ж����� key �Ƿ���ȵĺ���ָ��</param>
/// <param name="getkey">ͨ��Ԫ�ػ�ȡִ��key��ָ��ĺ���ָ��</param>
/// <param name="delfunc">����Ԫ�صĺ���ָ��</param>
/// <returns>������ hashmap</returns>
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
HASHMAP HashMap_SetCoefficient(HASHMAP* map, int coefficient)
{
    map->coefficient = coefficient;
}
/// <summary>
/// ����һ�� hashmap
/// </summary>
/// <param name="map">Ҫ������ hashmap</param>
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
/// ���� hashmap ���Ƿ������ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
bool HashMap_ContainKey(HASHMAP* map, void* key)
{
    return HashMap_GetPointerByKey(map, key, false) != NULL;
}

/// <summary>
/// ��ȡ hashmap ���Ƿ������ָ�� key ��Ԫ��
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
/// ɾ��һ��Ԫ��
/// </summary>
/// <param name="map">��ϣ��</param>
/// <param name="key">Ҫɾ��Ԫ�ص�key</param>
/// <param name="item">Ҫɾ����Ԫ��</param>
/// <param name="checkitem">�Ƿ�ʹ��item����</param>
/// <returns>��ɾ����Ԫ��</returns>
void* HashMap_Remove_Inner(HASHMAP* map, void* key, void* item,bool checkitem)
{
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
            if (node->hashvalue == hash && map->equalFunc(map->getKeyFunc(node->value), key) && (checkitem==false||item == node->value))
            {
                //node is the node to be del
                if (node == &firstnode->node)
                {
                    map->delKeyFunc(firstnode->node.value);

                    if (firstnode->node.next == NULL)
                    {
                        firstnode->used = 0;
                        goto endfordel;
                    }
                    else
                    {
                        HASHMAPNODE *secondnode = firstnode->node.next;
                        memcpy(firstnode, secondnode, sizeof(HASHMAPNODE));
                        free(secondnode);
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
        endfordel:
        return NULL;
    }
    else
    {
        return NULL;
    }
}
/// <summary>
/// �� hashmap ��ɾ��ָ����Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="item">Ҫɾ����Ԫ��</param>
/// <returns>��ɾ��Ԫ�ص�ָ��</returns>
void* HashMap_RemoveItem(HASHMAP* map, void* item)
{
    return HashMap_Remove_Inner(map, map->getKeyFunc(item), item, true);
}
/// <summary>
/// �� hashmap ��ɾ��ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <param name="item"></param>
/// <returns></returns>
void* HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item)
{
    return HashMap_Remove_Inner(map, key, item, true);
}
/// <summary>
/// �� hashmap ��ɾ������ָ�� key ��Ԫ��
/// </summary>
/// <param name="map"></param>
/// <param name="key"></param>
/// <returns></returns>
void* HashMap_RemoveByKey(HASHMAP* map, void* key)
{
    return HashMap_Remove_Inner(map, key, NULL, false);
}
void HashMap_SetNode(HASHMAPNODE* node, uint64_t hash)
{
    node->hashvalue = hash;
}
void HashMap_CheckAndResize(HASHMAP* map)
{
    if (map->count + 1 > map->listsize * map->coefficient)
    {
        //����
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
/// ��ȡָ�� key ��Ԫ��ָ��
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">ָ���� key</param>
/// <param name="allowadd">�Ƿ��������Ҳ���ָ�� key ��ָ��ʱԤ���µ�Ԫ��λ����ʹ�÷���ֵ��Ϊ��</param>
/// <returns>ָ��Ԫ��ָ���ָ��</returns>
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
            //û���ҵ�
            HASHMAPNODE* nnode = malloc(sizeof(HASHMAPNODE));
            if (nnode)
            {
                //��β�巨
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
/// �ú��������ڻ�ȡָ�� key ������Ԫ��
/// </summary>
/// <param name="map">hashmap</param>
/// <param name="key">ָ���� key</param>
/// <param name="lastnode">lastnode</param>
/// <param name="maxposscount">�� lastnode Ϊ NULL ʱ *maxposscount ������Ϊ�ù�ϣ���� key Ϊ�� key �������ܵ�Ԫ������</param>
/// <returns>�� lastnode ��Ϊ NULL ʱ������ hashcode ���׸� key Ϊ�� key ��Ԫ�أ����򷵻�lastnode����һ�� key Ϊ�� key ��Ԫ��</returns>
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
/// ���¼���ý��Ĺ�ϣֵ
/// </summary>
/// <param name="map">��ϣ��</param>
/// <param name="node">Ҫ�����ϣֵ�Ľ��</param>
void HashMap_RehashNode(HASHMAP* map, HASHMAPNODE* node)
{
    node->hashvalue = map->hashKeyFunc(map->getKeyFunc(node->value));
}
/// <summary>
/// ���¼����ϣ����Ԫ�صĹ�ϣֵ
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
/// �ڹ�ϣ��������µ�Ԫ��
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
        firstnode->used = 1;
    }
    map->count++;
}