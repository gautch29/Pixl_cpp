#include "Entities/Animation.hpp"

Animation::Animation() : currentFrame(0), frameDelay(100), lastFrameTime(0) {}

void Animation::setFrames(const std::vector<SDL_Texture *> &frames, int delay) {
  this->frames = frames;
  this->frameDelay = delay;
  this->currentFrame = 0;
  this->lastFrameTime = SDL_GetTicks();
}

void Animation::update() {
  if (frames.empty())
    return;

  Uint32 currentTime = SDL_GetTicks();
  if (currentTime - lastFrameTime >= static_cast<Uint32>(frameDelay)) {
    currentFrame++;
    if (currentFrame >= static_cast<int>(frames.size())) {
      currentFrame = 0;
    }
    lastFrameTime = currentTime;
  }
}

void Animation::reset() {
  currentFrame = 0;
  lastFrameTime = SDL_GetTicks();
}

SDL_Texture *Animation::getCurrentFrame() const {
  if (frames.empty())
    return nullptr;
  return frames[currentFrame];
}

bool Animation::isFinished() const {
  return currentFrame >= static_cast<int>(frames.size()) - 1;
}
