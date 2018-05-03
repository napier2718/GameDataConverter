#pragma once
#include "Vector.h"

#include <vector>

struct Enemy
{
  Vector<double> pos;
  int time, graphicID, patternID;
};
class Stage
{
public:
  Stage(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
private:
  std::vector<Enemy> enemyList;
};