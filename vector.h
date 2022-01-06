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
#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4

#define VECTOR_INIT(vec) vector vec; vector_init(&(vec))
#define VECTOR_ADD(vec, item) vector_add(&vec, (void *) (item))
#define VECTOR_SET(vec, id, item) vector_set(&(vec), (id), (void *) (item))
#define VECTOR_GET(vec, type, id) (type) vector_get(&(vec), (id))
#define VECTOR_DELETE(vec, id) vector_delete(&(vec), (id))
#define VECTOR_TOTAL(vec) vector_total(&(vec))
#define VECTOR_FREE(vec) vector_free(&(vec))
#define VECTOR_CLEAR(vec) vector_clear(&(vec))
#define VECTOR_MOVE(left,right) vector_move(&(left),&(right))

typedef struct vector {
    void* items;
    int capacity;
    int total;
} vector;

void vector_init(vector*);
int vector_total(vector*);
static void vector_resize(vector*, int);
void vector_add(vector*, void*);
void vector_set(vector*, int, void*);
void* vector_get(vector*, int);
void vector_delete(vector*, int);
void vector_free(vector*);
void vector_clear(vector*);
void vector_move(vector* left, vector* right);
void vector_initwithcap(vector* v, size_t capacity);
vector vector_clone(vector* right);
void vector_qsort(vector* vec, _CoreCrtSecureSearchSortCompareFunction func, void* context);
#endif

#define DEFINE_VECTOR(type) \
void vector_init_##type(vector* v);\
inline int vector_total_##type(vector* v){return vector_total(v);}\
void vector_initwithcap_##type(vector* v, size_t capacity);\
static void vector_resize_##type(vector* v, int capacity);\
void vector_add_##type(vector* v, type item);\
void vector_set_##type(vector* v, int index, type item);\
type vector_get_##type(vector* v, int index);\
void vector_delete_##type(vector* v, int index);\
vector vector_clone_##type(vector* right);\
inline void vector_free_##type(vector* v){ vector_free(v);}\
inline void vector_clear_##type(vector* v){ vector_clear(v);}\
void vector_qsort_##type(vector* vec, _CoreCrtSecureSearchSortCompareFunction func, void* context);

DEFINE_VECTOR(wchar_t)
DEFINE_VECTOR(int)