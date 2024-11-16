#include <iostream>
#include <Ogre.h>
#include <stdexcept>
#include <string>

#include <systemServices/SDL2Controller/SDL2Controller.hpp>
#include <systemServices/GameloopController/GameloopController.hpp>


int main(int argc, char ** args) {


	
	auto gameloopController = fluorite::GameloopController();


	auto sdl2Controller = fluorite::SDL2Controller();
	gameloopController.registerEvent(fluorite::GameloopController::PRE_INIT, [&](fluorite::GameloopController*, float){return sdl2Controller.startSDLWindow();});
	gameloopController.registerEvent(fluorite::GameloopController::PRE_FRAME, [&](fluorite::GameloopController*, float){return sdl2Controller.processInput();});

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