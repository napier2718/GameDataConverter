#include "Common.h"

#include <Windows.h>

using namespace std;

void EncodeUtf8ToShiftjis(char *shiftjis, const char *utf8)
{
  static wchar_t wbuf[TEXTSIZE];
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, TEXTSIZE);
  WideCharToMultiByte(CP_ACP, 0, wbuf, -1, (LPSTR)shiftjis, TEXTSIZE, " ", NULL);
}
void WriteString(string &text, FILE *&file)
{
  int length = (int)(text.size());
  fwrite(&length, sizeof(int), 1, file);
  fwrite(text.c_str(), sizeof(char), length, file);
}