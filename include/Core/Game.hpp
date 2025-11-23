#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <stack>

class GameState;
class ResourceManager;
class InputManager;
class Settings;

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void cleanup();

    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);

    SDL_Renderer* getRenderer() const { return renderer; }
    ResourceManager* getResourceManager() const { return resourceManager.get(); }
    InputManager* getInputManager() const { return inputManager.get(); }
    Settings* getSettings() const { return settings.get(); }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void quit() { running = false; }

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    
    std::stack<std::unique_ptr<GameState>> states;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<Settings> settings;
    
    bool running;
    int width;
    int height;
    const int TARGET_FPS = 60;
};
