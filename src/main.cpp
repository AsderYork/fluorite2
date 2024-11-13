#include <iostream>
#include <Ogre.h>
#include <stdexcept>
#include <string>

#include <systemServices/SDL2Service/SDL2Service.hpp>


int main(int argc, char ** args) {

	try
	{
        auto sdl = fluorite::SDL2Service();
        sdl.init("tess", 620, 480);
        while(sdl.frame()){}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}


	return 0;
}