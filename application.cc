#include "application.h"

#include <algorithm>
#include <iostream>

Application::Application(int window_width, int window_height)
    : window_width_(window_width), window_height_(window_height) {}

bool Application::initialize() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Error when initializing SDL2! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }

  // Create window
  window_ = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, window_width_,
                             window_height_, SDL_WINDOW_SHOWN);
  if (window_ == nullptr) {
    std::cerr << "Could not create window! SDL_Error: " << SDL_GetError()
              << '\n';
    return false;
  }

  // Create renderer
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer_) {
    std::cerr << "Could not create renderer! SDL_Error: " << SDL_GetError()
              << '\n';
    SDL_DestroyWindow(window_);
    return false;
  }

  // Map is half the screen and centered
  mapRect_.w = std::min(window_width_, window_height_) / 2;
  mapRect_.h = std::min(window_width_, window_height_) / 2;
  mapRect_.x = window_width_ / 2 - mapRect_.w / 2;
  mapRect_.y = window_height_ / 2 - mapRect_.h / 2;

  // Map is half the screen and centered
  playerRect_.w = 20;
  playerRect_.h = 20;
  playerRect_.x = window_width_ / 2 - playerRect_.w / 2;
  playerRect_.y = window_height_ / 2 - playerRect_.h / 2;

  return true;
}

void Application::processKeyDownCb(SDL_Keycode key_code) {
  if (key_code == SDLK_w) {
    movKeyStates_[kMovKeyW] = true;
  } else if (key_code == SDLK_a) {
    movKeyStates_[kMovKeyA] = true;
  } else if (key_code == SDLK_s) {
    movKeyStates_[kMovKeyS] = true;
  } else if (key_code == SDLK_d) {
    movKeyStates_[kMovKeyD] = true;
  }
}

void Application::processKeyUpCb(SDL_Keycode key_code) {
  if (key_code == SDLK_ESCAPE) {
    quit_ = true;
  }
  if (key_code == SDLK_w) {
    movKeyStates_[kMovKeyW] = false;
  } else if (key_code == SDLK_a) {
    movKeyStates_[kMovKeyA] = false;
  } else if (key_code == SDLK_s) {
    movKeyStates_[kMovKeyS] = false;
  } else if (key_code == SDLK_d) {
    movKeyStates_[kMovKeyD] = false;
  }
}

void Application::run() {
  const double kFrameTimeMs = 1000. / 60;

  Uint64 lastTime = SDL_GetTicks64();
  Uint64 frame_counter = 0;
  double unprocessed_time = 0;
  int frames = 0;

  SDL_Event e;

  while (!quit_) {
    while (!quit_ && SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          quit_ = true;
          break;
        case SDL_KEYDOWN:
          processKeyDownCb(e.key.keysym.sym);
          break;
        case SDL_KEYUP:
          processKeyUpCb(e.key.keysym.sym);
          break;
      }
    }

    bool render = false;
    Uint64 curr_time = SDL_GetTicks64();
    Uint64 passed_time = curr_time - lastTime;
    unprocessed_time += static_cast<double>(passed_time);
    frame_counter += passed_time;

    lastTime = curr_time;

    // Print a somewhat approximate FPS
    if (frame_counter >= 1000) {
      std::cout << "FPS: " << frames << " | " << 1000.0 / ((double)frames)
                << " ms\n";

      frames = 0;
      frame_counter = 0;
    }

    while (unprocessed_time > kFrameTimeMs) {
      update((float)kFrameTimeMs);

      render = true;
      unprocessed_time -= kFrameTimeMs;
    }

    if (render) {
      draw();
      frames++;
    } else {
      SDL_Delay(1);
    }
  }
}

void Application::shutdown() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  SDL_Quit();
}

void Application::update(float frame_time) {
  float player_mov[2] = {0.f, 0.f};
  // Simple update
  if (movKeyStates_[kMovKeyW]) {
    player_mov[1] -= 1.f;
  }
  if (movKeyStates_[kMovKeyS]) {
    player_mov[1] += 1.f;
  }
  if (movKeyStates_[kMovKeyD]) {
    player_mov[0] += 1.f;
  }
  if (movKeyStates_[kMovKeyA]) {
    player_mov[0] -= 1.f;
  }

  // Normalize to avoid diagonal movement being faster
  float length =
      sqrt(player_mov[0] * player_mov[0] + player_mov[1] * player_mov[1]);
  if (length > 0) {
    player_mov[0] /= length;
    player_mov[1] /= length;
  }

  int new_x = playerRect_.x +
              static_cast<int>(player_mov[0] * playerSpeed_ * frame_time);
  int new_y = playerRect_.y +
              static_cast<int>(player_mov[1] * playerSpeed_ * frame_time);

  playerRect_.x =
      std::clamp(new_x, mapRect_.x, mapRect_.x + mapRect_.w - playerRect_.w);
  playerRect_.y =
      std::clamp(new_y, mapRect_.y, mapRect_.y + mapRect_.h - playerRect_.h);
}

void Application::draw() {
  // Color palette from https://colorhunt.co/palette/dddddd22283130475ef05454
  // Clean screen
  SDL_SetRenderDrawColor(renderer_, 0x22, 0x28, 0x31, 0xFF);
  SDL_RenderClear(renderer_);

  // Draw
  SDL_SetRenderDrawColor(renderer_, 0x30, 0x47, 0x5E, 0xFF);
  SDL_RenderFillRect(renderer_, &mapRect_);

  SDL_SetRenderDrawColor(renderer_, 0xF0, 0x54, 0x54, 0xFF);
  SDL_RenderFillRect(renderer_, &playerRect_);

  // Update screen
  SDL_RenderPresent(renderer_);
}
