#include <SystemServices/Ogre3d/Ogre3dCameraControl/Ogre3dCameraControl.hpp>

namespace fluorite {

    Ogre3dCameraControll::Ogre3dCameraControll(Ogre::Camera* cam, SDL2Controller* control) : camera(cam), controller(control)  {}

    Ogre3dCameraControll::Ogre3dCameraControll(Ogre::SceneManager *sceneMgr, SDL2Controller* control) : controller(control)  {

            auto camNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
            auto mainCam = sceneMgr->createCamera("MainCam");
            mainCam->setNearClipDistance(0.1f);
            mainCam->setAutoAspectRatio(true);
            camNode->attachObject(mainCam);
            camNode->setPosition(0, 0, 0);
            camNode->lookAt(Ogre::Vector3(-8, 0, 0), Ogre::Node::TS_WORLD);
            camera = mainCam;
        }

    Ogre3dCameraControll& Ogre3dCameraControll::setPos(Ogre::Vector3 pos) {
        camera->getParentSceneNode()->setPosition(pos);
        return *this;
    }

    Ogre3dCameraControll& Ogre3dCameraControll::lookAt(Ogre::Vector3 lookAt) {
        camera->getParentSceneNode()->lookAt(lookAt, Ogre::Node::TS_WORLD);
        return *this;
    }

    
    Ogre::Camera* Ogre3dCameraControll::getCamera() {
        return camera;
    }


    void Ogre3dCameraControll::frame(float delta) {

        auto tq = controller->getMouseData().xrel * -sensitivity * delta;

        camera->getParentSceneNode()->yaw(Ogre::Radian(controller->getMouseData().xrel * -sensitivity * delta), Ogre::Node::TransformSpace::TS_WORLD);
        camera->getParentSceneNode()->pitch(Ogre::Radian(controller->getMouseData().yrel * -sensitivity * delta));

        auto currPos = camera->getParentNode()->getPosition();

        auto moveDir = Ogre::Vector3(0);
        moveDir.x = controller->isKeyPressed(SDLK_d) - controller->isKeyPressed(SDLK_a);
        moveDir.z = controller->isKeyPressed(SDLK_s) - controller->isKeyPressed(SDLK_w);
        moveDir.normalise();
        moveDir = moveDir * delta * speed;
        moveDir = camera->getParentSceneNode()->getOrientation() * moveDir;

        
        auto flyDir = Ogre::Vector3(0);
        flyDir.y = controller->isKeyPressed(SDLK_SPACE) - controller->isKeyPressed(SDLK_LSHIFT);
        flyDir.normalise();
        flyDir = flyDir * delta * speed;
        
        currPos = currPos + moveDir + flyDir;

        camera->getParentNode()->setPosition(currPos);

    }

}