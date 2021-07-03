#include "ProductRecord.h"

PRODUCTRECORE_PTR CreateProductRecord()
{
	PRODUCTRECORE_PTR PT = malloc(sizeof(PRODUCTRECORD));
	if (PT == NULL)
		return PT;
	memset(PT, 0, sizeof(PRODUCTRECORD));
	return PT;
}
vector* LoadToVector(LPSTR path)
{
	//FILE SHOULD BE UTF-8 ENCODED
	vector* p = malloc(sizeof(vector));
	vector_init(p);

	HANDLE hFile = CreateFile(path,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return p;
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
			PRODUCTRECORD *p = malloc(sizeof(PRODUCTRECORD));
			if (p)
			{
				for (size_t i = 0; i < FILESIZEINFO.QuadPart; i++)
				{
					char one = data[i];
					if (one == '\t')
					{
						size_t size = i - laststart + 1;
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