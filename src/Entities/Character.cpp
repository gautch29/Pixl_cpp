#include "Entities/Character.hpp"
#include <fstream>
#include <sstream>

Character::Character()
    : id(0), speed(0), jumpSpeed(0), staminaRecupSpeed(0.0f), damages(0.0f),
      staminaHit(0.0f), delayHit(0), delayStand(0), delayRun(0), delayJump(0),
      profilePicture(nullptr) {}

bool Character::loadFromFile(const std::string &basePath) {
  this->path = basePath;
  std::string specFile = basePath + "Specs.txt";

  std::ifstream file(specFile);
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

    if (key == "name")
      name = value;
    else if (key == "ID")
      id = std::stoi(value);
    else if (key == "speed")
      speed = std::stoi(value);
    else if (key == "jumpspeed")
      jumpSpeed = std::stoi(value);
    else if (key == "staminaRecupSpeed")
      staminaRecupSpeed = std::stof(value);
    else if (key == "damages")
      damages = std::stof(value);
    else if (key == "staminaHit")
      staminaHit = std::stof(value);
    else if (key == "delayHit")
      delayHit = std::stoi(value);
    else if (key == "delayStand")
      delayStand = std::stoi(value);
    else if (key == "delayRun")
      delayRun = std::stoi(value);
    else if (key == "delayJump")
      delayJump = std::stoi(value);
  }

  file.close();
  return true;
}
