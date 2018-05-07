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
        else if (buffer == "UP") tokens.back().direction = DirectionType::up;
        else if (buffer == "DOWN") tokens.back().direction = DirectionType::down;
        else if (buffer == "RIGHT") tokens.back().direction = DirectionType::right;
        else if (buffer == "LEFT") tokens.back().direction = DirectionType::left;
        else if (buffer == "PLAYER") tokens.back().direction = DirectionType::player;
        else tokens.push_back(Token(buffer, DirectionType::dNone, 1));
      }
    }
    tokensMap[name] = tokens;
  }
  // “WŠJ
  for (const auto pair : tokensMap) {
    Pattern pattern;
    for (const Token token : pair.second) {
      const vector<BasePattern> fragment = ExpandToken(token);
      pattern.list.reserve(pattern.list.size() + fragment.size() * token.repeat);
      for (int i = 0; i<token.repeat; i++) pattern.list.insert(pattern.list.end(), fragment.begin(), fragment.end());
    }
    patternMap[pair.first] = (int)patterns.size();
    patterns.push_back(pattern);
  }
}
void EnemyPattern::WriteDataFile(const char *dataFileName)
{
  FILE *dataFile;
  fopen_s(&dataFile, dataFileName, "wb");
  size_t size = patterns.size();
  fwrite(&size, sizeof(int), 1, dataFile);
  for (size_t i = 0; i < patterns.size(); i++)
  {
    size = patterns[i].list.size();
    fwrite(&size, sizeof(int), 1, dataFile);
    fwrite(&patterns[i].list[0], sizeof(BasePattern), size, dataFile);
  }
}
vector<BasePattern> EnemyPattern::ExpandToken(const Token &token)
{
  vector<BasePattern> fragment;
  if (token.name == "wait") fragment.push_back({ wait, DirectionType::dNone });
  else if (token.name == "move_normal") fragment.push_back({ move_normal, token.direction });
  else if (token.name == "shot") fragment.push_back({ shot, token.direction });
  else {
    for (const Token childToken : tokensMap[token.name]) {
      const vector<BasePattern> childList = ExpandToken(childToken);
      fragment.reserve(fragment.size() + childList.size() * childToken.repeat);
      for (int i = 0; i<childToken.repeat; i++) fragment.insert(fragment.end(), childList.begin(), childList.end());
    }
  }
  return fragment;
}