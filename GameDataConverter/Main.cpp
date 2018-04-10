#include "Vector.h"
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
  Vector<int> pos, size;
  int id;
  LinkType link;
  string text;
};
struct Bullet
{
  Vector<double> pos, v;
  double angle;
};
struct Player
{
  Vector<double> pos;
  double speed;
  int shotWait;
  vector<Bullet> shotBullet;
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
void EncodeUtf8ToShiftjis(char *shiftjis, const char *utf8)
{
  static wchar_t wbuf[TEXTSIZE];
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, TEXTSIZE);
  WideCharToMultiByte(CP_ACP, 0, wbuf, -1, (LPSTR)shiftjis, TEXTSIZE, " ", NULL);
}
void ReadTitleJSONFile(const char *jsonFileName, vector<string> &imageList, vector<pair<string, int>> &fontList, vector<Object> &bgList, vector<Object> &objectList, vector<Object> &linkList)
{
  using namespace rapidjson;
  FILE *jsonFile;
  char cJSONBuffer[65536], cTexiBuffer[TEXTSIZE];
  Document jsonData;
  unordered_map<string, int> imageMap;
  unordered_map<pair<string, int>, int> fontMap;
  fopen_s(&jsonFile, jsonFileName, "r");
  FileReadStream jsonFS(jsonFile, cJSONBuffer, sizeof(cJSONBuffer));
  jsonData.ParseStream(jsonFS);
  const Value &layers = jsonData["layers"];
  for (unsigned int i = 0; i < layers.Size(); i++) {
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
      object.pos.x = objects[j]["x"].GetInt();
      object.pos.y = objects[j]["y"].GetInt();
      object.size.x = objects[j]["width"].GetInt();
      object.size.y = objects[j]["height"].GetInt();
      switch (object.type) {
      case image:
      {
        string image(objects[j]["properties"]["filePath"].GetString());
        if (imageMap.find(image) == imageMap.end()) {
          object.id = (int)(imageList.size());
          imageMap[image] = object.id;
          imageList.push_back(image);
        }
        else object.id = imageMap[image];
      }
      break;
      case text:
      {
        EncodeUtf8ToShiftjis(cTexiBuffer, objects[j]["text"]["fontfamily"].GetString());
        pair<string, int> font = make_pair(string(cTexiBuffer), objects[j]["text"]["pixelsize"].GetInt());
        if (fontMap.find(font) == fontMap.end()) {
          object.id = (int)(fontList.size());
          fontMap[font] = object.id;
          fontList.push_back(font);
        }
        else object.id = fontMap[font];
      }
      EncodeUtf8ToShiftjis(cTexiBuffer, objects[j]["text"]["text"].GetString());
      object.text += cTexiBuffer;
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
}
void ReadPlayerJSONFile(const char *jsonFileName, Player &player)
{
  using namespace rapidjson;
  FILE *jsonFile;
  char cJSONBuffer[65536];
  Document jsonData;
  fopen_s(&jsonFile, jsonFileName, "r");
  FileReadStream jsonFS(jsonFile, cJSONBuffer, sizeof(cJSONBuffer));
  jsonData.ParseStream(jsonFS);
  player.pos.x = jsonData["start"]["x"].GetDouble();
  player.pos.y = jsonData["start"]["y"].GetDouble();
  player.speed = jsonData["speed"].GetDouble();
  player.shotWait = jsonData["shot"]["wait"].GetInt();
  const Value &bullets = jsonData["shot"]["bullet"];
  for (unsigned int i = 0; i < bullets.Size(); i++) {
    Bullet bullet;
    bullet.pos.x = bullets[i]["pos"]["x"].GetDouble();
    bullet.pos.y = bullets[i]["pos"]["y"].GetDouble();
    bullet.v.x = bullets[i]["v"]["x"].GetDouble();
    bullet.v.y = bullets[i]["v"]["y"].GetDouble();
    bullet.angle = bullets[i]["angle"].GetDouble();
    player.shotBullet.push_back(bullet);
  }
  fclose(jsonFile);
}
void WriteString(string &text, FILE *&file)
{
  int length = (int)(text.size());
  fwrite(&length, sizeof(int), 1, file);
  fwrite(text.c_str(), sizeof(char), length, file);
}
int main()
{
  vector<string> imageList;
  vector<pair<string, int>> fontList;
  vector<Object> bgList, objectList, linkList;
  ReadTitleJSONFile("data\\title.json", imageList, fontList, bgList, objectList, linkList);
  FILE *dataFile;
  fopen_s(&dataFile, "data\\title.data", "wb");
  size_t size = imageList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) WriteString(imageList[i], dataFile);
  size = fontList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) {
    WriteString(fontList[i].first, dataFile);
    fwrite(&fontList[i].second, sizeof(int), 1, dataFile);
  }
  size = objectList.size() + bgList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  size = bgList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) {
    fwrite(&bgList[i].type, sizeof(ObjectType), 1, dataFile);
    fwrite(&bgList[i].pos,  sizeof(int),        2, dataFile);
    fwrite(&bgList[i].size, sizeof(int),        2, dataFile);
    fwrite(&bgList[i].id,   sizeof(int),        1, dataFile);
    if (bgList[i].type == text) WriteString(bgList[i].text, dataFile);
  }
  size = objectList.size();
  for (size_t i = 0; i < size; i++) {
    fwrite(&objectList[i].type, sizeof(ObjectType), 1, dataFile);
    fwrite(&objectList[i].pos,  sizeof(int),        2, dataFile);
    fwrite(&objectList[i].size, sizeof(int),        2, dataFile);
    fwrite(&objectList[i].id,   sizeof(int),        1, dataFile);
    if (objectList[i].type == text) WriteString(objectList[i].text, dataFile);
  }
  size = linkList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&linkList[i].pos,  sizeof(int),      2, dataFile);
    fwrite(&linkList[i].size, sizeof(int),      2, dataFile);
    fwrite(&linkList[i].link, sizeof(LinkType), 1, dataFile);
  }
  fclose(dataFile);


  Player player;
  ReadPlayerJSONFile("data\\player.json", player);
  fopen_s(&dataFile, "data\\player.data", "wb");
  fwrite(&player.pos,      sizeof(double), 2, dataFile);
  fwrite(&player.speed,    sizeof(double), 1, dataFile);
  fwrite(&player.shotWait, sizeof(int),    1, dataFile);
  size = player.shotBullet.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&player.shotBullet[i].pos,   sizeof(double), 2, dataFile);
    fwrite(&player.shotBullet[i].v,     sizeof(double), 2, dataFile);
    fwrite(&player.shotBullet[i].angle, sizeof(double), 1, dataFile);
  }
  fclose(dataFile);

  return 0;
}