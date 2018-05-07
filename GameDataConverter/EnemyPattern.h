#pragma once
#include <unordered_map>

enum BasePatternType
{
  wait,
  move_normal,
  shot,
};
enum DirectionType
{
  dNone,
  up,
  down,
  right,
  left,
  player,
};
struct BasePattern
{
  BasePatternType pattern;
  DirectionType direction;
};
struct Pattern
{
  std::vector<BasePattern> list;
};
class EnemyPattern
{
public:
  EnemyPattern(const char *csvFileName) { ReadCSVFile(csvFileName); }
  void ReadCSVFile(const char *csvFileName);
  void WriteDataFile(const char *dataFileName);
  int GetPatternPosition(const std::string &name) { return patternMap[name]; }
private:
  struct Token
  {
    Token(std::string n, DirectionType d, int r) :name(n), direction(d), repeat(r) {}
    std::string name;
    DirectionType direction;
    int repeat;
  };
  std::vector<BasePattern> ExpandToken(const Token&);
  std::unordered_map<std::string, std::vector<Token>> tokensMap;
  std::vector<Pattern> patterns;
  std::unordered_map<std::string, int> patternMap;
};