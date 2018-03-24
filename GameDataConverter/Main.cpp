#include <cstdio>
#include <cstring>
// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

int main()
{
  //FindFirstFile();
  std::FILE *jsonFile;
  fopen_s(&jsonFile, "data\\title.json", "r");
  char cBuffer[65536];
  FileReadStream jsonFS(jsonFile, cBuffer, sizeof(cBuffer));
  Document jsonData;
  jsonData.ParseStream(jsonFS);
  const int layerNum = jsonData["layers"].Size();
  Value *imageData, *eventData;
  for (int i = 0; i < layerNum; i++) {
    if (strncmp(jsonData["layers"][i]["name"].GetString(), "image", 5) == 0) imageData = &jsonData["layers"][i];
    else if (strncmp(jsonData["layers"][i]["name"].GetString(), "event", 5) == 0) eventData = &jsonData["layers"][i];
  }
  printf("%s", (*imageData)["name"].GetString());
  return 0;
}