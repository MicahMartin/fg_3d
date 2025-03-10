#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_joystick.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_timer.h"
#include "input/VirtualController.h"
#include <cstdio>

constexpr int cScreenWidth{640};
constexpr int cScreenHeight{ 480 };

bool init();
void close();
void openGamePad(SDL_JoystickID which);

int constructSdlKeyboardInput();
int constructSdlPadInput();

SDL_Window* gWindow{ nullptr };
SDL_Surface* gScreenSurface{ nullptr };
SDL_Gamepad* gamePad{ nullptr };

int main (int argc, char *argv[]) {
  int frameCount{0};

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
      Uint32 frameStart = SDL_GetTicks();  // Record frame start time
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) quit = true;
        if (e.type == SDL_EVENT_JOYSTICK_ADDED) openGamePad(e.jdevice.which);
      }

      vc.update(constructSdlPadInput());
      vc.printHistory();
      if (vc.wasPressed(Input::LIGHT_P, true, false, 0)) {
        printf("lightP was pressed\n");
        quit = true;
      }

      SDL_FillSurfaceRect(gScreenSurface, nullptr, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
      SDL_UpdateWindowSurface(gWindow);
      // Calculate how long the frame took
      Uint32 frameTime = SDL_GetTicks() - frameStart;
      // If the frame finished early, delay to cap at 60 FPS (approx 16ms per frame)
      if (frameTime < 16) {
        SDL_Delay(16 - frameTime);
      }

      frameCount++;
    }
  }

  close();
  return exitCode;
}


bool init(){
  bool success{ true };

  if( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)){
    SDL_Log("SDL could not initialize subsystems! SDL error: %s\n", SDL_GetError());
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

void openGamePad(SDL_JoystickID which){
  if (gamePad == nullptr) {  /* we don't have a stick yet and one was added, open it! */
   gamePad = SDL_OpenGamepad(which);
   if (!gamePad) {
     SDL_Log("Failed to open joystick ID %u: %s", (unsigned int) which, SDL_GetError());
   }
  } 
}

int constructSdlKeyboardInput(){
  int retVal{ 0 };
  const bool* keyStates = SDL_GetKeyboardState(nullptr);
  if (keyStates[SDL_SCANCODE_A]) {
    retVal |= Input::LIGHT_P;
  }
  return retVal;
}

int constructSdlPadInput(){
  int retVal{0};
  if (gamePad) {
    // Map gamepad buttons to VirtualController input bitfield
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_SOUTH))  retVal |= Input::LIGHT_P;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_EAST))   retVal |= Input::LIGHT_K;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_WEST))   retVal |= Input::MEDIUM_P;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_NORTH))  retVal |= Input::MEDIUM_K;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER))  retVal |= Input::HEAVY_P;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) retVal |= Input::HEAVY_K;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_START))  retVal |= Input::START;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_BACK))   retVal |= Input::SELECT;

    // D-pad directions
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_DPAD_UP))    retVal |= Input::UP;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_DPAD_DOWN))  retVal |= Input::DOWN;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_DPAD_LEFT))  retVal |= Input::LEFT;
    if (SDL_GetGamepadButton(gamePad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) retVal |= Input::RIGHT;
  }
  return retVal;
}
