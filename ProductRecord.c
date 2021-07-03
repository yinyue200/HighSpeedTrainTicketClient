#include "ProductRecord.h"

PRODUCTRECORD_PTR CreateProductRecord()
{
	PRODUCTRECORD_PTR PT = malloc(sizeof(PRODUCTRECORD));
	if (PT == NULL)
		return PT;
	memset(PT, 0, sizeof(PRODUCTRECORD));
	return PT;
}
vector* LoadToVector(LPSTR path)
{
	//FILE SHOULD BE UTF-8 ENCODED
	vector* vec = malloc(sizeof(vector));
	vector_init(vec);

	HANDLE hFile = CreateFile(path,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return vec;//找不到文件，返回空 vector
	}

	LARGE_INTEGER FILESIZEINFO;
	GetFileSizeEx(hFile, &FILESIZEINFO);

	char* data = malloc(FILESIZEINFO.QuadPart);
	if (data)
	{
		DWORD readsize;

		BOOL readfilesucc = ReadFile(hFile, data, FILESIZEINFO.QuadPart, &readsize, NULL);

		if (readfilesucc)
		{
			int laststart = 0;
			int tindex = 0;
			PRODUCTRECORD_PTR p = CreateProductRecord();
			if (p)
			{
				for (size_t i = 0; i < FILESIZEINFO.QuadPart; i++)
				{
					char one = data[i];
					size_t size = i - laststart;
					if (one == '\t')
					{
						PWCHAR info = malloc(size * 2);
						MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, &data[laststart], size, info, size);
						switch (tindex)
						{
						case 0:
							p->Name = info;
						case 1:
							p->ID = info;
						default:
							break;
						}
						tindex++;
						laststart = i;
					}
					else if (one == '\r' || one == '\n')
					{
						laststart = i;
						if (size > 0)
						{
							tindex = 0;
							VECTOR_ADD(vec, p);
							p = CreateProductRecord();
							if (p != NULL)
							{
								UnrecoveryableFailed();
							}
						}
					}
				}
			}
			else
			{
				UnrecoveryableFailed();
			}		
		}
		else
		{
			UnrecoveryableFailed();
		}
	}
	else
	{
		UnrecoveryableFailed();
	}

}