#include "Core/Settings.hpp"
#include <fstream>
#include <sstream>

bool Settings::loadFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos)
      continue;

    std::string key = line.substr(0, line.find(' '));
    std::string value = line.substr(equalPos + 2);

    if (key == "stand_images")
      standImages = std::stoi(value);
    else if (key == "run_images")
      runImages = std::stoi(value);
    else if (key == "jump_images")
      jumpImages = std::stoi(value);
    else if (key == "attack_images")
      attackImages = std::stoi(value);
    else if (key == "player_size")
      playerSize = std::stoi(value);
    else if (key == "hitbox")
      hitbox = (value == "true");
    else if (key == "input")
      input = value;
    else if (key == "echelle")
      scale = std::stof(value);
    else if (key == "game_time")
      gameTime = std::stoi(value);
    else if (key == "time")
      time = value;
  }

  file.close();
  return true;
}

bool Settings::saveToFile(const std::string &filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  file << "stand_images = " << standImages << "\n";
  file << "run_images = " << runImages << "\n";
  file << "jump_images = " << jumpImages << "\n";
  file << "attack_images = " << attackImages << "\n";
  file << "player_size = " << playerSize << "\n";
  file << "hitbox = " << (hitbox ? "true" : "false") << "\n";
  file << "input = " << input << "\n";
  file << "echelle = " << scale << "\n";
  file << "game_time = " << gameTime << "\n";
  file << "time = " << time << "\n";

  file.close();
  return true;
}
