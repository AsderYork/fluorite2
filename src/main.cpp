#include <iostream>
#include <stdexcept>
#include <string>

#include <SystemServices/SDL2Controller/SDL2Controller.hpp>
#include <SystemServices/GameloopController/GameloopController.hpp>
#include <SystemServices/Ogre3d/Ogre3d.hpp>

#include <Terrain2/Terrain2.hpp>

#include <chrono>

class graphicSubchunkNode : public fluorite::TerrainMap::SubChunkData {
	std::optional<fluorite::GraphicsObject> graphicsObject;
public:
	graphicSubchunkNode ( graphicSubchunkNode &&  other) {
		other.graphicsObject = std::nullopt; 
	};
    graphicSubchunkNode &  operator= ( graphicSubchunkNode && ) = default;
    graphicSubchunkNode ( const graphicSubchunkNode & ) = delete;
    graphicSubchunkNode & operator= ( const graphicSubchunkNode & ) = delete;
   
	
	graphicSubchunkNode(const fluorite::GraphicsObject _graphicsObject) : graphicsObject(_graphicsObject) {}

    virtual ~graphicSubchunkNode() {
		if(graphicsObject.has_value()) {
			graphicsObject->destroy();
		}
	}
};

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;



int main(int argc, char ** args) {

	auto terrainMap = fluorite::TerrainMap(1024, 16);

	auto gameloopController = fluorite::GameloopController();

	auto sdl2Controller = fluorite::SDL2Controller();
	gameloopController.registerEvent(fluorite::GameloopController::PRE_INIT, [&](fluorite::GameloopController*, float){return sdl2Controller.startSDLWindow();});
	gameloopController.registerEvent(fluorite::GameloopController::PRE_FRAME, [&](fluorite::GameloopController*, float){return sdl2Controller.processInput();});


	auto ogre3d = fluorite::Ogre3d();
	gameloopController.registerEvent(fluorite::GameloopController::INIT, [&](fluorite::GameloopController*, float){return ogre3d.initOgre(&sdl2Controller);});
	gameloopController.registerEvent(fluorite::GameloopController::FRAME, [&](fluorite::GameloopController* game, float delta){ogre3d.ogreFrame(delta); return true;});
	gameloopController.registerEvent(fluorite::GameloopController::SHUTDOWN, [&](fluorite::GameloopController*, float){ogre3d.ogreShutdown(); return true;});

	
	float counter = 0;
	double lastop = 0;

	gameloopController.registerEvent(fluorite::GameloopController::PRE_FRAME, [&](fluorite::GameloopController*, float delta){
		counter += 0;

		auto cameraPpos = ogre3d.getCamera()->getPos();
		

		terrainMap.resetInUseFlags();
		auto t1 = high_resolution_clock::now();
		terrainMap.createChunksForAViewpoint({(int)(cameraPpos.x * 100), 0, (int)(cameraPpos.z * 100)}, 4096);
		auto t2 = high_resolution_clock::now();
		terrainMap.clearUnusedChunks();
		
		/* Getting number of milliseconds as a double. */
		duration<double, std::milli> ms_double = t2 - t1;
		lastop = ms_double.count();

		

		terrainMap.initializeSubchunks([&](fluorite::TerrainMap::SubChunk* subchunk) {
		
			if(subchunk->pos.y != 0) {return;}

			auto colorValue = Ogre::ColourValue();
			colorValue.setHSB(fmod(subchunk->size * 0.17, 1), 0.8f, 0.8f);
			auto graphicObject = ogre3d.testCube((float)subchunk->pos.x / 100.0f, (float)subchunk->pos.y / 100.0f, (float)subchunk->pos.z / 100.0f, (float)subchunk->size/ 100.0f, colorValue);
			subchunk->subchunkData.push_back(std::make_shared<graphicSubchunkNode>(graphicObject));
		});
		return true;
	});

	
	gameloopController.registerEvent(fluorite::GameloopController::POST_FRAME, [&](fluorite::GameloopController*, float delta){

				
		std::stringstream stream;
		stream << "Fluorite";
		stream << " FPS:" << std::fixed << std::setprecision(1) << (1.0f/delta) << ";";
		stream << " lastop:" << std::fixed << std::setprecision(3) << lastop << ";";


		auto text = stream.str();

		sdl2Controller.setWindowCaption(text);
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