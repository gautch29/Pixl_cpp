#include "Core/Settings.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

bool Settings::loadFromFile(const std::string &filename) {
  std::ifstream inFile(filename);
  if (!inFile.is_open()) {
    return false;
  }
  try {
    nlohmann::json j;
    inFile >> j;
    // Populate fields with defaults if missing
    standImages = j.value("standImages", standImages);
    runImages = j.value("runImages", runImages);
    jumpImages = j.value("jumpImages", jumpImages);
    attackImages = j.value("attackImages", attackImages);
    playerSize = j.value("playerSize", playerSize);
    hitbox = j.value("hitbox", hitbox);
    input = j.value("input", input);
    scale = j.value("scale", scale);
    gameTime = j.value("gameTime", gameTime);
    time = j.value("time", time);
    initialLife = j.value("initialLife", initialLife);
    initialStamina = j.value("initialStamina", initialStamina);
    initialFall = j.value("initialFall", initialFall);
  } catch (...) {
    return false;
  }
  return true;
}

bool Settings::saveToFile(const std::string &filename) {
  std::ofstream outFile(filename);
  if (!outFile.is_open()) {
    return false;
  }
  nlohmann::json j;
  j["standImages"] = standImages;
  j["runImages"] = runImages;
  j["jumpImages"] = jumpImages;
  j["attackImages"] = attackImages;
  j["playerSize"] = playerSize;
  j["hitbox"] = hitbox;
  j["input"] = input;
  j["scale"] = scale;
  j["gameTime"] = gameTime;
  j["time"] = time;
  j["initialLife"] = initialLife;
  j["initialStamina"] = initialStamina;
  j["initialFall"] = initialFall;
  outFile << j.dump(4);
  return true;
}
