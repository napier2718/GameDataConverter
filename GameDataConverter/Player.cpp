#include "Player.h"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

void Player::ReadCSVFile(const char * csvFileName)
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
    if (any_of(record[0].cbegin(), record[0].cend(), isdigit))
    {
      if (isFirst) {
        for (unsigned int i = 0; i < record.size(); i++) {
          switch (types[i]) {
          case 1:
            pos.x = stod(record[i]);
            break;
          case 2:
            pos.y = stod(record[i]);
            break;
          case 3:
            speed = stod(record[i]);
            break;
          case 4:
            graphicID = stoi(record[i]);
            break;
          case 5:
            shotWait = stoi(record[i]);
            break;
          }
        }
        isFirst = false;
      }
      else {
        Bullet bullet;
        double speed;
        for (unsigned int i = 0; i < record.size(); i++) {
          switch (types[i]) {
          case 1:
            bullet.pos.x = stod(record[i]);
            break;
          case 2:
            bullet.pos.y = stod(record[i]);
            break;
          case 3:
            speed = stod(record[i]);
            bullet.v.set(0.0, speed);
            break;
          case 4:
            bullet.graphicID = stoi(record[i]);
            break;
          case 5:
            bullet.hitboxID = stoi(record[i]);
            break;
          case 6:
            bullet.angle = stod(record[i]);
            break;
          }
        }
        bullet.v.rotate(bullet.angle);
        shotBullet.push_back(bullet);
      }
    }
    else
    {
      types.clear();
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "posX" || record[i] == "PosX") types.push_back(1);
        else if (record[i] == "posY" || record[i] == "PosY") types.push_back(2);
        else if (record[i] == "speed" || record[i] == "Speed") types.push_back(3);
        else if (record[i] == "graphic" || record[i] == "Graphic") types.push_back(4);
        else if (record[i] == "hitbox" || record[i] == "Hitbox") types.push_back(5);
        else if (record[i] == "shotWait" || record[i] == "ShotWait") types.push_back(5);
        else if (record[i] == "angle" || record[i] == "Angle") types.push_back(6);
        else types.push_back(0);
      }
    }
  }
}
void Player::WriteDataFile(const char * dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  fwrite(&pos, sizeof(double), 2, dataFile);
  fwrite(&speed, sizeof(double), 1, dataFile);
  fwrite(&graphicID, sizeof(int), 1, dataFile);
  fwrite(&shotWait, sizeof(int), 1, dataFile);
  size_t size = shotBullet.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&shotBullet[i].pos, sizeof(double), 2, dataFile);
    fwrite(&shotBullet[i].v, sizeof(double), 2, dataFile);
    fwrite(&shotBullet[i].graphicID, sizeof(int), 1, dataFile);
    fwrite(&shotBullet[i].hitboxID, sizeof(int), 1, dataFile);
    fwrite(&shotBullet[i].angle, sizeof(double), 1, dataFile);
  }
  fclose(dataFile);
}
