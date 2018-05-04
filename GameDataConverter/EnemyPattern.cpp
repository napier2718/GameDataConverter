#include "EnemyPattern.h"

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

void EnemyPattern::ReadCSVFile(const char *csvFileName)
{
  ifstream csvFile(csvFileName);
  while (!csvFile.eof()) {
    bool isFirst = true;
    string name;
    vector<Token> tokens;
    string buffer;
    csvFile >> buffer;
    if (buffer.empty()) break;
    istringstream sBuffer(buffer);
    while (getline(sBuffer, buffer, ',')) {
      if (isFirst) {
        name = buffer;
        isFirst = false;
      }
      else {
        if (buffer.empty()) break;
        if (all_of(buffer.cbegin(), buffer.cend(), isdigit)) tokens.back().repeat = stoi(buffer);
        else tokens.push_back(Token(buffer, 1));
      }
    }
    tokensMap[name] = tokens;
  }
  // “WŠJ
  for (const auto pair : tokensMap) {
    Pattern pattern;
    for (const Token token : pair.second) {
      const vector<PatternType> temp = ExpandToken(token.token);
      pattern.list.reserve(pattern.list.size() + temp.size() * token.repeat);
      for (int i = 0; i<token.repeat; i++) pattern.list.insert(pattern.list.end(), temp.begin(), temp.end());
    }
    listMap[pair.first] = (int)list.size();
    list.push_back(pattern);
  }
}
void EnemyPattern::WriteDataFile(const char *dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = list.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < list.size(); i++)
  {
    size = list[i].list.size();
    fwrite(&size, sizeof(int), 1, dataFile);
    fwrite(&list[i].list[0], sizeof(PatternType), size, dataFile);
  }
}
vector<PatternType> EnemyPattern::ExpandToken(const string &token)
{
  vector<PatternType> tempList;
  if (token == "wait") tempList.push_back(wait);
  else if (token == "normal_move") tempList.push_back(normal_move);
  else if (token == "shot") tempList.push_back(shot);
  else {
    for (const Token child : tokensMap[token]) {
      const vector<PatternType> temp = ExpandToken(child.token);
      tempList.reserve(tempList.size() + temp.size() * child.repeat);
      for (int i = 0; i<child.repeat; i++) tempList.insert(tempList.end(), temp.begin(), temp.end());
    }
  }
  return tempList;
}