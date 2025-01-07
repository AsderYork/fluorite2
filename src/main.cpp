#include <iostream>
#include <stdexcept>
#include <string>

#include <SystemServices/SDL2Controller/SDL2Controller.hpp>
#include <SystemServices/GameloopController/GameloopController.hpp>
#include <SystemServices/Ogre3d/Ogre3d.hpp>


int main(int argc, char ** args) {
	auto gameloopController = fluorite::GameloopController();

	auto sdl2Controller = fluorite::SDL2Controller();
	gameloopController.registerEvent(fluorite::GameloopController::PRE_INIT, [&](fluorite::GameloopController*, float){return sdl2Controller.startSDLWindow();});
	gameloopController.registerEvent(fluorite::GameloopController::PRE_FRAME, [&](fluorite::GameloopController*, float){return sdl2Controller.processInput();});


	auto ogre3d = fluorite::Ogre3d();
	gameloopController.registerEvent(fluorite::GameloopController::INIT, [&](fluorite::GameloopController*, float){return ogre3d.initOgre(&sdl2Controller);});
	gameloopController.registerEvent(fluorite::GameloopController::FRAME, [&](fluorite::GameloopController* game, float delta){ogre3d.ogreFrame(delta); return true;});
	gameloopController.registerEvent(fluorite::GameloopController::SHUTDOWN, [&](fluorite::GameloopController*, float){ogre3d.ogreShutdown(); return true;});


	//General init. Mostly for testing purposes
	gameloopController.registerEvent(fluorite::GameloopController::INIT, [&](fluorite::GameloopController*, float){
		ogre3d.testCube(-8, 0, 2);
		ogre3d.testCube(-8, 0, -2);
		return true;
	});



	try
	{
		gameloopController.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}