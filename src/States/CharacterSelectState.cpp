#include "States/CharacterSelectState.hpp"
#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Entities/Character.hpp"
#include "Input/InputManager.hpp"
#include "States/GameplayState.hpp"
#include "UI/Button.hpp"
#include <memory>

CharacterSelectState::CharacterSelectState()
    : background(nullptr), player1Character(nullptr),
      player2Character(nullptr) {}

CharacterSelectState::~CharacterSelectState() {
  for (auto *character : availableCharacters) {
    delete character;
  }
}

void CharacterSelectState::onEnter(Game *game) {
  background = game->getResourceManager()->loadTexture(
      "assets/Backgrounds/Settings/0.png");

  // Load available characters
  Character *deku = new Character();
  if (deku->loadFromFile("assets/Characters/Deku/")) {
    deku->profilePicture = game->getResourceManager()->loadTexture(
        "assets/Characters/Deku/Deku Head.png");
    availableCharacters.push_back(deku);
  }

  Character *duke = new Character();
  if (duke->loadFromFile("assets/Characters/Slash/")) {
    duke->profilePicture = game->getResourceManager()->loadTexture(
        "assets/Characters/Slash/Slash Head.png");
    availableCharacters.push_back(duke);
  }

  // Set default selections
  if (!availableCharacters.empty()) {
    player1Character = availableCharacters[0];
    player2Character = availableCharacters[0];
  }

  int screenWidth = game->getWidth();
  int screenHeight = game->getHeight();
  int profileSize = 100;

  // Back button
  auto backButton =
      std::make_unique<Button>(0, screenHeight - 90, 400, 90, "Back");
  backButton->setCallback([game]() { game->popState(); });
  buttons.push_back(std::move(backButton));

  // Start game button
  auto startButton = std::make_unique<Button>(
      screenWidth / 2 - 100, screenHeight - 90, 200, 90, "Fight!");
  startButton->setCallback([this, game]() {
    if (player1Character && player2Character) {
      game->changeState(
          std::make_unique<GameplayState>(player1Character, player2Character));
    }
  });
  buttons.push_back(std::move(startButton));

  // Character selection buttons for player 1
  for (size_t i = 0; i < availableCharacters.size(); i++) {
    int x = screenWidth / 8 + i * (profileSize + 20);
    int y = screenHeight / 4 + screenHeight / 10;

    auto button = std::make_unique<Button>(x, y, profileSize, profileSize, "");
    Character *character = availableCharacters[i];
    button->setCallback([this, character]() { player1Character = character; });
    buttons.push_back(std::move(button));
  }

  // Character selection buttons for player 2
  for (size_t i = 0; i < availableCharacters.size(); i++) {
    int x = screenWidth / 8 + screenWidth / 2 + i * (profileSize + 20);
    int y = screenHeight / 4 + screenHeight / 10;

    auto button = std::make_unique<Button>(x, y, profileSize, profileSize, "");
    Character *character = availableCharacters[i];
    button->setCallback([this, character]() { player2Character = character; });
    buttons.push_back(std::move(button));
  }
}

void CharacterSelectState::handleEvent(const SDL_Event &event, Game *game) {
  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
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

void CharacterSelectState::update(float deltaTime, Game *game) {}

void CharacterSelectState::render(SDL_Renderer *renderer, Game *game) {
  if (background) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
  }

  TTF_Font *font =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 50);
  SDL_Color white = {255, 255, 255, 255};

  // Render "Player 1" and "Player 2" titles
  SDL_Surface *p1Surface = TTF_RenderText_Blended(font, "Player 1", white);
  if (p1Surface) {
    SDL_Texture *p1Texture = SDL_CreateTextureFromSurface(renderer, p1Surface);
    SDL_Rect p1Rect = {game->getWidth() / 4 - p1Surface->w / 2,
                       game->getHeight() / 4, p1Surface->w, p1Surface->h};
    SDL_RenderCopy(renderer, p1Texture, nullptr, &p1Rect);
    SDL_DestroyTexture(p1Texture);
    SDL_FreeSurface(p1Surface);
  }

  SDL_Surface *p2Surface = TTF_RenderText_Blended(font, "Player 2", white);
  if (p2Surface) {
    SDL_Texture *p2Texture = SDL_CreateTextureFromSurface(renderer, p2Surface);
    SDL_Rect p2Rect = {game->getWidth() * 3 / 4 - p2Surface->w / 2,
                       game->getHeight() / 4, p2Surface->w, p2Surface->h};
    SDL_RenderCopy(renderer, p2Texture, nullptr, &p2Rect);
    SDL_DestroyTexture(p2Texture);
    SDL_FreeSurface(p2Surface);
  }

  // Render buttons first (so portraits are on top)
  int mouseX, mouseY;
  game->getInputManager()->getMousePosition(mouseX, mouseY);

  TTF_Font *buttonFont =
      game->getResourceManager()->loadFont("assets/Fonts/Alien.ttf", 32);
  for (auto &button : buttons) {
    bool hovered = button->contains(mouseX, mouseY);
    button->render(renderer, buttonFont, hovered);
  }

  // Render character portraits
  int profileSize = 100;
  for (size_t i = 0; i < availableCharacters.size(); i++) {
    Character *character = availableCharacters[i];
    if (character->profilePicture) {
      // Player 1 portraits
      SDL_Rect p1Rect = {game->getWidth() / 8 +
                             static_cast<int>(i) * (profileSize + 20),
                         game->getHeight() / 4 + game->getHeight() / 10,
                         profileSize, profileSize};
      SDL_RenderCopy(renderer, character->profilePicture, nullptr, &p1Rect);

      // Highlight selected P1 character
      if (player1Character == character) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green border
        SDL_Rect border = {p1Rect.x - 2, p1Rect.y - 2, p1Rect.w + 4,
                           p1Rect.h + 4};
        SDL_RenderDrawRect(renderer, &border);
      }

      // Player 2 portraits
      SDL_Rect p2Rect = {game->getWidth() / 8 + game->getWidth() / 2 +
                             static_cast<int>(i) * (profileSize + 20),
                         game->getHeight() / 4 + game->getHeight() / 10,
                         profileSize, profileSize};
      SDL_RenderCopy(renderer, character->profilePicture, nullptr, &p2Rect);

      // Highlight selected P2 character
      if (player2Character == character) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green border
        SDL_Rect border = {p2Rect.x - 2, p2Rect.y - 2, p2Rect.w + 4,
                           p2Rect.h + 4};
        SDL_RenderDrawRect(renderer, &border);
      }
    }
  }
}
