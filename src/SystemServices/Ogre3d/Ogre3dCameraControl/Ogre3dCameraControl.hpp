#pragma once

#include <Ogre.h>
#include <SystemServices/SDL2Controller/SDL2Controller.hpp>

namespace fluorite {

    class Ogre3dCameraControll {

        private:
            Ogre::Camera* camera;
            SDL2Controller* controller;
            float speed = 4;
            float sensitivity = 0.5f;

        public: 

        Ogre3dCameraControll(Ogre::Camera* cam, SDL2Controller* control);
        Ogre3dCameraControll(Ogre::SceneManager *sceneMgr, SDL2Controller* control);
        Ogre3dCameraControll& setPos(Ogre::Vector3 pos);
        Ogre::Camera* getCamera();

        
        Ogre3dCameraControll& lookAt(Ogre::Vector3 pos);

        void frame(float delta);
    };
}