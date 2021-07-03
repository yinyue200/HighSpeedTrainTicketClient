#pragma once
#include <Windows.h>
#include <stdint.h>
#include "common.h"
enum ProductState
{
	PRODUCTSTATE_UNKNOWN=0,
};
typedef struct ProductRecord
{
	wchar_t* Name;//0
	int ID;//1
	wchar_t* Type;//3
	enum ProductState State;//4
	int64_t Date;//5
	wchar_t* ProvideBy;//6
	wchar_t* RecievedBy;//6
	wchar_t* ResentBy;//退货商名称//7
	int Count;//8
	int64_t Cost;//进价,以分为单位//9
	int64_t Price;//10
	int64_t ResentPrice;//11
	wchar_t* Signer;//12
} PRODUCTRECORD;
typedef  PRODUCTRECORD* PRODUCTRECORD_PTR;
PRODUCTRECORD_PTR CreateProductRecord();