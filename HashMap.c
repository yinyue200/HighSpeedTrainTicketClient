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
    int64_t hash = node->hashvalue;
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
            while (HashMap_RePlaceItem(map, firstnode, i))
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
//����һ���µ� hashmap
//@param ��ʼ��С
//@param hash func
//@param �Դ���Ĳ�������hash�ĺ���ָ��
//@param �ж����� key �Ƿ���ȵĺ���ָ��
//@param ͨ��Ԫ�ػ�ȡִ��key��ָ��ĺ���ָ��
//@param ����Ԫ�صĺ���ָ��
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
    }
    return map;
}
//����һ�� hashmap
//@param Ҫ������ hashmap
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
//���� hashmap ���Ƿ������ָ�� key ��Ԫ��
bool HashMap_ContainKey(HASHMAP* map, void* key)
{
    return HashMap_GetPointerByKey(map, key, false) != NULL;
}
//��ȡ hashmap ���Ƿ������ָ�� key ��Ԫ��
void* HashMap_GetByKey(HASHMAP* map, void* key)
{
    void** ret = HashMap_GetPointerByKey(map, key, false);
    if (ret == NULL)
        return NULL;
    return *ret;
}
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
                    }
                    else
                    {
                        memcpy(firstnode, firstnode->node.next, sizeof(HASHMAPNODE));
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
//�� hashmap ��ɾ��ָ�� key ��Ԫ��
void* HashMap_RemoveItemByKey(HASHMAP* map, void* key, void* item)
{
    return HashMap_Remove_Inner(map, key, item, true);
}
//�� hashmap ��ɾ������ָ�� key ��Ԫ��
void* HashMap_RemoveByKey(HASHMAP* map, void* key)
{
    return HashMap_Remove_Inner(map, key, NULL, false);
}
void HashMap_SetNode(HASHMAPNODE* node, int64_t hash)
{
    node->hashvalue = hash;
}
void HashMap_CheckAndResize(HASHMAP* map)
{
    if (map->count + 1 > map->listsize)
    {
        //����
        size_t olistsize = map->listsize;
        map->listsize *= 2;
        HASHMAPNODEBASIC* nblock = realloc(map->item, sizeof(HASHMAPNODEBASIC) * map->listsize);
        if (nblock)
        {
            map->item = nblock;
            HashMap_RePlace(map, olistsize);
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
}
//��ȡָ�� key ��Ԫ��ָ��
//@param hashmap
//@param ָ���� key
//@param �Ƿ��������Ҳ���ָ�� key ��ָ��ʱԤ���µ�Ԫ��λ����ʹ�÷���ֵ��Ϊ��
//@return ָ��Ԫ��ָ���ָ��
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
                HashMap_SetNode(&firstnode->node, hash);

                firstnode->used++;

                map->count++;

                nnode->value = NULL;

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

            HashMap_SetNode(firstnode, hash);
            return &firstnode->node.value;
        }
        else
        {
            return NULL;
        }
    }
}
//�ú��������ڻ�ȡָ�� key ������Ԫ��
//@param hashmap
//@param ָ���� key
//@param lastnode
//@param �� lastnode Ϊ NULL ʱ *maxposscount ������Ϊ�ù�ϣ���� key Ϊ�� key �������ܵ�Ԫ������
//@return �� lastnode ��Ϊ NULL ʱ������ hashcode ���׸� key Ϊ�� key ��Ԫ�أ����򷵻�lastnode����һ�� key Ϊ�� key ��Ԫ��
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
                    return &node;
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
void HashMap_RehashNode(HASHMAP* map, HASHMAPNODE* node)
{
    node->hashvalue = map->hashKeyFunc(map->getKeyFunc(node->value));
}
//���¼����ϣ����Ԫ�صĹ�ϣֵ
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
//�ڹ�ϣ��������µ�Ԫ��
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
        HashMap_SetNode(firstnode, hash);
        firstnode->node.value = item;
        firstnode->used = 1;
    }
    map->count++;
}