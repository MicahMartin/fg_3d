#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "input/VirtualController.h"
#include <cstdio>

constexpr int cScreenWidth{640};
constexpr int cScreenHeight{ 480 };
int frameCount{0};


bool init();
void close();
int constructInput();

SDL_Window* gWindow{ nullptr };
SDL_Surface* gScreenSurface{ nullptr };

int main (int argc, char *argv[]) {
  VirtualController vc;
  int exitCode{ 0 };

  if(!init()){
    SDL_Log("Unable to initialize!\n");
    exitCode = 1;
  } else {
    bool quit{ false };
    SDL_Event e;
    SDL_zero(e);

    while (quit == false) {
      frameCount++;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) quit = true;
      }

      vc.update(constructInput());
      printf("%d\n", frameCount);
      if (vc.wasPressed(Input::LIGHT_P, true, 0, false)) {
        printf("lightP was pressed\n");
        quit = true;
      }

      SDL_FillSurfaceRect(gScreenSurface, nullptr, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
      SDL_UpdateWindowSurface(gWindow);
    }
  }

  close();
  return exitCode;
}


bool init(){
  bool success{ true };

  if( !SDL_Init(SDL_INIT_VIDEO)){
    SDL_Log("SDL could not initialize video subsystem! SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (gWindow = SDL_CreateWindow("MyWindow", cScreenWidth, cScreenHeight, 0); gWindow == nullptr) {
      SDL_Log("Could not create window! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }

  return success;
}

void close(){
  SDL_Log("Goodbye!\n");
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;
  gScreenSurface = nullptr;
  SDL_Quit();
}

int constructInput(){
  int retVal{ 0 };
  const bool* keyStates = SDL_GetKeyboardState(nullptr);
  if (keyStates[SDL_SCANCODE_A]) {
    retVal |= Input::LIGHT_P;
  }
  return retVal;
}
