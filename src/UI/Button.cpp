#include "UI/Button.hpp"
#include <SDL2/SDL_ttf.h>

Button::Button(int x, int y, int width, int height, const std::string &text)
    : text(text), callback(nullptr) {
  rect = {x, y, width, height};
  normalColor = {255, 0, 0, 255};
  hoverColor = {127, 0, 0, 255};
  textColor = {255, 255, 255, 255};
}

void Button::render(SDL_Renderer *renderer, TTF_Font *font, bool hovered) {
  if (useTexture && normalTexture) {
    // Render texture
    SDL_Texture *tex = (hovered && hoverTexture) ? hoverTexture : normalTexture;
    SDL_RenderCopy(renderer, tex, nullptr, &rect);
  } else {
    // Draw button background
    SDL_Color bgColor = hovered ? hoverColor : normalColor;
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b,
                           bgColor.a);
    SDL_RenderFillRect(renderer, &rect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
  }

  // Render text
  if (font && !text.empty()) {
    SDL_Surface *surface =
        TTF_RenderText_Blended(font, text.c_str(), textColor);
    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      if (texture) {
        int textW = surface->w;
        int textH = surface->h;

        int textX = rect.x + (rect.w - textW) / 2;
        if (!centerText) {
          textX = rect.x + 20; // Left padding
        }

        SDL_Rect textRect = {textX, rect.y + (rect.h - textH) / 2, textW,
                             textH};
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
      }
      SDL_FreeSurface(surface);
    }
  }
}

void Button::setTextures(SDL_Texture *normal, SDL_Texture *hover) {
  normalTexture = normal;
  hoverTexture = hover;
  useTexture = true;
}

void Button::setColors(SDL_Color normal, SDL_Color hover, SDL_Color text) {
  normalColor = normal;
  hoverColor = hover;
  textColor = text;
  useTexture = false;
}

bool Button::contains(int mouseX, int mouseY) const {
  return mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y &&
         mouseY <= rect.y + rect.h;
}

void Button::setCallback(std::function<void()> callback) {
  this->callback = callback;
}

void Button::onClick() {
  if (callback) {
    callback();
  }
}

void Button::setPosition(int x, int y) {
  rect.x = x;
  rect.y = y;
}

void Button::setSize(int width, int height) {
  rect.w = width;
  rect.h = height;
}
