cmake_minimum_required(VERSION 3.15)
project(PixL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use pkg-config to find SDL2 libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/Core/Game.cpp
    src/Core/ResourceManager.cpp
    src/Core/Settings.cpp
    src/Input/InputManager.cpp
    src/Entities/Character.cpp
    src/Entities/Player.cpp
    src/Entities/Animation.cpp
    src/States/MenuState.cpp
    src/States/GameplayState.cpp
    src/States/SettingsState.cpp
    src/States/CharacterSelectState.cpp
    src/States/PauseState.cpp
    src/UI/Button.cpp
    src/UI/HUD.cpp
)

# Create executable
add_executable(PixL ${SOURCES})

# Link libraries
target_link_libraries(PixL
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)

# Add library directories
target_link_directories(PixL PRIVATE
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_IMAGE_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
)

# Copy assets to build directory
add_custom_command(TARGET PixL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PixL>/assets
)

# Platform-specific settings
if(APPLE)
    target_compile_options(PixL PRIVATE -Wall -Wextra)
elseif(WIN32)
    target_compile_options(PixL PRIVATE /W4)
    # For Windows, we might need to copy SDL2 DLLs
endif()

# Install target
install(TARGETS PixL DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)
