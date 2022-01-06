//  SimpleStoreErp
//  Copyright(C) 2021 殷越
//
//  This program is free software : you can redistribute it and /or modify
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
#include "UserManage.h"
#include "common.h"
#include "ProductRecord.h"
#include "sha256.h"
//写入用户信息
bool yinyue200_UserRecordSaveToFile(LPWSTR path, vector* vec)
{
  HANDLE hFile = CreateFile(path,               // file to open
    GENERIC_READ | GENERIC_WRITE,          // open for reading
    0,       // share for reading
    NULL,                  // default security
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL, // normal file
    NULL);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    int errcod = GetLastError();
    MessageBox(NULL, L"Save Failed!", NULL, 0);
    return false;
  }
  size_t length = VECTOR_TOTAL(*vec);
  for (size_t i = 0; i < length; i++)
  {
    USERDATAINFO_PTR record = VECTOR_GET(*vec, USERDATAINFO_PTR, i);
    FailedIfFalse(WritePWSTR(record->Name, hFile));
    FailedIfFalse(WritePWSTR(L"\t", hFile));
    FailedIfFalse(WritePWSTR(record->PasswordHash, hFile));
    FailedIfFalse(WritePWSTR(L"\t", hFile));
    FailedIfFalse(WritePWSTR(record->Type, hFile));
    FailedIfFalse(WritePWSTR(L"\t", hFile));


    FailedIfFalse(WritePWSTR(L"\n", hFile));
  }
  CloseHandle(hFile);
}
//读取用户信息
vector* UserRecordLoadToVector(LPWSTR path)
{
  //FILE SHOULD BE UTF-8 ENCODED
  vector* vec = malloc(sizeof(vector));
  if (vec)
  {
    vector_init(vec);

    HANDLE hFile = CreateFile(path,               // file to open
      GENERIC_READ,          // open for reading
      FILE_SHARE_READ,       // share for reading
      NULL,                  // default security
      OPEN_EXISTING,         // existing file only
      FILE_ATTRIBUTE_NORMAL, // normal file
      NULL);                 // no attr. template
    if (hFile == INVALID_HANDLE_VALUE)
    {
      CloseHandle(hFile);
      return vec;//找不到文件，返回空 vector
    }

    LARGE_INTEGER FILESIZEINFO;
    GetFileSizeEx(hFile, &FILESIZEINFO);
    if (FILESIZEINFO.QuadPart == 0)
    {
      CloseHandle(hFile);
      return vec;
    }
    char* data = malloc(FILESIZEINFO.QuadPart);
    if (data)
    {
      DWORD readsize;

      BOOL readfilesucc = ReadFile(hFile, data, FILESIZEINFO.QuadPart, &readsize, NULL);

      if (readfilesucc)
      {
        int laststart = 0;
        int tindex = 0;
        BOOL lastisbl = FALSE;
        USERDATAINFO_PTR p = yinyue200_safemalloc(sizeof(USERDATAINFO));
        for (size_t i = 0; i < FILESIZEINFO.QuadPart; i++)
        {
          char one = data[i];
          size_t size = i - laststart;
          if (one == '\t' || one == '\r' || one == '\n')
          {
            if (size > 0)
            {
              PWCHAR info = yinyue200_safemalloc(size * 2 + 2);//假定UTF-8的每个字节最多可以在UTF-16中占两个字节
              int sizechars = MultiByteToWideChar(CP_UTF8, 0, &data[laststart], size, info, size);
              info[sizechars] = 0;
              switch (tindex)
              {
              case 0:
                p->Name = info;
                break;
              case 1:
                p->PasswordHash = info;
                break;
              case 2:
                p->Type = info;
                break;
              default:
                break;
              }
            }
            if(one == '\t')
              tindex++;
            laststart = i + 1;
          }
          if (one == '\r' || one == '\n')
          {
            laststart = i + 1;
            if (!lastisbl)
            {
              tindex = 0;
              VECTOR_ADD(*vec, p);
              p = yinyue200_safemalloc(sizeof(USERDATAINFO));
            }
            lastisbl = true;
          }
          else
          {
            lastisbl = false;
          }
        }
        free(p);
      }
      else
      {
        UnrecoveryableFailed();
      }
      free(data);
    }
    else
    {
      UnrecoveryableFailed();
    }
    CloseHandle(hFile);
  }
  else
  {
    UnrecoveryableFailed();
  }
  return vec;
}
//对宽字符串做 SHA-256 运算
void Hash256LPWSTR(LPWSTR str,wchar_t buf[65])
{
  int len = wcslen(str);
  char out[65];//SHA-256 的字符串表示形式有64个字符
  buf[64] = 0;
  sha256(str, len * sizeof(wchar_t) / sizeof(unsigned char),out);
  for (size_t i = 0; i < 64; i++)
  {
    buf[i] = out[i];
  }
}
