#include "Stage.h"

#include <fstream>
#include <sstream>

using namespace std;

void Stage::ReadCSVFile(const char * csvFileName, EnemyPattern &ePattern)
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
          enemy.patternID = ePattern.GetListPosition(record[i]);
          break;
        }
      }
      enemyList.push_back(enemy);

    }
  }
}
void Stage::WriteDataFile(const char * dataFileName)
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
