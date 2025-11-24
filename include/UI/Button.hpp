#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <functional>
#include <string>

class Button {
public:
  Button(int x, int y, int width, int height, const std::string &text);

  void render(SDL_Renderer *renderer, TTF_Font *font, bool hovered = false);
  bool contains(int mouseX, int mouseY) const;

  void setCallback(std::function<void()> callback);
  void onClick();

  void setPosition(int x, int y);
  void setSize(int width, int height);

  SDL_Rect getRect() const { return rect; }

private:
  SDL_Rect rect;
  std::string text;
  std::function<void()> callback;

  SDL_Color normalColor;
  SDL_Color hoverColor;
  SDL_Color textColor;

  // New visual properties
  SDL_Texture *normalTexture = nullptr;
  SDL_Texture *hoverTexture = nullptr;
  bool useTexture = false;
  bool centerText = true;
  float currentScale = 1.0f;

public:
  void setTextures(SDL_Texture *normal, SDL_Texture *hover);
  void setColors(SDL_Color normal, SDL_Color hover, SDL_Color text);
  void setAlignment(bool center) { centerText = center; }
};
