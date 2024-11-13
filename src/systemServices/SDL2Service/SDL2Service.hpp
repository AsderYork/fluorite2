#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace fluorite
{
    /**
     * Creates and manages an SDL2 Window
     */
    class SDL2Service
    {
        bool isInited = false;
        SDL_Window* windowPtr = nullptr;
    public:
        SDL2Service();
        void init(std::string name, int width, int height);

        bool frame();


        ~SDL2Service();
    };  

}