//  HighSpeedTrainTicketClient
//	Copyright(C) 2022 ��Խ
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

#include "HashMap.h"
#include "common.h"

void HashMap_PlaceItem(HASHMAP* map, HASHMAPNODE* node, size_t placeto)
{
	HASHMAPNODE* item = &map->item[placeto];
	if (item->used)
	{
		//��ϣ���г�ͻ����ͷ�巨
		HASHMAPNODE* nnode = malloc(sizeof(HASHMAPNODE));
		if (nnode)
		{
			memcpy(nnode, item, sizeof(HASHMAPNODE));
			map->item[placeto] = *node;
			node->next = nnode;
		}
		else
		{
			UnrecoveryableFailed();
		}
	}
	else
	{
		//�޳�ͻ
		map->item[placeto] = *node;
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
		HASHMAPNODE *firstnode = &map->item[i];
		if (firstnode->used)
		{
			while (HashMap_RePlaceItem(map, firstnode, i))
			{
				HASHMAPNODE *next = map->item[i].next;
				*firstnode = *next;
				free(next);
			}
			//��ʱ firstnode �Ѳ����ܷ��� replace
			HASHMAPNODE* onode = firstnode;
			HASHMAPNODE* node = firstnode->next;
			while (node)
			{
				if (HashMap_RePlaceItem(map, node, i))
				{
					onode->next = node->next;
					free(node);
					node = onode->next;
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

HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc)
{
	size = max(size, HASHMAP_INIT_SIZE);
	HASHMAP map;
	map.count = 0;
	map.item = malloc(sizeof(HASHMAPNODE) * size);
	if (map.item == NULL)
	{
		UnrecoveryableFailed();
	}
	else
	{
		memset(map.item, 0, sizeof(HASHMAPNODE) * size);
		map.listsize = size;
		map.equalFunc = equal;
		map.getKeyFunc = getkey;
		map.delKeyFunc = delfunc;
	}
}

void HashMap_Free(HASHMAP* map)
{
	free(map->item);
}
bool HashMap_ContainKey(HASHMAP* map, void* key)
{
	return HashMap_GetPointerByKey(map, key, false) != NULL;
}
void* HashMap_GetFromKey(HASHMAP* map, void* key)
{
	return *HashMap_GetPointerByKey(map, key, false);
}
void* HashMap_RemoveByKey(HASHMAP* map, void* key)
{
	int64_t hash = map->hashKeyFunc(key);
	size_t place = hash % map->listsize;
	HASHMAPNODE* tobefind = &map->item[place];
	if (tobefind->used)
	{
		HASHMAPNODE* onode;
		HASHMAPNODE* node = tobefind;
		do
		{
			onode = node;
			if (node->hashvalue == hash && map->equalFunc(map->getKeyFunc(node->value), key))
			{
				//node is the node to be del
				if (node == tobefind)
				{
					if (tobefind->next == NULL)
					{
						tobefind->used = false;
					}
					else
					{
						memcpy(tobefind, tobefind->next, sizeof(HASHMAPNODE));
					}
				}
				else
				{
					onode->next = node->next;
					HASHMAPNODE *tobedel = node;
					node = node->next;
					map->delKeyFunc(tobedel->value);
					free(tobedel);
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
void HashMap_SetNode(HASHMAPNODE* node, int64_t hash)
{
	node->hashvalue = hash;
	node->used = true;
}
void HashMap_CheckAndResize(HASHMAP* map)
{
	if (map->count + 1 > map->listsize)
	{
		//����
		size_t olistsize = map->listsize;
		map->listsize *= 2;
		HASHMAPNODE* nblock = realloc(map->item, sizeof(HASHMAPNODE) * map->listsize);
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
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd)
{
	if (allowadd)
	{
		HashMap_CheckAndResize(map);
	}
	int64_t hash = map->hashKeyFunc(key);
	size_t place = hash % map->listsize;
	HASHMAPNODE* tobefind = &map->item[place];
	if (tobefind->used)
	{
		HASHMAPNODE* onode;
		HASHMAPNODE* node = tobefind;
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
				HashMap_SetNode(tobefind, hash);
				return &tobefind->value;
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
		HashMap_SetNode(tobefind, hash);
		return &tobefind->value;
	}
}


