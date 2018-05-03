#pragma once
#include "Vector.h"

#include <vector>

class HitBox
{
public:
  HitBox(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
private:
  std::vector<Vector<double>> list;
};