/*
  Copyright (C) 2011 Thijs Ferket (RdR)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <irrlicht.h>
#include "driverChoice.h"
#include "ProgressBar.h"

using namespace irr;

int main() {
	// ask user for driver
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType == video::EDT_COUNT)
		return 1;

	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType = driverType;
	params.WindowSize = core::dimension2d<u32>(1024, 768);
	params.Bits = 32;
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	// Set window title
	core::stringw str = L"Driver: ";
	str += driver->getName();
	device->setWindowCaption(str.c_str());

    // Create a new progressbar
	ProgressBar* bar = new ProgressBar(env);
	bar->setPosition(irr::core::vector2di(10, 10));
	bar->setDimension(300, 100);
	bar->setTextColor(irr::video::SColor(255, 255, 255, 255));

	while (device->run()) {
		if (device->isWindowActive()) {
			driver->beginScene(true, true, 0);

			smgr->drawAll();

			env->drawAll();

			driver->endScene();
		}
	}

	device->drop();

	return 0;
}
