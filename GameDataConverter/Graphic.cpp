#include "Graphic.h"

#include "Common.h"

#include <fstream>
#include <sstream>

using namespace std;

void Graphic::ReadCSVFile(const char *fileCSVFileName, const char *imageCSVFileName)
{
  ifstream csvFile(fileCSVFileName);
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
        if (record[i] == "filepath" || record[i] == "FilePath") types.push_back(1);
        else if (record[i] == "sizeX" || record[i] == "SizeX") types.push_back(2);
        else if (record[i] == "sizeY" || record[i] == "SizeY") types.push_back(3);
        else if (record[i] == "divX" || record[i] == "DivX") types.push_back(4);
        else if (record[i] == "divY" || record[i] == "DivY") types.push_back(5);
        else types.push_back(0);
      }
      isHeader = false;
    }
    else {
      GraphicFile file;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          file.path = record[i];
          break;
        case 2:
          file.size.x = stoi(record[i]);
          break;
        case 3:
          file.size.y = stoi(record[i]);
          break;
        case 4:
          file.div.x = stoi(record[i]);
          break;
        case 5:
          file.div.y = stoi(record[i]);
          break;
        }
      }
      fileList.push_back(file);
    }
  }
  csvFile.close();

  csvFile.open(imageCSVFileName);
  types.clear();
  isHeader = true;
  while (!csvFile.eof()) {
    vector<string> record;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) record.push_back(buffer);
    if (isHeader) {
      for (unsigned int i = 0; i < record.size(); i++) {
        if (record[i] == "gHandleID" || record[i] == "GHandleID") types.push_back(1);
        else if (record[i] == "sizeX" || record[i] == "SizeX") types.push_back(2);
        else if (record[i] == "sizeY" || record[i] == "SizeY") types.push_back(3);
        else if (record[i] == "hitbox" || record[i] == "Hitbox") types.push_back(4);
        else if (record[i] == "enableAnimation" || record[i] == "EnableAnimation") types.push_back(5);
        else if (record[i] == "animationFrame" || record[i] == "AnimationFrame") types.push_back(6);
        else if (record[i] == "animationWait" || record[i] == "AnimationWait") types.push_back(7);
        else types.push_back(0);
      }
      isHeader = false;
    }
    else {
      GraphicImage image;
      for (unsigned int i = 0; i < record.size(); i++) {
        switch (types[i]) {
        case 1:
          image.gHandleID = stoi(record[i]);
          break;
        case 2:
          image.size.x = stoi(record[i]);
          break;
        case 3:
          image.size.y = stoi(record[i]);
          break;
        case 4:
          image.hitboxID = stoi(record[i]);
          break;
        case 5:
          image.enableAnimation = (stoi(record[i]) == 1 ? true : false);
          break;
        case 6:
          image.animationFrame = stoi(record[i]);
          break;
        case 7:
          image.animationWait = stoi(record[i]);
          break;
        }
      }
      imageList.push_back(image);
    }
  }
}
void Graphic::WriteDataFile(const char *dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = fileList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    WriteString(fileList[i].path, dataFile);
    fwrite(&fileList[i].size, sizeof(int), 2, dataFile);
    fwrite(&fileList[i].div, sizeof(int), 2, dataFile);
  }
  size = imageList.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (int i = 0; i < size; i++) {
    fwrite(&imageList[i].gHandleID, sizeof(int), 1, dataFile);
    fwrite(&imageList[i].size, sizeof(int), 2, dataFile);
    fwrite(&imageList[i].hitboxID, sizeof(int), 1, dataFile);
    fwrite(&imageList[i].enableAnimation, sizeof(bool), 1, dataFile);
    fwrite(&imageList[i].animationFrame, sizeof(int), 2, dataFile);
  }
  fclose(dataFile);
}
