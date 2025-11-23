#include "Core/ResourceManager.hpp"
#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer *renderer) : renderer(renderer) {}

ResourceManager::~ResourceManager() { clear(); }

SDL_Texture *ResourceManager::loadTexture(const std::string &path) {
  // Check if already loaded
  auto it = textures.find(path);
  if (it != textures.end()) {
    return it->second;
  }

  // Load new texture
  SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
  if (!texture) {
    std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError()
              << std::endl;
    return nullptr;
  }

  textures[path] = texture;
  return texture;
}

TTF_Font *ResourceManager::loadFont(const std::string &path, int size) {
  std::string key = path + "_" + std::to_string(size);

  auto it = fonts.find(key);
  if (it != fonts.end()) {
    return it->second;
  }

  TTF_Font *font = TTF_OpenFont(path.c_str(), size);
  if (!font) {
    std::cerr << "Failed to load font: " << path << " - " << TTF_GetError()
              << std::endl;
    return nullptr;
  }

  fonts[key] = font;
  return font;
}

std::vector<SDL_Texture *>
ResourceManager::loadAnimationFrames(const std::string &basePath,
                                     const std::string &prefix, int count) {

  std::vector<SDL_Texture *> frames;
  for (int i = 1; i <= count; i++) {
    std::string path = basePath + prefix + std::to_string(i) + ".png";
    SDL_Texture *texture = loadTexture(path);
    if (texture) {
      frames.push_back(texture);
    }
  }
  return frames;
}

void ResourceManager::clear() {
  for (auto &pair : textures) {
    SDL_DestroyTexture(pair.second);
  }
  textures.clear();

  for (auto &pair : fonts) {
    TTF_CloseFont(pair.second);
  }
  fonts.clear();
}
