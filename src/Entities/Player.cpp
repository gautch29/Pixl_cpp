#include "Entities/Player.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Settings.hpp"
#include "Entities/Character.hpp"
#include "Input/InputManager.hpp"
#include <SDL2/SDL_ttf.h>
#include <cmath>

Player::Player(int playerId, SDL_Color color)
    : playerId(playerId), mainColor(color), character(nullptr),
      settings(nullptr), posX(0), posY(0), speedX(0.0f), speedY(0.0f),
      accelX(0.0f), accelY(0.0f), direction(Direction::RIGHT),
      mode(PlayerMode::STAND), wantedMode(PlayerMode::STAND),
      wantedAttack(false), life(1.0f), stamina(100.0f), lastHitTime(0),
      currentAnimation(nullptr) {

  pseudo = "Player " + std::to_string(playerId);
  hitbox = {0, 0, 0, 0};
}

void Player::initialize(Character *character, int startX, int startY,
                        ResourceManager *resources, Settings *settings) {
  this->character = character;
  this->settings = settings;
  this->posX = startX;
  this->posY = startY - settings->initialFall;

  loadAnimations(resources);

  life = 1.0f;
  stamina = settings->initialStamina;
  mode = PlayerMode::STAND;
  currentAnimation = &standRight;
}

void Player::loadAnimations(ResourceManager *resources) {
  int playerSize = settings->playerSize;

  // Load stand animations
  auto standLFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "1Stance/Left/", "StanceL",
      settings->standImages);
  auto standRFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "1Stance/Right/", "StanceR",
      settings->standImages);

  standLeft.setFrames(standLFrames, character->delayStand);
  standRight.setFrames(standRFrames, character->delayStand);

  // Load run animations
  auto runLFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "4Run/Left/", "RunL", settings->runImages);
  auto runRFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "4Run/Right/", "RunR", settings->runImages);

  runLeft.setFrames(runLFrames, character->delayRun);
  runRight.setFrames(runRFrames, character->delayRun);

  // Load jump animations
  auto jumpLFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "3Jump/Left/", "Jump",
      settings->jumpImages);
  auto jumpRFrames = resources->loadAnimationFrames(
      "assets/" + character->path + "3Jump/Right/", "Jump",
      settings->jumpImages);

  jumpLeft.setFrames(jumpLFrames, character->delayJump);
  jumpRight.setFrames(jumpRFrames, character->delayJump);
}

void Player::update(float deltaTime, int baseHeight, int screenWidth) {
  updatePhysics(deltaTime, baseHeight);
  updateAnimation();

  // Update hitbox
  int playerSize = settings->playerSize;
  hitbox.x = posX + playerSize / 4;
  hitbox.y = posY + playerSize / 15;
  hitbox.w = playerSize / 2;
  hitbox.h = playerSize - playerSize / 15;

  // Recover stamina
  stamina += character->staminaRecupSpeed;
  if (stamina > settings->initialStamina) {
    stamina = settings->initialStamina;
  }
}

void Player::handleInput(PlayerInput &input) { updateControls(input); }

void Player::updateControls(PlayerInput &input) {
  bool up = input.controls[0];
  bool left = input.controls[1];
  bool down = input.controls[2];
  bool right = input.controls[3];
  bool attack = input.controls[4];

  wantedAttack = attack;

  // Determine wanted mode based on input
  if (!up && !left && !down && !right) {
    wantedMode = PlayerMode::STAND;
  } else if (!up && left && !down && !right) {
    wantedMode = PlayerMode::RUN;
    direction = Direction::LEFT;
  } else if (!up && !left && !down && right) {
    wantedMode = PlayerMode::RUN;
    direction = Direction::RIGHT;
  } else if (up && !left && !down && !right) {
    wantedMode = PlayerMode::JUMP;
  } else if (up && left && !down && !right) {
    wantedMode = PlayerMode::SIDE_JUMP;
    direction = Direction::LEFT;
  } else if (up && !left && !down && right) {
    wantedMode = PlayerMode::SIDE_JUMP;
    direction = Direction::RIGHT;
  } else {
    wantedMode = PlayerMode::STAND;
  }
}

void Player::updateMode(int baseHeight, int screenWidth) {
  int playerSize = settings->playerSize;
  bool onGround = (posY >= baseHeight - playerSize);

  // Transition to wanted mode if on ground
  if (wantedMode == PlayerMode::STAND && mode != PlayerMode::STAND &&
      onGround) {
    mode = PlayerMode::STAND;
    currentAnimation =
        (direction == Direction::RIGHT) ? &standRight : &standLeft;
    currentAnimation->reset();
  } else if (wantedMode == PlayerMode::RUN && mode != PlayerMode::RUN &&
             onGround) {
    mode = PlayerMode::RUN;
    currentAnimation = (direction == Direction::RIGHT) ? &runRight : &runLeft;
    currentAnimation->reset();
  } else if (wantedMode == PlayerMode::JUMP && mode != PlayerMode::JUMP &&
             onGround) {
    mode = PlayerMode::JUMP;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    currentAnimation->reset();
    speedY = -character->jumpSpeed;
  } else if (wantedMode == PlayerMode::SIDE_JUMP &&
             mode != PlayerMode::SIDE_JUMP && onGround) {
    mode = PlayerMode::SIDE_JUMP;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    currentAnimation->reset();
    speedY = -character->jumpSpeed;
  }

  // If in air and not already in jumped state
  if (!onGround && mode != PlayerMode::JUMPED && mode != PlayerMode::JUMP &&
      mode != PlayerMode::SIDE_JUMP) {
    mode = PlayerMode::JUMPED;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    currentAnimation->reset();
  }

  // Boundary checks
  if (hitbox.x <= 0 &&
      ((wantedMode == PlayerMode::RUN && direction == Direction::LEFT) ||
       mode == PlayerMode::JUMPED || mode == PlayerMode::SIDE_JUMP)) {
    mode = PlayerMode::STAND;
    currentAnimation = &standLeft;
    currentAnimation->reset();
  }

  if (hitbox.x + hitbox.w >= screenWidth &&
      ((wantedMode == PlayerMode::RUN && direction == Direction::RIGHT) ||
       mode == PlayerMode::JUMPED || mode == PlayerMode::SIDE_JUMP)) {
    mode = PlayerMode::STAND;
    currentAnimation = &standRight;
    currentAnimation->reset();
  }
}

void Player::updatePhysics(float deltaTime, int baseHeight) {
  int playerSize = settings->playerSize;
  const float gravity = 0.6f;

  // Update mode first
  updateMode(baseHeight, 1920); // TODO: pass actual screen width

  // Apply mode-specific movement
  if (mode == PlayerMode::STAND) {
    speedX = 0;
  } else if (mode == PlayerMode::RUN) {
    speedX =
        (direction == Direction::RIGHT) ? character->speed : -character->speed;
  } else if (mode == PlayerMode::JUMP) {
    speedX = 0;
  } else if (mode == PlayerMode::SIDE_JUMP) {
    speedX =
        (direction == Direction::RIGHT) ? character->speed : -character->speed;
  }

  // Apply gravity
  if (posY < baseHeight - playerSize) {
    accelY = gravity;
  } else {
    accelY = 0;
    speedY = 0;
    posY = baseHeight - playerSize;
  }

  // Update velocity and position
  speedX += accelX;
  speedY += accelY;
  posX += static_cast<int>(speedX);
  posY += static_cast<int>(speedY);
}

void Player::updateAnimation() {
  if (currentAnimation) {
    currentAnimation->update();
  }
}

void Player::render(SDL_Renderer *renderer) {
  if (!currentAnimation)
    return;

  SDL_Texture *frame = currentAnimation->getCurrentFrame();
  if (!frame)
    return;

  int playerSize = settings->playerSize;
  SDL_Rect destRect = {posX, posY, playerSize, playerSize};
  SDL_RenderCopy(renderer, frame, nullptr, &destRect);

  // Draw hitbox if enabled
  if (settings->hitbox) {
    SDL_SetRenderDrawColor(renderer, mainColor.r, mainColor.g, mainColor.b,
                           255);
    SDL_RenderDrawRect(renderer, &hitbox);
  }
}

void Player::takeDamage(float damage) {
  life -= damage;
  if (life < 0)
    life = 0;
}

bool Player::canAttack() const {
  Uint32 currentTime = SDL_GetTicks();
  return stamina > character->staminaHit &&
         (currentTime - lastHitTime) > static_cast<Uint32>(character->delayHit);
}

void Player::performAttack() {
  stamina -= character->staminaHit;
  if (stamina < 0)
    stamina = 0;
  lastHitTime = SDL_GetTicks();
}
