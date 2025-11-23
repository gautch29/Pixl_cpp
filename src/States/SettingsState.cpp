#include "States/SettingsState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Settings.hpp"
#include "Input/InputManager.hpp"
#include "UI/Button.hpp"

SettingsState::SettingsState() : background(nullptr) {}

SettingsState::~SettingsState() {}

void SettingsState::onEnter(Game *game) {
  background = game->getResourceManager()->loadTexture(
      "assets/Backgrounds/Settings/0.png");

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();
  int ecart = 60;

  // Back to menu button
  auto menuButton =
      std::make_unique<Button>(0, screenHeight - 90, 400, 90, "Back");
  menuButton->setCallback([game]() {
    game->getSettings()->saveToFile("assets/Settings.json");
    game->popState();
  });
  buttons.push_back(std::move(menuButton));

  // Toggle hitbox button
  auto hitboxButton = std::make_unique<Button>(screenWidth * 3 / 4 - 60,
                                               screenHeight / 2 - ecart - 20,
                                               120, 40, "Hitbox");
  hitboxButton->setCallback(
      [game]() { game->getSettings()->hitbox = !game->getSettings()->hitbox; });
  buttons.push_back(std::move(hitboxButton));

  // Toggle day/night button
  auto timeButton = std::make_unique<Button>(
      screenWidth * 3 / 4 - 60, screenHeight / 2 - 20, 120, 40, "Time");
  timeButton->setCallback([game]() {
    auto settings = game->getSettings();
    settings->time = (settings->time == "day") ? "night" : "day";
  });
  buttons.push_back(std::move(timeButton));

  // Game time adjustment buttons
  auto plusMinuteButton = std::make_unique<Button>(
      screenWidth * 3 / 4 + 100, screenHeight / 2 - 2 * ecart - 20, 60, 40,
      "+60");
  plusMinuteButton->setCallback(
      [game]() { game->getSettings()->gameTime += 60; });
  buttons.push_back(std::move(plusMinuteButton));

  auto plusSecondButton =
      std::make_unique<Button>(screenWidth * 3 / 4 + 60,
                               screenHeight / 2 - 2 * ecart - 20, 40, 40, "+1");
  plusSecondButton->setCallback(
      [game]() { game->getSettings()->gameTime += 1; });
  buttons.push_back(std::move(plusSecondButton));

  auto minusMinuteButton = std::make_unique<Button>(
      screenWidth * 3 / 4 - 160, screenHeight / 2 - 2 * ecart - 20, 60, 40,
      "-60");
  minusMinuteButton->setCallback([game]() {
    if (game->getSettings()->gameTime > 60) {
      game->getSettings()->gameTime -= 60;
    }
  });
  buttons.push_back(std::move(minusMinuteButton));

  auto minusSecondButton =
      std::make_unique<Button>(screenWidth * 3 / 4 - 100,
                               screenHeight / 2 - 2 * ecart - 20, 40, 40, "-1");
  minusSecondButton->setCallback([game]() {
    if (game->getSettings()->gameTime > 1) {
      game->getSettings()->gameTime -= 1;
    }
  });
  buttons.push_back(std::move(minusSecondButton));
}

void SettingsState::handleEvent(const SDL_Event &event, Game *game) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
    game->getSettings()->saveToFile("assets/Settings.json");
    game->popState();
    return;
  }

  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    int mouseX, mouseY;
    game->getInputManager()->getMousePosition(mouseX, mouseY);

    for (auto &button : buttons) {
      if (button->contains(mouseX, mouseY)) {
        button->onClick();
        break;
      }
    }
  }
}

void SettingsState::update(float deltaTime, Game *game) {}

void SettingsState::render(SDL_Renderer *renderer, Game *game) {
  if (background) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
  }

  int mouseX, mouseY;
  game->getInputManager()->getMousePosition(mouseX, mouseY);

  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 30);

  for (auto &button : buttons) {
    bool hovered = button->contains(mouseX, mouseY);
    button->render(renderer, font, hovered);
  }

  // Render current settings values
  Settings *settings = game->getSettings();
  SDL_Color white = {255, 255, 255, 255};

  // Render Hitbox state
  std::string hitboxText = settings->hitbox ? "ON" : "OFF";
  SDL_Surface *hitboxSurface =
      TTF_RenderText_Blended(font, hitboxText.c_str(), white);
  if (hitboxSurface) {
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, hitboxSurface);
    if (texture) {
      SDL_Rect rect = {game->getWidth() * 3 / 4 + 70,
                       game->getHeight() / 2 - 60 - 20 + 10, hitboxSurface->w,
                       hitboxSurface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(hitboxSurface);
  }

  // Render Time state
  std::string timeStateText = (settings->time == "day") ? "Day" : "Night";
  SDL_Surface *timeStateSurface =
      TTF_RenderText_Blended(font, timeStateText.c_str(), white);
  if (timeStateSurface) {
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, timeStateSurface);
    if (texture) {
      SDL_Rect rect = {game->getWidth() * 3 / 4 + 70,
                       game->getHeight() / 2 - 20 + 10, timeStateSurface->w,
                       timeStateSurface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(timeStateSurface);
  }

  // Render game time
  int minutes = settings->gameTime / 60;
  int seconds = settings->gameTime % 60;
  std::string timeText = std::to_string(minutes) + ":" +
                         (seconds < 10 ? "0" : "") + std::to_string(seconds);

  SDL_Surface *surface = TTF_RenderText_Blended(font, timeText.c_str(), white);
  if (surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
      SDL_Rect rect = {game->getWidth() * 3 / 4 - 30,
                       game->getHeight() / 2 - 120 - 20, surface->w,
                       surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
  }
}
