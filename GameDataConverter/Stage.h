#pragma once
#include "EnemyPattern.h"
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
  Stage(const char *csvFileName, EnemyPattern &ePattern) { ReadCSVFile(csvFileName, ePattern); }
  void ReadCSVFile(const char *csvFileName, EnemyPattern &ePattern);
  void WriteDataFile(const char *dataFileName);
private:
  std::vector<Enemy> enemyList;
};