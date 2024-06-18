
#ifndef INPUT_H
#define INPUT_H 1

/*
TODO:
-Uninitialize functions calls / destructors
-check current controll system
*/

#include <vpad/input.h>

#include <padscore/kpad.h> //WII REMOTE -> wrapper on WPAD 

#include "drc-test/utils.h"

#include "logger.h"
#include "mathlibs_converters.h"
#include <glm/glm.hpp>

static VPADStatus vpad;
// detecting keys
// vpad.hold | .trigger | release
// compare against VPADButtons

static VPADReadError vError;

void InitWiiUGamepad()
{
	VPADInit();
}

bool ReadInputWiiUGamepad()
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
			//VORP_LOG("%s succesfull input read %d key pressed", controller_type.c_str(), vpad.trigger);
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


glm::vec3 get_gamepad_angle()
{
	return VpadToGlm(vpad.angle);
}

void PrintGamepadCompleteData()
{
	glm::vec3 angle = get_gamepad_angle();
	//this setup working but it accumulates degrees instead of showing the absolute rotation
	glm::vec3 angleDegrees = glm::degrees(angle);
	
	//VORP_LOG("Gyro: %f %f %f", vpad.gyro.x, vpad.gyro.y, vpad.gyro.z);
	//VORP_LOG("Angle: %f %f %f", angleDegrees.x, angleDegrees.y, angleDegrees.z);
	//VORP_LOG("Accel: %f %f %f", vpad.accelorometer.acc.x, vpad.accelorometer.acc.y, vpad.accelorometer.acc.z);
	//VORP_LOG("Accel Vert: %f %f", vpad.accelorometer.vertical.x, vpad.accelorometer.vertical.y);
}

static KPADStatus kpad[4];
//compare againt WPADButton at <padscore/wpad.h>
static KPADError kError;

void InitWiiController()
{
	KPADInit();
}

bool ReadInputWiiController(int controller_index)
{
	int read_data = 0;
	read_data = KPADReadEx((KPADChan)controller_index, &kpad[controller_index], 1, &kError);
	//if(read_data) VORP_LOG("Data read C%d", controller_index);

	std::string controller_type = "Wii Controller " + std::to_string(read_data) + " ";

	switch (vError) {
	case  KPAD_ERROR_OK:
		//VORP_LOG("%s succesfull input read %d key pressed", controller_type.c_str(),kpad.trigger);
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


void ReadInputWiiControllers()
{
	for (int i = 0; i < 4; i++)
	{
		ReadInputWiiController(i);
	}
}

void PrintWiiControllerCompleteData(int controller_index)
{
	KPADStatus& curr_kpad =  kpad[controller_index]; 


	VORP_LOG("Pointing at: %f %f", curr_kpad.pos.x, curr_kpad.pos.y);
}

#endif // INPUT_H
