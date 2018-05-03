#pragma once
#include "Vector.h"

#include <vector>

struct Bullet
{
  Vector<double> pos, v;
  int graphicID, hitboxID;
  double angle;
};
class Player
{
public:
  Player(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
private:
  Vector<double> pos;
  double speed;
  int graphicID, shotWait;
  std::vector<Bullet> shotBullet;
};