#pragma once

#include <Ogre.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <SystemServices/SDL2Controller/SDL2Controller.hpp>
#include <SystemServices/Ogre3d/Ogre3dCameraControl/Ogre3dCameraControl.hpp>
#include <memory>

namespace fluorite
{

    /**
     * Represents a graphical objects. GrapgicsObjects can be created via Ogre3D. They are controlled mostly through
     * interactions with Ogre3d
     */
    class GraphicsObject {
    private:
        Ogre::SceneNode* sceneNode;

        GraphicsObject(Ogre::SceneNode* _sceneNode) : sceneNode(_sceneNode) {}
        friend class Ogre3d;

    public:

        void destroy() {
            sceneNode->destroyAllChildrenAndObjects();
        }
    };


    /**
     * Initializes Ogre3D and allows for creation and management of GraphicsObjects and cameras
     */
    class Ogre3d
    {
    private: 
        SDL2Controller* sdlController;
        std::unique_ptr<Ogre3dCameraControll> mainCamera;
    public:

        bool initOgre(SDL2Controller* sdl);
        void ogreFrame(float delta);
        void ogreShutdown();

        Ogre3dCameraControll* getCamera() const;

        GraphicsObject testCube(float x, float y, float z, float size, Ogre::ColourValue color = Ogre::ColourValue::White);

    };
    
    
}
