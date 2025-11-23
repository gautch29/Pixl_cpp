#include "States/GameplayState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Settings.hpp"
#include "Entities/Character.hpp"
#include "Entities/Player.hpp"
#include "Input/InputManager.hpp"
#include "States/MenuState.hpp"
#include "States/PauseState.hpp"
#include "UI/HUD.hpp"
#include <cmath>

GameplayState::GameplayState(Character *char1, Character *char2)
    : background(nullptr), character1(char1), character2(char2),
      settings(nullptr), startTime(0), pausedTime(0), lastUpdateTime(0),
      lifeInitialized(false), finished(false) {}

GameplayState::~GameplayState() {}

void GameplayState::onEnter(Game *game) {
  settings = game->getSettings();

  // Load background based on time setting
  if (settings->time == "day") {
    background = game->getResourceManager()->loadTexture(
        "assets/Backgrounds/Game/Background 0.JPG");
  } else {
    background = game->getResourceManager()->loadTexture(
        "assets/Backgrounds/Game/Background 1.JPG");

    if (!background)
      printf("Failed to load Game background\n");
  }

  // Initialize players
  printf("Initializing players...\n");
  player1 = std::make_unique<Player>(1, (SDL_Color){255, 0, 0, 255});
  player2 = std::make_unique<Player>(2, (SDL_Color){0, 0, 255, 255});

  int groundHeight =
      game->getHeight() - 100; // Ground is 100 pixels from bottom
  int spawnY = groundHeight - settings->playerSize; // Spawn on ground

  player1->initialize(character1, game->getWidth() / 4, spawnY,
                      game->getResourceManager(), settings);
  player2->initialize(character2, game->getWidth() * 3 / 4, spawnY,
                      game->getResourceManager(), settings);
  printf("Players initialized\n");

  startTime = SDL_GetTicks();
  pausedTime = 0;
  lastUpdateTime = SDL_GetTicks();
  lifeInitialized = false;
  finished = false;

  // Player 2 attacking Player 1
  if (player2->getWantedAttack() && player2->canAttack() &&
      player2->getLife() > 0 && player1->getLife() > 0) {
    SDL_Rect p1Hitbox = player1->getHitbox();
    SDL_Rect p2Hitbox = player2->getHitbox();

    SDL_Point p1Pos = player1->getPosition();
    SDL_Point p2Pos = player2->getPosition();

    int distance = std::abs(p1Pos.x - p2Pos.x);
    int playerSize = settings->playerSize;

    bool p2FacingP1 =
        (player2->getDirection() == Direction::RIGHT && p2Pos.x < p1Pos.x) ||
        (player2->getDirection() == Direction::LEFT && p2Pos.x > p1Pos.x);

    bool verticallyAligned =
        !(p1Hitbox.y + p1Hitbox.h < p2Hitbox.y + p2Hitbox.h / 2 ||
          p1Hitbox.y > p2Hitbox.y + p2Hitbox.h - p2Hitbox.h / 2);

    if (p2FacingP1 && distance <= playerSize * 0.8f && verticallyAligned) {
      player1->takeDamage(character2->damages);
      player2->performAttack();
    }
  }
}

void GameplayState::onExit(Game *game) {}

void GameplayState::handleEvent(const SDL_Event &event, Game *game) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    if (!finished) {
      game->pushState(std::make_unique<PauseState>());
    } else {
      game->changeState(std::make_unique<MenuState>());
    }
  }
}

void GameplayState::update(float deltaTime, Game *game) {
  Uint32 currentTime = SDL_GetTicks();
  // If more than 100ms passed since last update, assume we were paused
  if (currentTime - lastUpdateTime > 100) {
    pausedTime += (currentTime - lastUpdateTime);
  }
  lastUpdateTime = currentTime;

  if (finished)
    return;

  // Update Shake
  if (shakeTimer > 0) {
    shakeTimer -= deltaTime;
    if (shakeTimer < 0)
      shakeTimer = 0;
  }

  // Update Input
  game->getInputManager()->update();
  player1->handleInput(game->getInputManager()->getPlayer1Input());
  player2->handleInput(game->getInputManager()->getPlayer2Input());

  // Update Players
  int groundHeight = game->getHeight() - 100;
  player1->update(deltaTime, groundHeight, game->getWidth());
  player2->update(deltaTime, groundHeight, game->getWidth());

  // Check Collisions
  checkCollisions();

  // Check Win Condition
  checkWinCondition(game);
}

void GameplayState::checkCollisions() {
  // Player 1 attacking Player 2
  if (player1->getWantedAttack() && player1->canAttack() &&
      player1->getLife() > 0 && player2->getLife() > 0) {
    SDL_Rect p1Hitbox = player1->getHitbox();
    SDL_Rect p2Hitbox = player2->getHitbox();

    // Check if players are close enough and facing each other
    SDL_Point p1Pos = player1->getPosition();
    SDL_Point p2Pos = player2->getPosition();

    int distance = std::abs(p1Pos.x - p2Pos.x);
    int playerSize = settings->playerSize;

    bool p1FacingP2 =
        (player1->getDirection() == Direction::RIGHT && p1Pos.x < p2Pos.x) ||
        (player1->getDirection() == Direction::LEFT && p1Pos.x > p2Pos.x);

    // Check vertical alignment
    bool verticallyAligned =
        !(p2Hitbox.y + p2Hitbox.h < p1Hitbox.y + p1Hitbox.h / 2 ||
          p2Hitbox.y > p1Hitbox.y + p1Hitbox.h - p1Hitbox.h / 2);

    if (p1FacingP2 && distance <= playerSize * 0.8f && verticallyAligned) {
      player2->takeDamage(character1->damages);
      player1->performAttack();

      // Trigger Shake
      shakeTimer = 0.2f;
      shakeMagnitude = 5.0f;
    }
  }

  // Player 2 attacking Player 1
  if (player2->getWantedAttack() && player2->canAttack() &&
      player2->getLife() > 0 && player1->getLife() > 0) {
    SDL_Rect p2Hitbox = player2->getHitbox();
    SDL_Rect p1Hitbox = player1->getHitbox();

    SDL_Point p2Pos = player2->getPosition();
    SDL_Point p1Pos = player1->getPosition();

    int distance = std::abs(p2Pos.x - p1Pos.x);
    int playerSize = settings->playerSize;

    bool p2FacingP1 =
        (player2->getDirection() == Direction::RIGHT && p2Pos.x < p1Pos.x) ||
        (player2->getDirection() == Direction::LEFT && p2Pos.x > p1Pos.x);

    bool verticallyAligned =
        !(p1Hitbox.y + p1Hitbox.h < p2Hitbox.y + p2Hitbox.h / 2 ||
          p1Hitbox.y > p2Hitbox.y + p2Hitbox.h - p2Hitbox.h / 2);

    if (p2FacingP1 && distance <= playerSize * 0.8f && verticallyAligned) {
      player1->takeDamage(character2->damages);
      player2->performAttack();

      // Trigger Shake
      shakeTimer = 0.2f;
      shakeMagnitude = 5.0f;
    }
  }
}

void GameplayState::checkWinCondition(Game *game) {
  Settings *settings = game->getSettings();
  Uint32 currentTime = SDL_GetTicks();
  Uint32 elapsedTime = (currentTime - startTime - pausedTime) / 1000;

  if (player1->getLife() <= 0 || player2->getLife() <= 0 ||
      elapsedTime >= static_cast<Uint32>(settings->gameTime)) {
    finished = true;
  }
}

void GameplayState::render(SDL_Renderer *renderer, Game *game) {
  // Render background
  if (background) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
  }

  // Render players
  player1->render(renderer);
  player2->render(renderer);

  // Render HUD
  Settings *settings = game->getSettings();
  Uint32 currentTime = SDL_GetTicks();
  Uint32 elapsedTime = (currentTime - startTime - pausedTime) / 1000;
  int timeRemaining = settings->gameTime - elapsedTime;
  if (timeRemaining < 0)
    timeRemaining = 0;

  HUD hud;
  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 50);

  if (!font) {
    printf("ERROR: Failed to load font assets/Fonts/Alien.ttf\n");
  } else {
    hud.render(renderer, font, player1.get(), player2.get(), timeRemaining,
               game->getWidth(), game->getHeight());
  }

  // Render "FIGHT!" at the start
  if (lifeInitialized && elapsedTime < 2) {
    TTF_Font *winFont =
        game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 200);
    SDL_Color red = {255, 0, 0, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(winFont, "Fight!", red);
    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_Rect rect = {game->getWidth() / 2 - surface->w / 2,
                       game->getHeight() / 2 - surface->h / 2, surface->w,
                       surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
    }
  }

  // Render win message
  if (finished) {
    TTF_Font *koFont =
        game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 100);
    std::string message;
    SDL_Color color = {255, 255, 255, 255};

    if (player1->getLife() <= 0 && player2->getLife() > 0) {
      message = "PLAYER 2 WINS!";
      color = {0, 0, 255, 255};
    } else if (player2->getLife() <= 0 && player1->getLife() > 0) {
      message = "PLAYER 1 WINS!";
      color = {255, 0, 0, 255};
    } else if (player1->getLife() <= 0 && player2->getLife() <= 0) {
      message = "DRAW";
    } else {
      message = "TIME'S OVER";
    }

    SDL_Surface *surface =
        TTF_RenderText_Blended(koFont, message.c_str(), color);
    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_Rect rect = {game->getWidth() / 2 - surface->w / 2,
                       game->getHeight() / 4 - surface->h / 2, surface->w,
                       surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
    }
  }
}
