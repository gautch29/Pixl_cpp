#pragma once

#include "Animation.hpp"
#include "Character.hpp"
#include <SDL2/SDL.h>
#include <memory>

class ResourceManager;
struct Settings;
struct PlayerInput;

enum class PlayerMode {
  STAND = 0,
  RUN = 1,
  JUMP = 2,
  JUMPED = 3,
  SIDE_JUMP = 4,
  ATTACK = 5,
  DASH = 6
};

enum class Direction { LEFT, RIGHT };

class Player {
public:
  Player(int playerId, SDL_Color color);

  void initialize(Character *character, int startX, int startY,
                  ResourceManager *resources, Settings *settings);

  void update(float deltaTime, int baseHeight, int screenWidth);
  void render(SDL_Renderer *renderer);

  void handleInput(PlayerInput &input);

  // Getters
  SDL_Rect getHitbox() const { return hitbox; }
  float getLife() const { return life; }
  float getStamina() const { return stamina; }
  Direction getDirection() const { return direction; }
  PlayerMode getMode() const { return mode; }
  SDL_Point getPosition() const { return {posX, posY}; }
  bool getWantedAttack() const { return wantedAttack; }

  // Combat
  void takeDamage(float damage);
  bool canAttack() const;
  void performAttack();

  // Setters
  void setLife(float l) { life = l; }
  void setStamina(float s) { stamina = s; }

private:
  void updateControls(PlayerInput &input);
  void updateMode(int baseHeight, int screenWidth);
  void updatePhysics(float deltaTime, int baseHeight);
  void updateAnimation();

  void loadAnimations(ResourceManager *resources);

  int playerId;
  std::string pseudo;
  SDL_Color mainColor;

  Character *character;
  Settings *settings;

  int posX, posY;
  float speedX, speedY;
  float accelX, accelY;

  SDL_Rect hitbox;
  Direction direction;
  PlayerMode mode;
  PlayerMode wantedMode;
  bool wantedAttack;
  bool wantedDash;

  float life;
  float stamina;

  Uint32 lastHitTime;

  // Combat Juice
  float knockbackX;
  int stunTimer;

  // Dash
  int dashTimer;
  const int DASH_DURATION = 15; // frames
  const float DASH_SPEED = 25.0f;
  const float DASH_COST = 20.0f;

  // Animations
  Animation standLeft, standRight;
  Animation runLeft, runRight;
  Animation jumpLeft, jumpRight;
  Animation attackLeft, attackRight;

  Animation *currentAnimation;
};
