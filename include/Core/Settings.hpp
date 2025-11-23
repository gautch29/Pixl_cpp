#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

struct Settings {
  int standImages = 4;
  int runImages = 8;
  int jumpImages = 6;
  int attackImages = 6;
  int playerSize = 400;
  bool hitbox = false;
  std::string input = "keyboard";
  float scale = 1.66f;
  int gameTime = 180; // in seconds
  std::string time = "day";

  int initialLife = 100;
  int initialStamina = 100;
  int initialFall = 300;

  bool loadFromFile(const std::string &filename);
  bool saveToFile(const std::string &filename);
};
