#ifndef APPLICATION_H
#define APPLICATION_H

#define SDL_MAIN_HANDLED
#include <SDL.h>
//#include <SDL_Keycode.h>

class Application {
 public:
  Application() = delete;
  Application(int window_width, int window_height);

  bool initialize();
  void run();
  void shutdown();

 private:
  void update(float frame_time);
  void draw();

  void processKeyDownCb(SDL_Keycode key_code);
  void processKeyUpCb(SDL_Keycode key_code);

 private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  int window_width_;
  int window_height_;

  bool quit_ = false;

  SDL_Rect mapRect_;
  SDL_Rect playerRect_;

  // To know if W A S D is being pressed
  static constexpr int kMovKeyW = 0;
  static constexpr int kMovKeyA = 1;
  static constexpr int kMovKeyS = 2;
  static constexpr int kMovKeyD = 3;
  char movKeyStates_[4] = {false, false, false, false};

  float playerSpeed_ = .1f;
};

#endif  // APPLICATION_H