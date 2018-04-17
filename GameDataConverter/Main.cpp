#include "Vector.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <String>
#include <unordered_map>
#include <vector>
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
  int colorID;
  string text;
};
struct GraphicImage
{
  string filepath;
  Vector<int> size, div;
};
struct GraphicPattern
{
  int gHandleID;
  Vector<int> size;
  bool enableAnimation;
  int animationFrame, animationWait;
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
struct Enemy
{
  Vector<double> pos;
  int time, graphicID, patternID;
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
void WriteString(string &text, FILE *&file)
{
  int length = (int)(text.size());
  fwrite(&length, sizeof(int), 1, file);
  fwrite(text.c_str(), sizeof(char), length, file);
}
void WriteObjectList(vector<Object> &objectList, FILE *&file)
{
  size_t size = objectList.size();
  for (size_t i = 0; i < size; i++) {
    fwrite(&objectList[i].type, sizeof(ObjectType), 1, file);
    fwrite(&objectList[i].pos, sizeof(int), 2, file);
    fwrite(&objectList[i].size, sizeof(int), 2, file);
    fwrite(&objectList[i].id, sizeof(int), 1, file);
    if (objectList[i].type == text) {
      fwrite(&objectList[i].colorID, sizeof(int), 1, file);
      WriteString(objectList[i].text, file);
    }
  }
}
void ReadTitleJSONFile(const char *jsonFileName, vector<string> &imageList, vector<unsigned int> &colorList, vector<pair<string, int>> &fontList, vector<Object> &bgList, vector<Object> &objectList, vector<Object> &linkList)
{
  using namespace rapidjson;
  FILE *jsonFile;
  char cJSONBuffer[65536], cTexiBuffer[TEXTSIZE];
  Document jsonData;
  unordered_map<string, int> imageMap;
  unordered_map<string, int> colorMap;
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
        string color(objects[j]["text"]["color"].GetString());
        if (colorMap.find(color) == colorMap.end()) {
          object.colorID = (int)(colorMap.size());
          colorMap[color] = object.colorID;
          colorList.push_back(0xff000000 + stoi(color.c_str() + 1, NULL, 16));
        }
        else object.id = colorMap[color];
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
void WriteTitleDataFile(const char *dataFileName, vector<string> &imageList, vector<unsigned int> &colorList, vector<pair<string, int>> &fontList, vector<Object> &bgList, vector<Object> &objectList, vector<Object> &linkList)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = imageList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) WriteString(imageList[i], dataFile);
  size = colorList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) fwrite(&colorList[i], sizeof(unsigned int), 1, dataFile);
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
  WriteObjectList(bgList, dataFile);
  WriteObjectList(objectList, dataFile);
  size = linkList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&linkList[i].pos, sizeof(int), 2, dataFile);
    fwrite(&linkList[i].size, sizeof(int), 2, dataFile);
    fwrite(&linkList[i].link, sizeof(LinkType), 1, dataFile);
  }
  fclose(dataFile);
}
void ReadGraphicCSVFile(const char *fileCsvFileName, const char *imageCsvFileName, vector<GraphicImage> &imageList, vector<GraphicPattern> &patternList)
{
  ifstream csvFile(fileCsvFileName);
  vector<int> types;
  bool isHeader = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (isHeader) {
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "filepath" || record[i] == "FilePath") types.push_back(1);
        else if (record[i] == "sizeX" || record[i] == "SizeX") types.push_back(2);
        else if (record[i] == "sizeY" || record[i] == "SizeY") types.push_back(3);
        else if (record[i] == "divX" || record[i] == "DivX") types.push_back(4);
        else if (record[i] == "divY" || record[i] == "DivY") types.push_back(5);
        else types.push_back(0);
      }
      isHeader = false;
    }
    else {
      GraphicImage image;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          image.filepath = record[i];
          break;
        case 2:
          image.size.x = stoi(record[i]);
          break;
        case 3:
          image.size.y = stoi(record[i]);
          break;
        case 4:
          image.div.x = stoi(record[i]);
          break;
        case 5:
          image.div.y = stoi(record[i]);
          break;
        }
      }
      imageList.push_back(image);
    }
  }
  csvFile.close();

  csvFile.open(imageCsvFileName);
  types.clear();
  isHeader = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (isHeader) {
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "gHandleID" || record[i] == "GHandleID") types.push_back(1);
        else if (record[i] == "sizeX" || record[i] == "SizeX") types.push_back(2);
        else if (record[i] == "sizeY" || record[i] == "SizeY") types.push_back(3);
        else if (record[i] == "enableAnimation" || record[i] == "EnableAnimation") types.push_back(4);
        else if (record[i] == "animationFrame" || record[i] == "AnimationFrame") types.push_back(5);
        else if (record[i] == "animationWait" || record[i] == "AnimationWait") types.push_back(6);
        else types.push_back(0);
      }
      isHeader = false;
    }
    else {
      GraphicPattern pattern;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          pattern.gHandleID = stoi(record[i]);
          break;
        case 2:
          pattern.size.x = stoi(record[i]);
          break;
        case 3:
          pattern.size.y = stoi(record[i]);
          break;
        case 4:
          pattern.enableAnimation = (stoi(record[i]) == 1 ? true : false);
          break;
        case 5:
          pattern.animationFrame = stoi(record[i]);
          break;
        case 6:
          pattern.animationWait = stoi(record[i]);
          break;
        }
      }
      patternList.push_back(pattern);
    }
  }
}
void WriteGraphicDataFile(const char *dataFileName, vector<GraphicImage> &imageList, vector<GraphicPattern> &patternList)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = imageList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    WriteString(imageList[i].filepath, dataFile);
    fwrite(&imageList[i].size, sizeof(int), 2, dataFile);
    fwrite(&imageList[i].div,  sizeof(int), 2, dataFile);
  }
  size = patternList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&patternList[i].gHandleID, sizeof(int), 1, dataFile);
    fwrite(&patternList[i].size, sizeof(int), 2, dataFile);
    fwrite(&patternList[i].enableAnimation, sizeof(bool), 1, dataFile);
    fwrite(&patternList[i].animationFrame, sizeof(int), 2, dataFile);
  }
  fclose(dataFile);
}
void ReadHitBoxCSVFile(const char *csvFileName, vector<Vector<double>> &hitboxList)
{
  ifstream csvFile(csvFileName);
  vector<int> types;
  bool isHeader = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (isHeader) {
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "sizeX" || record[i] == "SizeX") types.push_back(1);
        else if (record[i] == "sizeY" || record[i] == "SizeY") types.push_back(2);
        else types.push_back(0);
      }
      isHeader = false;
    }
    else {
      Vector<double> hitbox;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          hitbox.x = stod(record[i]);
          break;
        case 2:
          hitbox.y = stod(record[i]);
          break;
        }
      }
      hitboxList.push_back(hitbox);
    }
  }
}
void WriteHitBoxDataFile(const char *dataFileName, vector<Vector<double>> &hitboxList)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = hitboxList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&hitboxList[i], sizeof(double), 2, dataFile);
  }
}
void ReadPlayerCSVFile(const char *csvFileName, Player &player)
{
  ifstream csvFile(csvFileName);
  vector<int> types;
  bool isFirst = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (any_of(record[0].cbegin(), record[0].cend(), isdigit))
    {
      if (isFirst) {
        for (unsigned int i = 0; i < record.size(); i++) {
          switch (types[i]) {
          case 1:
            player.pos.x = stod(record[i]);
            break;
          case 2:
            player.pos.y = stod(record[i]);
            break;
          case 3:
            player.speed = stod(record[i]);
            break;
          case 4:
            player.shotWait = stoi(record[i]);
            break;
          }
        }
        isFirst = false;
      }
      else {
        Bullet bullet;
        double speed;
        for (unsigned int i = 0; i < record.size(); i++) {
          switch (types[i]) {
          case 1:
            bullet.pos.x = stod(record[i]);
            break;
          case 2:
            bullet.pos.y = stod(record[i]);
            break;
          case 3:
            speed = stod(record[i]);
            bullet.v.set(0.0, speed);
            break;
          case 4:
            bullet.angle = stod(record[i]);
            break;
          }
        }
        bullet.v.rotate(bullet.angle);
        player.shotBullet.push_back(bullet);
      }
    }
    else
    {
      types.clear();
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "posX" || record[i] == "PosX") types.push_back(1);
        else if (record[i] == "posY" || record[i] == "PosY") types.push_back(2);
        else if (record[i] == "speed" || record[i] == "Speed") types.push_back(3);
        else if (record[i] == "shotWait" || record[i] == "ShotWait") types.push_back(4);
        else if (record[i] == "angle" || record[i] == "Angle") types.push_back(4);
        else types.push_back(0);
      }
    }
  }
}
void WritePlayerDataFile(const char *dataFileName, Player &player)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  fwrite(&player.pos, sizeof(double), 2, dataFile);
  fwrite(&player.speed, sizeof(double), 1, dataFile);
  fwrite(&player.shotWait, sizeof(int), 1, dataFile);
  size_t size = player.shotBullet.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&player.shotBullet[i].pos, sizeof(double), 2, dataFile);
    fwrite(&player.shotBullet[i].v, sizeof(double), 2, dataFile);
    fwrite(&player.shotBullet[i].angle, sizeof(double), 1, dataFile);
  }
  fclose(dataFile);
}
void ReadStageCSVFile(const char *csvFileName, vector<Enemy> &enemyList)
{
  ifstream csvFile(csvFileName);
  vector<int> types;
  bool isFirst = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (isFirst) {
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "posX" || record[i] == "PosX") types.push_back(1);
        else if (record[i] == "posY" || record[i] == "PosY") types.push_back(2);
        else if (record[i] == "time" || record[i] == "Time") types.push_back(3);
        else if (record[i] == "graphic" || record[i] == "Graphic") types.push_back(4);
        else if (record[i] == "pattern" || record[i] == "Pattern") types.push_back(5);
        else types.push_back(0);
      }
      isFirst = false;
    }
    else {
      Enemy enemy;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          enemy.pos.x = stod(record[i]);
          break;
        case 2:
          enemy.pos.y = stod(record[i]);
          break;
        case 3:
          enemy.time = stoi(record[i]);
          break;
        case 4:
          enemy.graphicID = stoi(record[i]);
          break;
        case 5:
          enemy.patternID = stoi(record[i]);
          break;
        }
      }
      enemyList.push_back(enemy);

    }
  }
}
void WriteStageDataFile(const char *dataFileName, vector<Enemy> &enemyList)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = enemyList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&enemyList[i].pos, sizeof(double), 2, dataFile);
    fwrite(&enemyList[i].time, sizeof(int), 3, dataFile);
  }
  fclose(dataFile);
}
int main()
{
  vector<string> fileList;
  vector<unsigned int> colorList;
  vector<pair<string, int>> fontList;
  vector<Object> bgList, objectList, linkList;
  ReadTitleJSONFile("data\\title.json", fileList, colorList, fontList, bgList, objectList, linkList);
  WriteTitleDataFile("data\\title.data", fileList, colorList, fontList, bgList, objectList, linkList);

  vector<GraphicImage> imageList;
  vector<GraphicPattern> patternList;
  ReadGraphicCSVFile("data\\graphicFile.csv", "data\\graphicImage.csv", imageList, patternList);
  WriteGraphicDataFile("data\\graphic.data", imageList, patternList);

  vector<Vector<double>> hitboxList;
  ReadHitBoxCSVFile("data\\hitbox.csv", hitboxList);
  WriteHitBoxDataFile("data\\hitbox.data", hitboxList);

  Player player;
  ReadPlayerCSVFile("data\\player.csv", player);
  WritePlayerDataFile("data\\player.data", player);

  vector<Enemy> enemyList;
  ReadStageCSVFile("data\\stage.csv", enemyList);
  WriteStageDataFile("data\\stage.data", enemyList);

  return 0;
}