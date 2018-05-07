#include "Title.h"

#include "Common.h"

#include <unordered_map>

// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;

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
void Title::ReadJSONFile(const char *jsonFileName)
{
  using namespace rapidjson;
  FILE *jsonFile;
  char cJSONBuffer[65536], cTexiBuffer[TEXTSIZE];
  Document jsonData;
  unordered_map<string, int> imageFileMap;
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
      TitleObject object;
      object.id = -1;
      object.link = lNone;
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
        string imageFile(objects[j]["properties"]["filePath"].GetString());
        if (imageFileMap.find(imageFile) == imageFileMap.end()) {
          object.id = (int)(imageFileList.size());
          imageFileMap[imageFile] = object.id;
          imageFileList.push_back(imageFile);
        }
        else object.id = imageFileMap[imageFile];
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
void Title::WriteDataFile(const char *dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = imageFileList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < size; i++) WriteString(imageFileList[i], dataFile);
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
  WriteTitleObjectList(bgList, dataFile);
  WriteTitleObjectList(objectList, dataFile);
  size = linkList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&linkList[i].pos, sizeof(int), 2, dataFile);
    fwrite(&linkList[i].size, sizeof(int), 2, dataFile);
    fwrite(&linkList[i].link, sizeof(LinkType), 1, dataFile);
  }
  fclose(dataFile);
}
void Title::WriteTitleObjectList(vector<TitleObject> &objectList, FILE *&file)
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