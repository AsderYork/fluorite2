#include <systemServices/SDL2Controller/SDL2Controller.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <algorithm>

namespace fluorite {

    size_t SDL2Controller::getWindowIdentifier() {
            return windowIdentifier;
    }
    
    bool SDL2Controller::startSDLWindow()
    {
        if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) { return false; }
        SDL_Window* window = SDL_CreateWindow("Fluorite is the best gem2!",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
        if (window == NULL) { return false; }


        SDL_SysWMinfo systemInfo; 
        SDL_VERSION(&systemInfo.version);
        SDL_GetWindowWMInfo(window, &systemInfo);

        windowIdentifier = (size_t)systemInfo.info.win.window;

        SDL_SetRelativeMouseMode(SDL_TRUE);

        return true;
    }
    
    bool SDL2Controller::processInput(){
        bool continueRunning = true;
        SDL_Event event;

        bool mouseMotionNoted = false;

        while(SDL_PollEvent(&event)) {

            switch(event.type) {
                case SDL_QUIT: {continueRunning = false; break;}
                case SDL_KEYDOWN: {keyboardState[event.key.keysym.sym] = true; break;}
                case SDL_KEYUP: {keyboardState[event.key.keysym.sym] = false; break;}
                case SDL_MOUSEMOTION: {mouse.setFromMotion(event.motion); mouseMotionNoted = true; break;}
                default: {break;}
            }
        }

        if(!mouseMotionNoted) {
            mouse.resetRelativeMotion();
        }

        return continueRunning;
    }

    SDL2Controller::SDL2Controller() {
    }

    bool SDL2Controller::isKeyPressed(SDL_Keycode key) {

        auto iter = keyboardState.find(key);
        if(iter == keyboardState.end()) {
            return false;
        }
        return iter->second;
        return false;
    }

    const SDL2Controller::Mouse SDL2Controller::getMouseData() {
        return mouse;
    }


    void SDL2Controller::Mouse::setFromMotion(SDL_MouseMotionEvent motion) {
        x = motion.x;
        y = motion.y;
        xrel = motion.xrel;
        yrel = motion.yrel;
    }

    void SDL2Controller::Mouse::resetRelativeMotion() {
        xrel = 0;
        yrel = 0;
    }

}
