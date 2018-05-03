#pragma once
#include <String>

#define TEXTSIZE 128

void EncodeUtf8ToShiftjis(char *shiftjis, const char *utf8);
void WriteString(std::string &text, FILE *&file);