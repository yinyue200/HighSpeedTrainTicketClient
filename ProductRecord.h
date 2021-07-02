#pragma once
#include <Windows.h>
#include <stdint.h>
enum ProductState
{
	PRODUCTSTATE_UNKNOWN=0,
};
typedef struct
{
	wchar_t* Name;
	int ID;
	wchar_t* Type;
	ProductState State;
	int64_t Date;
	wchar_t* ProvideBy;
	wchar_t* RecievedBy;
	wchar_t* ResentBy;//�˻�������
	int Count;
	int64_t Cost;//����,�Է�Ϊ��λ
	int64_t Price;
	int64_t ResentPrice;
	wchar_t* Signer;
} PRODUCTRECORD;