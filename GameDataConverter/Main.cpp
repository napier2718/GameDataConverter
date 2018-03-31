#include <cstdio>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <Windows.h>
// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#define TEXTSIZE 128

using namespace std;
using namespace rapidjson;


void EncodeUtf8ToShiftjis(char *shiftjis, const char *utf8)
{
  static wchar_t wbuf[TEXTSIZE];
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, TEXTSIZE);
  WideCharToMultiByte(CP_ACP, 0, wbuf, -1, (LPSTR)shiftjis, TEXTSIZE, " ", NULL);
}

enum ObjectType
{
  image,
  text,
  link,
};
enum LinkType
{
  none,
  start,
  end,
};
struct Object
{
  ObjectType type;
  int posX, posY;
  int sizeX, sizeY;
  int id;
  LinkType link;
  string text;
};
namespace std
{
  template <>
  class hash<pair<string, int>> {
  public:
    size_t operator()(const pair<string, int>& x) const {
      return hash<string>()(x.first) ^ hash<int>()(x.second);
    }
  };
}

int main()
{
  vector<Object> bgList, objectList, linkList;
  unordered_map<string, int> imageMap;
  vector<string> imageMapList;
  unordered_map<pair<string, int>, int> fontMap;
  vector<pair<string, int>> fontMapList;
  FILE *jsonFile;
  fopen_s(&jsonFile, "data\\title.json", "r");
  char cBuffer[65536];
  char cTemp[TEXTSIZE];
  FileReadStream jsonFS(jsonFile, cBuffer, sizeof(cBuffer));
  Document jsonData;
  jsonData.ParseStream(jsonFS);
  const Value &layers = jsonData["layers"];
  const int layerNum = layers.Size();
  for (int i = 0; i < layerNum; i++) {
    const Value &objects = layers[i]["objects"];
    const int objectNum = objects.Size();
    for (int j = 0; j < objectNum; j++) {
      Object object;
      object.id = -1;
      object.link = none;
      object.text.clear();
      if (strncmp(objects[j]["type"].GetString(), "image", 5) == 0) object.type = image;
      else if (strncmp(objects[j]["type"].GetString(), "text", 4) == 0) object.type = text;
      else if (strncmp(objects[j]["type"].GetString(), "link", 4) == 0) {
        object.type = link;
        if (strncmp(objects[j]["properties"]["link"].GetString(), "start", 5) == 0) object.link = start;
        else if (strncmp(objects[j]["properties"]["link"].GetString(), "end", 3) == 0) object.link = LinkType::end;
      }
      object.posX  = objects[j]["x"].GetInt();
      object.posY  = objects[j]["y"].GetInt();
      object.sizeX = objects[j]["width"].GetInt();
      object.sizeY = objects[j]["height"].GetInt();
      switch (object.type) {
      case image:
        {
          string image(objects[j]["properties"]["filePath"].GetString());
          if (imageMap.find(image) == imageMap.end()) {
            object.id = (int)(imageMapList.size());
            imageMap[image] = object.id;
            imageMapList.push_back(image);
          }
          else object.id = imageMap[image];
        }
        break;
      case text:
        {
          EncodeUtf8ToShiftjis(cTemp, objects[j]["text"]["fontfamily"].GetString());
          pair<string, int> font = make_pair(string(cTemp), objects[j]["text"]["pixelsize"].GetInt());
          if (fontMap.find(font) == fontMap.end()) {
            object.id = (int)(fontMapList.size());
            fontMap[font] = object.id;
            fontMapList.push_back(font);
          }
          else object.id = fontMap[font];
        }
        EncodeUtf8ToShiftjis(cTemp, objects[j]["text"]["text"].GetString());
        object.text += cTemp;
        break;
      }
      if (object.type == link) linkList.push_back(object);
      else {
        if (strncmp(layers[i]["name"].GetString(), "bottom", 6) == 0) bgList.push_back(object);
        else objectList.push_back(object);
      }
    }
  }
  fclose(jsonFile);

  FILE *dataFile;
  fopen_s(&dataFile, "data\\title.data", "wb");
  size_t size = imageMapList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) {
    int length = (int)(imageMapList[i].size());
    fwrite(&length, sizeof(int), 1, dataFile);
    fwrite(imageMapList[i].c_str(), sizeof(char), length, dataFile);
  }
  size = fontMapList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) {
    int length = (int)(fontMapList[i].first.size());
    fwrite(&length, sizeof(int), 1, dataFile);
    fwrite(fontMapList[i].first.c_str(), sizeof(char), length, dataFile);
    fwrite(&fontMapList[i].second, sizeof(int), 1, dataFile);
  }
  size = objectList.size() + bgList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  size = bgList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) {
    fwrite(&bgList[i].type, sizeof(ObjectType), 1, dataFile);
    fwrite(&bgList[i].posX, sizeof(int), 4, dataFile);
    fwrite(&bgList[i].id, sizeof(int), 1, dataFile);
    if (bgList[i].type == text) {
      int length = (int)(bgList[i].text.size());
      fwrite(&length, sizeof(int), 1, dataFile);
      fwrite(bgList[i].text.c_str(), sizeof(char), length, dataFile);
    }
  }
  size = objectList.size();
  for (size_t i = 0; i < size; i++) {
    fwrite(&objectList[i].type, sizeof(ObjectType), 1, dataFile);
    fwrite(&objectList[i].posX, sizeof(int), 4, dataFile);
    fwrite(&objectList[i].id, sizeof(int), 1, dataFile);
    if (objectList[i].type == text) {
      int length = (int)(objectList[i].text.size());
      fwrite(&length, sizeof(int), 1, dataFile);
      fwrite(objectList[i].text.c_str(), sizeof(char), length, dataFile);
    }
  }
  size = linkList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&linkList[i].posX, sizeof(int), 4, dataFile);
    fwrite(&linkList[i].link, sizeof(LinkType), 1, dataFile);
  }
  fclose(dataFile);
  return 0;
}