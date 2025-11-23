#include "Entities/Player.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Settings.hpp"
#include "Entities/Character.hpp"
#include "Input/InputManager.hpp"
#include <SDL2/SDL_ttf.h>
#include <cmath>

Player::Player(int playerId, SDL_Color color)
    : playerId(playerId), mainColor(color), character(nullptr),
      settings(nullptr), posX(0), posY(0), speedX(0), speedY(0), accelX(0),
      accelY(0), direction(Direction::RIGHT), mode(PlayerMode::STAND),
      wantedMode(PlayerMode::STAND), wantedAttack(false), wantedDash(false),
      life(100), stamina(100), lastHitTime(0), knockbackX(0), stunTimer(0),
      dashTimer(0), currentAnimation(nullptr) {

  pseudo = "Player " + std::to_string(playerId);
  hitbox = {0, 0, 0, 0};
}

void Player::initialize(Character *character, int startX, int startY,
                        ResourceManager *resources, Settings *settings) {
  printf("Player::initialize start\n");
  this->character = character;
  this->settings = settings;
  this->posX = startX;
  this->posY = startY - settings->initialFall;

  loadAnimations(resources);

  life = 100.0f;
  stamina = settings->initialStamina;
  mode = PlayerMode::STAND;
  currentAnimation = &standRight;
  printf("Player::initialize end\n");
}

void Player::loadAnimations(ResourceManager *resources) {
  int playerSize = settings->playerSize;

  // Load stand animations
  auto standLFrames = resources->loadAnimationFrames(
      character->path + "1Stance/Left/", "StanceL", settings->standImages);
  auto standRFrames = resources->loadAnimationFrames(
      character->path + "1Stance/Right/", "StanceR", settings->standImages);

  standLeft.setFrames(standLFrames, character->delayStand);
  standRight.setFrames(standRFrames, character->delayStand);

  // Load run animations
  auto runLFrames = resources->loadAnimationFrames(
      character->path + "4Run/Left/", "RunL", settings->runImages);
  auto runRFrames = resources->loadAnimationFrames(
      character->path + "4Run/Right/", "RunR", settings->runImages);

  runLeft.setFrames(runLFrames, character->delayRun);
  runRight.setFrames(runRFrames, character->delayRun);

  // Load jump animations
  auto jumpLFrames = resources->loadAnimationFrames(
      character->path + "3Jump/Left/", "Jump", settings->jumpImages);
  auto jumpRFrames = resources->loadAnimationFrames(
      character->path + "3Jump/Right/", "Jump", settings->jumpImages);

  jumpLeft.setFrames(jumpLFrames, character->delayJump);
  jumpRight.setFrames(jumpRFrames, character->delayJump);

  // Load attack animations
  auto attackLFrames = resources->loadAnimationFrames(
      character->path + "Attack/Left/", "L", settings->attackImages);
  auto attackRFrames = resources->loadAnimationFrames(
      character->path + "Attack/Right/", "R", settings->attackImages);

  attackLeft.setFrames(attackLFrames, character->delayAttack);
  attackRight.setFrames(attackRFrames, character->delayAttack);
}

void Player::update(float deltaTime, int baseHeight, int screenWidth) {
  // Handle Stun
  if (stunTimer > 0) {
    stunTimer--;
    // Still apply physics (gravity/knockback) but no input/animation updates
    updatePhysics(deltaTime, baseHeight);
    return;
  }

  updateMode(baseHeight, screenWidth); // Update mode based on input and state
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

void Player::handleInput(PlayerInput &input) {
  if (stunTimer > 0)
    return; // No input when stunned
  updateControls(input);
}

void Player::updateControls(PlayerInput &input) {
  bool up = input.controls[0];
  bool left = input.controls[1];
  bool down = input.controls[2];
  bool right = input.controls[3];
  bool attack = input.controls[4];
  bool dash = input.controls[5]; // Secondary is Dash

  wantedAttack = attack;
  wantedDash = dash;

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

  // Check for Dash
  if (wantedDash && onGround && mode != PlayerMode::DASH &&
      stamina >= DASH_COST) {
    mode = PlayerMode::DASH;
    dashTimer = DASH_DURATION;
    stamina -= DASH_COST;
    // Use Run animation for dash but maybe faster?
    currentAnimation = (direction == Direction::RIGHT) ? &runRight : &runLeft;
    if (currentAnimation) {
      currentAnimation->reset();
    }
    return;
  }

  // Handle Dash State
  if (mode == PlayerMode::DASH) {
    if (dashTimer > 0) {
      dashTimer--;
    }
    if (dashTimer <= 0) {
      mode = PlayerMode::STAND; // End dash
      dashTimer = 0;
      // Reset to stand animation based on direction
      currentAnimation =
          (direction == Direction::RIGHT) ? &standRight : &standLeft;
      if (currentAnimation) {
        currentAnimation->reset();
      }
      // Prevent immediate re-dash
      wantedDash = false;
    }
    return;
  }

  // Check for attack first (highest priority if on ground)
  if (wantedAttack && canAttack() && onGround && mode != PlayerMode::ATTACK) {
    mode = PlayerMode::ATTACK;
    currentAnimation =
        (direction == Direction::RIGHT) ? &attackRight : &attackLeft;
    if (currentAnimation)
      currentAnimation->reset();
  }

  // If attacking, check if finished
  if (mode == PlayerMode::ATTACK) {
    if (currentAnimation && currentAnimation->isFinished()) {
      mode = PlayerMode::STAND;
      currentAnimation =
          (direction == Direction::RIGHT) ? &standRight : &standLeft;
      if (currentAnimation)
        currentAnimation->reset();
    }
    return; // Don't change mode while attacking
  }

  // Transition to wanted mode if on ground
  if (wantedMode == PlayerMode::STAND && mode != PlayerMode::STAND &&
      onGround) {
    mode = PlayerMode::STAND;
    currentAnimation =
        (direction == Direction::RIGHT) ? &standRight : &standLeft;
    if (currentAnimation)
      currentAnimation->reset();
  } else if (wantedMode == PlayerMode::RUN && onGround) {
    // Check if we need to switch to RUN or just update direction
    if (mode != PlayerMode::RUN) {
      mode = PlayerMode::RUN;
      currentAnimation = (direction == Direction::RIGHT) ? &runRight : &runLeft;
      if (currentAnimation)
        currentAnimation->reset();
    } else {
      // Already in RUN, check if direction changed
      Animation *targetAnim =
          (direction == Direction::RIGHT) ? &runRight : &runLeft;
      if (currentAnimation != targetAnim) {
        currentAnimation = targetAnim;
        if (currentAnimation)
          currentAnimation->reset();
      }
    }
  } else if (wantedMode == PlayerMode::JUMP && mode != PlayerMode::JUMP &&
             onGround) {
    mode = PlayerMode::JUMP;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    if (currentAnimation)
      currentAnimation->reset();
    speedY = -character->jumpSpeed;
  } else if (wantedMode == PlayerMode::SIDE_JUMP &&
             mode != PlayerMode::SIDE_JUMP && onGround) {
    mode = PlayerMode::SIDE_JUMP;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    if (currentAnimation)
      currentAnimation->reset();
    speedY = -character->jumpSpeed;
  }

  // If in air and not already in jumped state
  if (!onGround && mode != PlayerMode::JUMPED && mode != PlayerMode::JUMP &&
      mode != PlayerMode::SIDE_JUMP &&
      mode != PlayerMode::ATTACK) { // Also don't change if attacking
    mode = PlayerMode::JUMPED;
    currentAnimation = (direction == Direction::RIGHT) ? &jumpRight : &jumpLeft;
    if (currentAnimation)
      currentAnimation->reset();
  }

  // Boundary checks
  if (hitbox.x <= 0 &&
      ((wantedMode == PlayerMode::RUN && direction == Direction::LEFT) ||
       mode == PlayerMode::JUMPED || mode == PlayerMode::SIDE_JUMP ||
       mode == PlayerMode::DASH)) {
    mode = PlayerMode::STAND;
    currentAnimation = &standLeft;
    if (currentAnimation)
      currentAnimation->reset();
  }

  if (hitbox.x + hitbox.w >= screenWidth &&
      ((wantedMode == PlayerMode::RUN && direction == Direction::RIGHT) ||
       mode == PlayerMode::JUMPED || mode == PlayerMode::SIDE_JUMP ||
       mode == PlayerMode::DASH)) {
    mode = PlayerMode::STAND;
    currentAnimation = &standRight;
    if (currentAnimation)
      currentAnimation->reset();
  }
}

void Player::updatePhysics(float deltaTime, int baseHeight) {
  const float gravity = 0.6f;
  int playerSize = settings->playerSize;

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
  } else if (mode == PlayerMode::ATTACK) {
    speedX = 0; // Stop moving when attacking
  }

  // Apply gravity
  if (posY < baseHeight - playerSize || speedY < 0) {
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

  // Apply Hit Stun
  stunTimer = 10; // 10 frames of stun

  // Apply Knockback (simple: push away from facing direction of attacker?
  // We don't know attacker here.
  // Assume knockback is opposite to current direction or just backwards)
  // Ideally takeDamage should take a "sourceX" or "direction".
  // For now, push backwards relative to facing
  knockbackX = (direction == Direction::RIGHT) ? -10.0f : 10.0f;
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
