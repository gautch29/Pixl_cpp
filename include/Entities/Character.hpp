#pragma once

#include <SDL2/SDL.h>
#include <string>

class Character {
public:
  Character();

  bool loadFromFile(const std::string &path);

  std::string name;
  std::string path;
  int id;

  int speed;
  int jumpSpeed;

  float staminaRecupSpeed;
  float damages;
  float staminaHit;

  int delayHit;
  int delayAttack;
  int delayStand;
  int delayRun;
  int delayJump;

  SDL_Texture *profilePicture;
};
