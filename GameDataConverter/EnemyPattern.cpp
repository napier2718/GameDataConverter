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
    bool isReadDirection = false;
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
        else {
          if (isReadDirection) {
            tokens.back().direction = CheckDirectionType(buffer);
            isReadDirection = false;
          }
          else {
            tokens.push_back(Token(buffer, DirectionType::dNone, 1));
            BasePatternType bpt = CheckBasePatternType(buffer);
            if (bpt >= move_normal && bpt <= shot) isReadDirection = true;
          }
        }
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
  BasePatternType bpt = CheckBasePatternType(token.name);
  if (bpt == pNone) {
    for (const Token childToken : tokensMap[token.name]) {
      const vector<BasePattern> childList = ExpandToken(childToken);
      fragment.reserve(fragment.size() + childList.size() * childToken.repeat);
      for (int i = 0; i<childToken.repeat; i++) fragment.insert(fragment.end(), childList.begin(), childList.end());
    }
  }
  else if (bpt == wait)fragment.push_back({ bpt, DirectionType::dNone });
  else fragment.push_back({ bpt, token.direction });
  return fragment;
}
BasePatternType EnemyPattern::CheckBasePatternType(const string &tokenName)
{
  if (tokenName == "wait") return BasePatternType::wait;
  else if (tokenName == "move_normal") return BasePatternType::move_normal;
  else if (tokenName == "shot") return BasePatternType::shot;
  return BasePatternType::pNone;
}
DirectionType EnemyPattern::CheckDirectionType(const string &tokenName)
{
  if (tokenName == "UP") return DirectionType::up;
  else if (tokenName == "DOWN") return DirectionType::down;
  else if (tokenName == "RIGHT") return DirectionType::right;
  else if (tokenName == "LEFT") return DirectionType::left;
  else if (tokenName == "PLAYER") return DirectionType::player;
  return DirectionType::dNone;
}