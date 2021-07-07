//  SimpleStoreErp
//	Copyright(C) 2021 “Û‘Ω
//
//	This program is free software : you can redistribute it and /or modify
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

void vector_init(vector* v)
{
    vector_initwithcap(v, VECTOR_INIT_CAPACITY);
}

void vector_initwithcap(vector* v,size_t capacity)
{
    v->capacity = capacity;
    v->total = 0;
    v->items = malloc(sizeof(void*) * v->capacity);
}

int vector_total(vector* v)
{
    return v->total;
}

static void vector_resize(vector* v, int capacity)
{
#ifdef DEBUG_ON
    printf("vector_resize: %d to %d\n", v->capacity, capacity);
#endif

    void** items = realloc(v->items, sizeof(void*) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void vector_add(vector* v, void* item)
{
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void vector_set(vector* v, int index, void* item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

void* vector_get(vector* v, int index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

void vector_delete(vector* v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

void vector_free(vector* v)
{
    free(v->items);
}

void vector_clear(vector* v)
{
    v->total = 0;
}

void vector_move(vector* left, vector* right)
{
    memcpy(left, right, sizeof(vector));
}

vector vector_clone(vector* right)
{
    vector vec;

    vector_initwithcap(&vec, max(right->capacity, VECTOR_INIT_CAPACITY));
    vec.total = right->total;
    if(right->capacity>0)
        memcpy(vec.items, right->items, sizeof(void*) * right->capacity);
    return vec;
}

void vector_qsort(vector* vec, _CoreCrtSecureSearchSortCompareFunction func,void* context)
{
    qsort_s(vec->items, vector_total(vec), sizeof(void*), func, context);
}