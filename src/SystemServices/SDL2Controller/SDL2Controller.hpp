#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <map>

namespace fluorite {

    class SDL2Controller
    {

    public:
        struct Mouse {
            int32_t x;
            int32_t y;
            int32_t xrel;
            int32_t yrel;

            void setFromMotion(SDL_MouseMotionEvent motion);
            void resetRelativeMotion();

        };

    private:
        size_t windowIdentifier;
        std::map<SDL_Keycode, bool> keyboardState;
        Mouse mouse;

    public:


        SDL2Controller();

        size_t getWindowIdentifier();
        bool startSDLWindow();
        bool processInput();

        bool isKeyPressed(SDL_Keycode key);
        const Mouse getMouseData();

        

    };

    
}