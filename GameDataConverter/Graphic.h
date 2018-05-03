#pragma once
#include "Vector.h"

#include <String>
#include <vector>

struct GraphicFile
{
  std::string path;
  Vector<int> size, div;
};
struct GraphicImage
{
  int gHandleID;
  Vector<int> size;
  int hitboxID;
  bool enableAnimation;
  int animationFrame, animationWait;
};
class Graphic
{
public:
  Graphic(const char *fileCSVFileName, const char *imageCSVFileName) { ReadCSVFile(fileCSVFileName, imageCSVFileName); }
  void ReadCSVFile(const char *fileCSVFileName, const char *imageCSVFileName);
  void WriteDataFile(const char *dataFileName);
private:
  std::vector<GraphicFile> fileList;
  std::vector<GraphicImage> imageList;
};