#pragma once
#include <unordered_map>

struct Token
{
  Token(std::string t, int r) :token(t), repeat(r) {}
  std::string token;
  int repeat;
};
enum PatternType
{
  wait,
  normal_move,
  shot,
};
struct Pattern
{
  std::vector<PatternType> list;
};
class EnemyPattern
{
public:
  EnemyPattern(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
  int GetListPosition(const std::string &name) { return listMap[name]; }
private:
  std::vector<PatternType> ExpandToken(const std::string &token);
  std::unordered_map<std::string, std::vector<Token>> tokensMap;
  std::vector<Pattern> list;
  std::unordered_map<std::string, int> listMap;
};