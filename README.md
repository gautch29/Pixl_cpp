# PixL Fighter - C++ Edition

A 2D fighting game rewritten in modern C++ with SDL2.

## Features
- 2-player local multiplayer
- Multiple characters with unique stats
- Combat system with health and stamina
- Character selection
- Configurable settings
- Animated sprites

## Building

### Prerequisites

#### macOS
```bash
brew install sdl2 sdl2_image sdl2_ttf cmake
```

#### Windows
Download and install:
- CMake: https://cmake.org/download/
- SDL2 development libraries: https://www.libsdl.org/download-2.0.php
- SDL2_image: https://www.libsdl.org/projects/SDL_image/
- SDL2_ttf: https://www.libsdl.org/projects/SDL_ttf/

### Compile

#### macOS/Linux
```bash
mkdir build
cd build
cmake ..
make
./PixL
```

#### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
Release\PixL.exe
```

## Controls

### Player 1
- Movement: Arrow keys or WASD
- Attack: Configurable (see Inputs.txt)

### Player 2
- Movement: Configurable (see Inputs.txt)
- Attack: Configurable (see Inputs.txt)

## Configuration

Edit `assets/Settings.txt` to configure:
- Game time
- Hitbox display
- Player size
- Animation frame counts

Edit `assets/Inputs.txt` to configure controls for both players.

## License

Original game by Gauthier Baron
