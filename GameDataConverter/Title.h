#pragma once
#include "Vector.h"

#include <String>
#include <vector>

enum ObjectType
{
  image,
  text,
  link,
};
enum LinkType
{
  lNone,
  start,
  end,
};
struct TitleObject
{
  ObjectType type;
  Vector<int> pos, size;
  int id;
  LinkType link;
  int colorID;
  std::string text;
};
class Title
{
public:
  Title(const char *jsonFileName) { ReadJSONFile(jsonFileName); }
  void ReadJSONFile(const char *jsonFileName);
  void WriteDataFile(const char *dataFileName);
private:
  void WriteTitleObjectList(std::vector<TitleObject>&, FILE*&);
  std::vector<std::string> imageFileList;
  std::vector<unsigned int> colorList;
  std::vector<std::pair<std::string, int>> fontList;
  std::vector<TitleObject> bgList, objectList, linkList;
};