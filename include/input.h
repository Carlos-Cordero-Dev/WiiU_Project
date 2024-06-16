
#ifndef INPUT_H
#define INPUT_H 1
/*

VPAD -> wii u gamepad
KPAD -> high level wrapper for WPAD -> wii remote?

TODO:
check current controll system

*/



#include <vpad/input.h>

#include <padscore/kpad.h> //WII REMOTE -> wrapper on WPAD 

#include "drc-test/utils.h"

#include "logger.h"

static VPADStatus vpad;
// detecting keys
// vpad.hold | .trigger | release
// compare against VPADButtons

static VPADReadError vError;

void InitWiiUGamepad()
{
	VPADInit();
}

void ReadInputWiiUGamepad()
{
	VPADRead(VPADChan::VPAD_CHAN_0/*real wiiu has always this on channel 0*/, &vpad, 1, &vError);

	std::string controller_type = "WiiUGamepad";

	switch (vError) {
		case VPAD_READ_SUCCESS:
			// writes to the screen directly 
			// 
			/*
				void write(uint32_t row, uint32_t column, const char* format, ...) { //const char* write
					OSScreenPutFontEx(SCREEN_TV, row, column, format);
					OSScreenPutFontEx(SCREEN_DRC, row, column, format);
				}
			*/
			//write(15, 1, "Success!");
			VORP_LOG("%s succesfull input read %d key pressed", controller_type.c_str(), vpad.trigger);
			return true;
		case VPAD_READ_NO_SAMPLES:
			VORP_LOG("%s no samples", controller_type.c_str());
			return false;
		case VPAD_READ_INVALID_CONTROLLER:
			VORP_LOG("%s invalid Controller", controller_type.c_str());
			return false;
		default:
			VORP_LOG("%s unkown error %s", controller_type.c_str() ,hex_tostring(vError, 8));
			return false;
	}

}

void get_gamepad()
{

}

static KPADStatus kpad;
static KPADError kError;

void InitWiiController()
{
	KPADInit();
}

void ReadInputWiiController()
{
	KPADReadEx(KPADChan::WPAD_CHAN_0, &kpad, 256/*wtf*/, &kError);

	std::string controller_type = "Wii Controller";

	switch (vError) {
	case  KPAD_ERROR_OK:
			VORP_LOG("%s succesfull input read", controller_type.c_str());
			return true;
		case KPAD_ERROR_NO_SAMPLES:
			VORP_LOG("%s no samples", controller_type.c_str());
			return false;
		case KPAD_ERROR_INVALID_CONTROLLER:
			VORP_LOG("%s invalid Controller", controller_type.c_str());
			return false;
		default:
			VORP_LOG("%s unkown error %s", controller_type.c_str(), hex_tostring(vError, 8));
			return false;
	}
}

#endif // INPUT_H
