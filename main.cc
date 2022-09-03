#include "application.h"

int main(int /*argc*/, char* /*argv*/[]) {
  Application app(800, 600);
  if (!app.initialize()) {
    return 1;
  }
  app.run();
  app.shutdown();

  return 0;
}
