//  HighSpeedTrainTicketClient
//	Copyright(C) 2022 殷越
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
	HASHMAPNODEBASIC* item = &map->item[placeto];
	if (item->used)
	{
		//哈希表有冲突，做头插法
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
		//无冲突
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

HASHMAP HashMap_Create(size_t size, HashMap_HashKeyFunc hash, HashMap_IsKeyEqualFunc equal, HashMap_GetKeyFunc getkey, HashMap_FreeItemFunc delfunc)
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
	}
	return map;
}

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
bool HashMap_ContainKey(HASHMAP* map, void* key)
{
	return HashMap_GetPointerByKey(map, key, false) != NULL;
}
void* HashMap_GetByKey(HASHMAP* map, void* key)
{
	return *HashMap_GetPointerByKey(map, key, false);
}
void* HashMap_RemoveByKey(HASHMAP* map, void* key)
{
	uint64_t hash = map->hashKeyFunc(key);
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
				//node is the node to be del
				if (node == &firstnode->node)
				{
					if (firstnode->node.next == NULL)
					{
						firstnode->used = 0;
					}
					else
					{
						memcpy(firstnode, firstnode->node.next, sizeof(HASHMAPNODE));
					}
				}
				else
				{
					onode->next = node->next;
					HASHMAPNODE *tobedel = node;
					node = node->next;
					map->delKeyFunc(tobedel->value);
					free(tobedel);

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
void HashMap_SetNode(HASHMAPNODE* node, int64_t hash)
{
	node->hashvalue = hash;
}
void HashMap_CheckAndResize(HASHMAP* map)
{
	if (map->count + 1 > map->listsize)
	{
		//扩容
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
void** HashMap_GetPointerByKey(HASHMAP* map, void* key, bool allowadd)
{
	if (allowadd)
	{
		HashMap_CheckAndResize(map);
	}
	uint64_t hash = map->hashKeyFunc(key);
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
				HashMap_SetNode(&firstnode->node, hash);

				firstnode->used++;

				return &firstnode->node.value;
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
		HashMap_SetNode(firstnode, hash);
		return &firstnode->node.value;
	}
}

HASHMAPNODE* HashMap_GetPointersByKey(HASHMAP* map, void* key, HASHMAPNODE *lastnode, size_t *maxposscount)
{
	if (lastnode == NULL)
	{
		uint64_t hash = map->hashKeyFunc(key);
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
			HashMap_SetNode(&tobefind->node, hash);
			return &tobefind->node;
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
}