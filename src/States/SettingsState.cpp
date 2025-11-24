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
      "assets/Backgrounds/Settings_BG.png");

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();
  int centerX = screenWidth / 2;
  int centerY = screenHeight / 2;
  int buttonWidth = 200;
  int buttonHeight = 50;
  int spacing = 20;

  // Back to menu button
  auto menuButton = std::make_unique<Button>(50, 50, 150, 50, "BACK");
  menuButton->setCallback([game]() {
    game->getSettings()->saveToFile("assets/Settings.json");
    game->popState();
  });
  menuButton->setColors({0, 0, 0, 150}, {255, 50, 50, 50},
                        {255, 255, 255, 255});
  buttons.push_back(std::move(menuButton));

  // --- Settings Controls ---
  int startY = centerY - 100;
  int labelX = centerX - 300;
  int controlX = centerX + 50;

  // Toggle hitbox button
  auto hitboxButton = std::make_unique<Button>(controlX, startY, buttonWidth,
                                               buttonHeight, "TOGGLE");
  hitboxButton->setCallback(
      [game]() { game->getSettings()->hitbox = !game->getSettings()->hitbox; });
  hitboxButton->setColors({0, 0, 0, 150}, {0, 255, 255, 50},
                          {255, 255, 255, 255});
  buttons.push_back(std::move(hitboxButton));

  // Toggle day/night button
  auto timeButton =
      std::make_unique<Button>(controlX, startY + buttonHeight + spacing,
                               buttonWidth, buttonHeight, "SWITCH");
  timeButton->setCallback([game]() {
    auto settings = game->getSettings();
    settings->time = (settings->time == "day") ? "night" : "day";
  });
  timeButton->setColors({0, 0, 0, 150}, {255, 0, 255, 50},
                        {255, 255, 255, 255});
  buttons.push_back(std::move(timeButton));

  // Game time adjustment buttons
  int timeY = startY + (buttonHeight + spacing) * 2;

  auto minusMinuteButton =
      std::make_unique<Button>(controlX, timeY, 50, buttonHeight, "-60");
  minusMinuteButton->setCallback([game]() {
    if (game->getSettings()->gameTime > 60) {
      game->getSettings()->gameTime -= 60;
    }
  });
  minusMinuteButton->setColors({0, 0, 0, 150}, {255, 50, 50, 50},
                               {255, 255, 255, 255});
  buttons.push_back(std::move(minusMinuteButton));

  auto minusSecondButton =
      std::make_unique<Button>(controlX + 60, timeY, 40, buttonHeight, "-1");
  minusSecondButton->setCallback([game]() {
    if (game->getSettings()->gameTime > 1) {
      game->getSettings()->gameTime -= 1;
    }
  });
  minusSecondButton->setColors({0, 0, 0, 150}, {255, 50, 50, 50},
                               {255, 255, 255, 255});
  buttons.push_back(std::move(minusSecondButton));

  auto plusSecondButton =
      std::make_unique<Button>(controlX + 110, timeY, 40, buttonHeight, "+1");
  plusSecondButton->setCallback(
      [game]() { game->getSettings()->gameTime += 1; });
  plusSecondButton->setColors({0, 0, 0, 150}, {0, 255, 0, 50},
                              {255, 255, 255, 255});
  buttons.push_back(std::move(plusSecondButton));

  auto plusMinuteButton =
      std::make_unique<Button>(controlX + 160, timeY, 50, buttonHeight, "+60");
  plusMinuteButton->setCallback(
      [game]() { game->getSettings()->gameTime += 60; });
  plusMinuteButton->setColors({0, 0, 0, 150}, {0, 255, 0, 50},
                              {255, 255, 255, 255});
  buttons.push_back(std::move(plusMinuteButton));
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
  SDL_Color cyan = {0, 255, 255, 255};
  SDL_Color magenta = {255, 0, 255, 255};

  int centerX = game->getWidth() / 2;
  int centerY = game->getHeight() / 2;
  int startY = centerY - 100;
  int labelX = centerX - 300;
  int buttonHeight = 50;
  int spacing = 20;

  // Render Hitbox Label
  SDL_Surface *hitboxLabelSurf = TTF_RenderText_Blended(font, "HITBOXES", cyan);
  if (hitboxLabelSurf) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, hitboxLabelSurf);
    SDL_Rect rect = {labelX, startY + (buttonHeight - hitboxLabelSurf->h) / 2,
                     hitboxLabelSurf->w, hitboxLabelSurf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(hitboxLabelSurf);
  }

  // Render Hitbox Value
  std::string hitboxText = settings->hitbox ? "ON" : "OFF";
  SDL_Surface *hitboxValSurf =
      TTF_RenderText_Blended(font, hitboxText.c_str(), white);
  if (hitboxValSurf) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, hitboxValSurf);
    SDL_Rect rect = {centerX + 270,
                     startY + (buttonHeight - hitboxValSurf->h) / 2,
                     hitboxValSurf->w, hitboxValSurf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(hitboxValSurf);
  }

  // Render Time Label
  SDL_Surface *timeLabelSurf =
      TTF_RenderText_Blended(font, "TIME OF DAY", magenta);
  if (timeLabelSurf) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, timeLabelSurf);
    SDL_Rect rect = {labelX,
                     startY + buttonHeight + spacing +
                         (buttonHeight - timeLabelSurf->h) / 2,
                     timeLabelSurf->w, timeLabelSurf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(timeLabelSurf);
  }

  // Render Time Value
  std::string timeStateText = (settings->time == "day") ? "DAY" : "NIGHT";
  SDL_Surface *timeValSurf =
      TTF_RenderText_Blended(font, timeStateText.c_str(), white);
  if (timeValSurf) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, timeValSurf);
    SDL_Rect rect = {centerX + 270,
                     startY + buttonHeight + spacing +
                         (buttonHeight - timeValSurf->h) / 2,
                     timeValSurf->w, timeValSurf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(timeValSurf);
  }

  // Render Game Time Label
  SDL_Surface *gameTimeLabelSurf =
      TTF_RenderText_Blended(font, "ROUND TIME", white);
  if (gameTimeLabelSurf) {
    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(renderer, gameTimeLabelSurf);
    SDL_Rect rect = {labelX,
                     startY + (buttonHeight + spacing) * 2 +
                         (buttonHeight - gameTimeLabelSurf->h) / 2,
                     gameTimeLabelSurf->w, gameTimeLabelSurf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(gameTimeLabelSurf);
  }

  // Render Game Time Value
  int minutes = settings->gameTime / 60;
  int seconds = settings->gameTime % 60;
  std::string timeText = std::to_string(minutes) + ":" +
                         (seconds < 10 ? "0" : "") + std::to_string(seconds);

  SDL_Surface *gameTimeValSurf =
      TTF_RenderText_Blended(font, timeText.c_str(), white);
  if (gameTimeValSurf) {
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, gameTimeValSurf);
    if (texture) {
      SDL_Rect rect = {centerX + 270,
                       startY + (buttonHeight + spacing) * 2 +
                           (buttonHeight - gameTimeValSurf->h) / 2,
                       gameTimeValSurf->w, gameTimeValSurf->h};
      SDL_RenderCopy(renderer, texture, nullptr, &rect);
      SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(gameTimeValSurf);
  }
}
