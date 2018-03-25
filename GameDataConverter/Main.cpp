#include <cstdio>
#include <cstring>
#include <vector>
#include <Windows.h>
// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#define TEXTSIZE 128

using namespace std;
using namespace rapidjson;

enum ObjectType
{
  image,
  text,
};
struct Object
{
  ObjectType type;
  int posX, posY;
  int sizeX, sizeY;
  char text[TEXTSIZE];
};

void EncodeUtf8ToShiftjis(char *shiftjis, const char *utf8)
{
  static wchar_t wbuf[TEXTSIZE];
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, TEXTSIZE);
  WideCharToMultiByte(CP_ACP, 0, wbuf, -1, (LPSTR)shiftjis, TEXTSIZE, " ", NULL);
}

int main()
{
  //FindFirstFile();
  vector<Object> objectList;
  FILE *jsonFile;
  fopen_s(&jsonFile, "data\\title.json", "r");
  char cBuffer[65536];
  FileReadStream jsonFS(jsonFile, cBuffer, sizeof(cBuffer));
  Document jsonData;
  jsonData.ParseStream(jsonFS);
  const Value &layers = jsonData["layers"];
  const int layerNum = layers.Size();
  for (int i = 0; i < layerNum; i++) {
    if (strncmp(layers[i]["name"].GetString(), "image", 5) == 0) {
      const Value &objects = layers[i]["objects"];
      const int objectNum = objects.Size();
      for (int j = 0; j < objectNum; j++) {
        Object object;
        if (strncmp(objects[j]["type"].GetString(), "image", 5) == 0) object.type = image;
        else if (strncmp(objects[j]["type"].GetString(), "text", 4) == 0) object.type = text;
        object.posX  = objects[j]["x"].GetInt();
        object.posY  = objects[j]["y"].GetInt();
        object.sizeX = objects[j]["width"].GetInt();
        object.sizeY = objects[j]["height"].GetInt();
        switch (object.type) {
        case image:
          strcpy_s(object.text, TEXTSIZE, objects[j]["properties"]["filePath"].GetString());
          break;
        case text:
          EncodeUtf8ToShiftjis(object.text, objects[j]["text"]["text"].GetString());
          EncodeUtf8ToShiftjis(object.text + 64, objects[j]["text"]["fontfamily"].GetString());
          break;
        }
        objectList.push_back(object);
      }
    }
    //else if (strncmp(layers[i]["name"].GetString(), "event", 5) == 0) {}
  }
  fclose(jsonFile);

  FILE *dataFile;
  fopen_s(&dataFile, "data\\title.data", "wb");
  const int size = objectList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&objectList[i].type, sizeof(ObjectType), 1, dataFile);
    fwrite(&objectList[i].posX, sizeof(int), 4, dataFile);
    fwrite(&objectList[i].text, sizeof(char), TEXTSIZE, dataFile);
  }
  fclose(dataFile);
  return 0;
}