#include <iostream>
#include <stdexcept>
#include <string>

#include <SystemServices/SDL2Controller/SDL2Controller.hpp>
#include <SystemServices/GameloopController/GameloopController.hpp>
#include <SystemServices/Ogre3d/Ogre3d.hpp>

#include <Terrain2/Terrain2.hpp>


int main(int argc, char ** args) {

	auto terrainMap = fluorite::TerrainMap();

	terrainMap.loadForAPoint({0,0,0}, 512);
	terrainMap.createChunksForAViewpoint({0,0,0}, 512);
	terrainMap.resetInUseFlags();

	terrainMap.createChunksForAViewpoint({0,0,0}, 512);

	terrainMap.clearUnusedChunks();

	auto terminalChunks = terrainMap.getTerminalChunks();

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

		/**
		 * Test cubes actualy have size 2 and they begin at their geometric center, not at minimal cords
		 */
		for(auto grapgicChunkPtr : terminalChunks) {

			auto colorValue = Ogre::ColourValue();
			colorValue.setHSB(fmod(grapgicChunkPtr->lod * 0.17, 1), 0.8f, 0.8f);
			ogre3d.testCube((float)grapgicChunkPtr->pos.x / 100.0f, (float)grapgicChunkPtr->pos.y / 100.0f, (float)grapgicChunkPtr->pos.z / 100.0f, (float)grapgicChunkPtr->size/ 100.0f, colorValue);
		}

		return true;
	});



	
	gameloopController.registerEvent(fluorite::GameloopController::POST_FRAME, [&](fluorite::GameloopController*, float delta){
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "%.1f", 1.0f/delta);

		sdl2Controller.setWindowCaption("Fluorite FPS:" + std::string(buffer, 64));
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