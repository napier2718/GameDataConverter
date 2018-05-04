#include "EnemyPattern.h"
#include "Graphic.h"
#include "HitBox.h"
#include "Player.h"
#include "Stage.h"
#include "Title.h"

int main()
{
  Title title("data\\title.json");
  title.WriteDataFile("data\\title.data");

  Graphic graphic("data\\graphicFile.csv", "data\\graphicImage.csv");
  graphic.WriteDataFile("data\\graphic.data");

  HitBox hitbox("data\\hitbox.csv");
  hitbox.WriteDataFile("data\\hitbox.data");

  Player player("data\\player.csv");
  player.WriteDataFile("data\\player.data");

  EnemyPattern ePattern("data\\enemyPattern.csv");
  ePattern.WriteDataFile("data\\enemyPattern.data");

  Stage stage("data\\stage.csv", ePattern);
  stage.WriteDataFile("data\\stage.data");

  return 0;
}