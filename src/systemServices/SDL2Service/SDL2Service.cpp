#include <systemServices/SDL2Service/SDL2Service.hpp>
#include <iostream>

namespace fluorite
{

    SDL2Service::SDL2Service() { }

    
    void SDL2Service::init(std::string name, int width, int height) {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) { throw std::runtime_error(SDL_GetError()); }
        isInited = true;

        #if defined linux && SDL_VERSION_ATLEAST(2, 0, 8) // Disable compositor bypass
            if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) { throw std::runtime_error("SDL can not disable compositor bypass!"); }
        #endif

        // Create window
        windowPtr = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    }
    
    SDL2Service::~SDL2Service()
    {
        if(windowPtr != nullptr) {
            SDL_DestroyWindow(windowPtr);
        }
        if(isInited) {
            SDL_Quit();
        }
    }


    bool SDL2Service::frame() {
        bool continueRunning = true;
        SDL_Event event;

        bool mouseMotionNoted = false;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {continueRunning = false; break;}
                default: {break;}
            }
        }

        return continueRunning;

    }    


}


