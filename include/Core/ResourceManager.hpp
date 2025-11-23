#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ResourceManager {
public:
  ResourceManager(SDL_Renderer *renderer);
  ~ResourceManager();

  SDL_Texture *loadTexture(const std::string &path);
  TTF_Font *loadFont(const std::string &path, int size);

  // Load animation frames
  std::vector<SDL_Texture *> loadAnimationFrames(const std::string &basePath,
                                                 const std::string &prefix,
                                                 int count);

  void clear();

private:
  SDL_Renderer *renderer;
  std::map<std::string, SDL_Texture *> textures;
  std::map<std::string, TTF_Font *> fonts;
};
