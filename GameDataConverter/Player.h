#pragma once
#include "Vector.h"

#include <vector>

struct Bullet
{
  int graphicID;
  Vector<double> pos, v;
  double angle;
};
class Player
{
public:
  Player(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
private:
  int graphicID;
  Vector<double> pos;
  double speed;
  int shotWait;
  std::vector<Bullet> shotBullet;
};