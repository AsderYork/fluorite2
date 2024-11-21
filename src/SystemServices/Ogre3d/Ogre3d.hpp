#pragma once

#include <Ogre.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <SystemServices/SDL2Controller/SDL2Controller.hpp>

namespace fluorite
{

    class Ogre3d
    {
    private: 
    SDL2Controller* sdlController;
    public:

        bool initOgre(SDL2Controller* sdl);
        void ogreFrame(float delta);
        void ogreShutdown();

    };
    
    
}
