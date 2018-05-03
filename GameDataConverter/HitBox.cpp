#include "HitBox.h"

#include <fstream>
#include <sstream>

using namespace std;

void HitBox::ReadCSVFile(const char * csvFileName)
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
      list.push_back(hitbox);
    }
  }
}
void HitBox::WriteDataFile(const char * dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = list.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&list[i], sizeof(double), 2, dataFile);
  }
}
