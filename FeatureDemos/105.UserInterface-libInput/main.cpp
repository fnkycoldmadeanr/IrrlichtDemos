/** Example 005 User Interface

This tutorial shows how to use the built in User Interface of
the Irrlicht Engine. It will give a brief overview and show
how to create and use windows, buttons, scroll bars, static
texts, and list boxes.

As always, we include the header files, and use the irrlicht
namespaces. We also store a pointer to the Irrlicht device,
a counter variable for changing the creation position of a window,
and a pointer to a listbox.
*/
#include <irrlicht.h>
#include "driverChoice.h"
#include <X11/Xutil.h>
#include <cstdio>
#include <thread>

//#if USE_X11
	#include "libInput-main/src/x11/X11InputController.h"
	#include "libInput-main/src/x11/X11Keyboard.h"
//#else

using namespace std;

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif

// Declare a structure to hold some context for the event receiver so that it
// has it available inside its OnEvent() method.
struct SAppContext
{
	IrrlichtDevice *device;
	s32				counter;
	IGUIListBox*	listbox;
};

// Define some values that we'll use to identify individual GUI controls.
enum
{
	GUI_ID_QUIT_BUTTON = 101,
	GUI_ID_NEW_WINDOW_BUTTON,
	GUI_ID_FILE_OPEN_BUTTON,
	GUI_ID_TRANSPARENCY_SCROLL_BAR
};

/*
	Set the skin transparency by changing the alpha values of all skin-colors
*/
void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin)
{
	for (s32 i=0; i<irr::gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(alpha);
		skin->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
}

/*
The Event Receiver is not only capable of getting keyboard and
mouse input events, but also events of the graphical user interface
(gui). There are events for almost everything: Button click,
Listbox selection change, events that say that a element was hovered
and so on. To be able to react to some of these events, we create
an event receiver.
We only react to gui events, and if it's such an event, we get the
id of the caller (the gui element which caused the event) and get
the pointer to the gui environment.
*/
class MyEventReceiver : public IEventReceiver
{
public:
	MyEventReceiver(SAppContext & context) : Context(context) { }

	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			IGUIEnvironment* env = Context.device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{

			/*
			If a scrollbar changed its scroll position, and it is
			'our' scrollbar (the one with id GUI_ID_TRANSPARENCY_SCROLL_BAR), then we change
			the transparency of all gui elements. This is a very
			easy task: There is a skin object, in which all color
			settings are stored. We simply go through all colors
			stored in the skin and change their alpha value.
			*/
			case EGET_SCROLL_BAR_CHANGED:
				if (id == GUI_ID_TRANSPARENCY_SCROLL_BAR)
				{
					s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
					setSkinTransparency(pos, env->getSkin());
				}
				break;

			/*
			If a button was clicked, it could be one of 'our'
			three buttons. If it is the first, we shut down the engine.
			If it is the second, we create a little window with some
			text on it. We also add a string to the list box to log
			what happened. And if it is the third button, we create
			a file open dialog, and add also this as string to the list box.
			That's all for the event receiver.
			*/
			case EGET_BUTTON_CLICKED:
				switch(id)
				{
				case GUI_ID_QUIT_BUTTON:
					Context.device->closeDevice();
					return true;

				case GUI_ID_NEW_WINDOW_BUTTON:
					{
					Context.listbox->addItem(L"Window created");
					Context.counter += 30;
					if (Context.counter > 200)
						Context.counter = 0;

					IGUIWindow* window = env->addWindow(
						rect<s32>(100 + Context.counter, 100 + Context.counter, 300 + Context.counter, 200 + Context.counter),
						false, // modal?
						L"Test window");

					env->addStaticText(L"Please close me",
						rect<s32>(35,35,140,50),
						true, // border?
						false, // wordwrap?
						window);
					}
					return true;

				case GUI_ID_FILE_OPEN_BUTTON:
					Context.listbox->addItem(L"File open");
					// There are some options for the file open dialog
					// We set the title, make it a modal window, and make sure
					// that the working directory is restored after the dialog
					// is finished.
					env->addFileOpenDialog(L"Please choose a file.", true, 0, -1, true);
					return true;

				default:
					return false;
				}
				break;

			case EGET_FILE_SELECTED:
				{
					// show the model filename, selected in the file dialog
					IGUIFileOpenDialog* dialog =
						(IGUIFileOpenDialog*)event.GUIEvent.Caller;
					Context.listbox->addItem(dialog->getFileName());
				}
				break;

			default:
				break;
			}
		}

		return false;
	}

private:
	SAppContext & Context;
};


/*
Ok, now for the more interesting part. First, create the Irrlicht device. As in
some examples before, we ask the user which driver he wants to use for this
example:
*/
int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device and exit if creation failed

	IrrlichtDevice * device = createDevice(driverType, core::dimension2d<u32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	/* The creation was successful, now we set the event receiver and
		store pointers to the driver and to the gui environment. */

	device->setWindowCaption(L"Irrlicht Engine - User Interface Demo");
	device->setResizable(true);

	video::IVideoDriver* driver = device->getVideoDriver();
	IGUIEnvironment* env = device->getGUIEnvironment();


	//Display* display = XOpenDisplay(NULL);




	Display* display =	(Display*)device->getVideoDriver()->getExposedVideoData().OpenGLLinux.X11Display;
//	int screen = DefaultScreen(display);
	Window main_window;
//	XSetWindowAttributes window_attrs;
//	window_attrs.background_pixel = XWhitePixel(display, screen);
//	window_attrs.border_pixel = XBlackPixel(display, screen);
//	window_attrs.win_gravity = SouthWestGravity;
	main_window =	(Window)device->getVideoDriver()->getExposedVideoData().OpenGLLinux.X11Window;//

//	XMapWindow(display, main_window);
//	XFlush(display);

    X11InputController input_controller(display, &main_window);
	if (!input_controller.initialize()) {
		fprintf(stderr, "Failed to initialize X11 input controller\n");
		return -1;
	}

	input_controller.enumerate();
	auto keyboards = input_controller.getKeyboards();

	for (const auto& keyboard : keyboards) {
		printf("Found keyboard %s with device id: %d\n", keyboard->name(), keyboard->deviceId());
	}

	auto mice = input_controller.getMice();
	for (const auto mouse : mice) {
		printf("Found a mouse %s with device id: %d\n", mouse->name(), mouse->deviceId());
	}


	bool input_valid = false;
	char choice;
	int device_id;

	KeyboardPtr keyboard;
	MousePtr mouse;

	do {
		printf("Select the input device:\n");
		printf("1. Mouse\n");
		printf("2. Keyboard\n");

		//cin >> choice;
		//if (choice == '1') {
			printf("Enter the mouse id\n");
			cin >> device_id;
			mouse = input_controller.getMouseById(device_id);
			if (mouse) {
				printf("Selected mouse with device id: %d\n", mouse->deviceId());
				input_valid = true;
			} else {
				printf("Mouse with device id: %d not found!\n", device_id);
			}
		//} else if (choice == '2') {
			printf("Enter the keyboard id\n");
			cin >> device_id;
			keyboard = input_controller.getKeyboardById(device_id);
			if (keyboard) {
				printf("Selected keyboard with device id: %d\n", keyboard->deviceId());
				input_valid = true;
			} else {
				printf("Keyboard with device id: %d not found!\n", device_id);
			}
		//} else {
		//	input_valid = false;
		//}
	} while(!input_valid);


	printf("Press Enter to continue...");
	getchar();


//
//	int ret = input_controller.detachDeviceById(14);
//	printf("Detached ret: %d\n", ret);
//		input_controller.enumerate();


	/*
	To make the font a little bit nicer, we load an external font
	and set it as the new default font in the skin.
	To keep the standard font for tool tip text, we set it to
	the built-in font.
	*/

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);

	skin->setFont(env->getBuiltInFont(), EGDF_TOOLTIP);

	/*
	We add three buttons. The first one closes the engine. The second
	creates a window and the third opens a file open dialog. The third
	parameter is the id of the button, with which we can easily identify
	the button in the event receiver.
	*/

	env->addButton(rect<s32>(10,240,110,240 + 32), 0, GUI_ID_QUIT_BUTTON,
			L"Quit", L"Exits Program");
	env->addButton(rect<s32>(10,280,110,280 + 32), 0, GUI_ID_NEW_WINDOW_BUTTON,
			L"New Window", L"Launches a new Window");
	env->addButton(rect<s32>(10,320,110,320 + 32), 0, GUI_ID_FILE_OPEN_BUTTON,
			L"File Open", L"Opens a file");

	/*
	Now, we add a static text and a scrollbar, which modifies the
	transparency of all gui elements. We set the maximum value of
	the scrollbar to 255, because that's the maximal value for
	a color value.
	Then we create an other static text and a list box.
	*/

	env->addStaticText(L"Transparent Control:", rect<s32>(150,20,350,40), true);
	IGUIScrollBar* scrollbar = env->addScrollBar(true,
			rect<s32>(150, 45, 350, 60), 0, GUI_ID_TRANSPARENCY_SCROLL_BAR);
	scrollbar->setMax(255);
	scrollbar->setPos(255);
	setSkinTransparency( scrollbar->getPos(), env->getSkin());

	// set scrollbar position to alpha value of an arbitrary element
	scrollbar->setPos(env->getSkin()->getColor(EGDC_WINDOW).getAlpha());

	env->addStaticText(L"Logging ListBox:", rect<s32>(50,110,250,130), true);
	IGUIListBox * listbox = env->addListBox(rect<s32>(50, 140, 250, 210));
	env->addEditBox(L"Editable Text", rect<s32>(350, 80, 550, 100));

	// Store the appropriate data in a context structure.
	SAppContext context;
	context.device = device;
	context.counter = 0;
	context.listbox = listbox;

	// Then create the event receiver, giving it that context structure.
	MyEventReceiver receiver(context);

	// And tell the device to use our custom event receiver.
	device->setEventReceiver(&receiver);


	/*
	And at last, we create a nice Irrlicht Engine logo in the top left corner.
	*/
	env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),
			position2d<int>(10,10));


	/*
	That's all, we only have to draw everything.
	*/

	while(device->run() && driver)
	if (device->isWindowActive())
	{


		//if (mouse) {
			auto state = mouse->getMouseState();
			if (!state)
				continue;

			//system("clear");
			printf("Mouse state: x: %d, y: %d, btn1: %d, btn2: %d, btn3: %d\n",
					state->x, state->y, state->btn1_pressed,
					state->btn2_pressed, state->btn3_pressed);

		//}

		//if (keyboard) {
			auto kb_state = keyboard->getKeyboardState();
			bool* keys_state = kb_state->keys_pressed;
			//system("clear");
			printf("Keys pressed: ");
			for (int i = 0; i < 256; i++) {
				if (keys_state[i]) {
					printf("%d, ", i);
				}
			}
			printf("\n");
		//}
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//break;


		driver->beginScene(true, true, SColor(0,200,200,200));

		env->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}

/*
**/
