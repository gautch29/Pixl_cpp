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
};
