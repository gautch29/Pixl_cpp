#include "Core/Game.hpp"
#include "Core/ResourceManager.hpp"
#include "Core/Settings.hpp"
#include "Input/InputManager.hpp"
#include "States/GameState.hpp"
#include <iostream>

Game::Game()
    : window(nullptr), renderer(nullptr), running(false), width(1920),
      height(1080) {}

Game::~Game() {}

bool Game::initialize() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return false;
  }

  // Initialize SDL_image
  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    std::cerr << "SDL_image initialization failed: " << IMG_GetError()
              << std::endl;
    return false;
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf initialization failed: " << TTF_GetError()
              << std::endl;
    return false;
  }

  // Create window in fullscreen
  window = SDL_CreateWindow("PixL Fighter", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height,
                            SDL_WINDOW_FULLSCREEN_DESKTOP);

  if (!window) {
    std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    return false;
  }

  // Get actual window size (important for fullscreen)
  SDL_GetWindowSize(window, &width, &height);

  // Create renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
    return false;
  }

  // Initialize subsystems
  resourceManager = std::make_unique<ResourceManager>(renderer);
  inputManager = std::make_unique<InputManager>();
  settings = std::make_unique<Settings>();

  // Load settings
  if (!settings->loadFromFile("assets/Settings.txt")) {
    std::cerr << "Warning: Could not load settings, using defaults"
              << std::endl;
  }

  // Load input configuration
  if (!inputManager->loadInputConfig("assets/Inputs.txt")) {
    std::cerr << "Warning: Could not load input config, using defaults"
              << std::endl;
  }

  running = true;
  return true;
}

void Game::run() {
  const int FRAME_DELAY = 1000 / TARGET_FPS;
  Uint32 frameStart;
  int frameTime;

  while (running && !states.empty()) {
    frameStart = SDL_GetTicks();

    handleEvents();
    update(1.0f / TARGET_FPS);
    render();

    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frameTime);
    }
  }
}

void Game::cleanup() {
  while (!states.empty()) {
    states.pop();
  }

  resourceManager.reset();
  inputManager.reset();
  settings.reset();

  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }

  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void Game::handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      running = false;
    }

    inputManager->handleEvent(event);

    if (!states.empty()) {
      states.top()->handleEvent(event, this);
    }
  }

  inputManager->update();
}

void Game::update(float deltaTime) {
  if (!states.empty()) {
    states.top()->update(deltaTime, this);
  }
}

void Game::render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if (!states.empty()) {
    states.top()->render(renderer, this);
  }

  SDL_RenderPresent(renderer);
}

void Game::pushState(std::unique_ptr<GameState> state) {
  if (state) {
    state->onEnter(this);
    states.push(std::move(state));
  }
}

void Game::popState() {
  if (!states.empty()) {
    states.top()->onExit(this);
    states.pop();
  }
}

void Game::changeState(std::unique_ptr<GameState> state) {
  popState();
  pushState(std::move(state));
}
