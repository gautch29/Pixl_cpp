#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Animation {
public:
  Animation();

  void setFrames(const std::vector<SDL_Texture *> &frames, int delay);
  void update();
  void reset();

  SDL_Texture *getCurrentFrame() const;
  bool isFinished() const;

private:
  std::vector<SDL_Texture *> frames;
  int currentFrame;
  int frameDelay;
  Uint32 lastFrameTime;
};
