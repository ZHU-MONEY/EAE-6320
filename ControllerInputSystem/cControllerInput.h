#pragma once
#include "ExternalLibraries.win.h"
#include "Windows.h"
#include "Xinput.h"
#include  <cstdint>
#include "Engine/Results/Results.h"
#include <vector>


class cControllerInput
{
public:
	enum eButtonCode
	{
		GAMEPAD_DPAD_UP = 0,
		GAMEPAD_DPAD_DOWN = 1,
		GAMEPAD_DPAD_LEFT = 2,
		GAMEPAD_DPAD_RIGHT = 3,

		GAMEPAD_START = 4,
		GAMEPAD_BACK = 5,

		GAMEPAD_LEFT_THUMB = 6,
		GAMEPAD_RIGHT_THUMB = 7,

		GAMEPAD_LEFT_SHOULDER = 8,
		GAMEPAD_RIGHT_SHOULDER = 9,

		GAMEPAD_A = 10,
		GAMEPAD_B = 11,
		GAMEPAD_X = 12,
		GAMEPAD_Y = 13,
	};
	//eAnalogCode is not used in the original code, but have it here, so people in the future may want to use it
	enum eAnalogCode
	{
		TRIGGER_LEFT = 0,
		TRIGGER_RIGHT = 1,
		THUMB_LEFT = 2,
		THUMB_RIGHT = 3,
	};

	//pre defined 3 level of vibrations, if you want to have more levels, contact Yi De Zhu
	enum eVibrationStrength
	{
		WEAK = 1,
		MEDIUM = 2,
		STRONG = 3
	};
	struct sShortVector2
	{
		//MIN = -32768, MAX = 32767
		short x = 0;
		short y = 0;

		void Reset()
		{
			x = 0;
			y = 0;
		}
	};
	struct sFloatVector2
	{
		float x = 0.0f;
		float y = 0.0f;

		void Reset()
		{
			x = 0.0f;
			y = 0.0f;
		}
	};
	struct sControllerState
	{
		bool buttonStates[14];
		cControllerInput::sShortVector2 leftStickState;
		cControllerInput::sShortVector2 rightStickState;
		cControllerInput::sFloatVector2 normalizedLeftStickState;
		cControllerInput::sFloatVector2 normalizedRightStickState;
		short leftTriggerState;
		short rightTriggerState;
		float normalizedLeftTriggerState;
		float normalizedRightTriggerState;
		DWORD packedNumber;

		void ResetState() {
			for (int i = 0; i < 14; i++)
				buttonStates[i] = false;
			leftStickState.Reset();
			rightStickState.Reset();
			normalizedLeftStickState.Reset();
			normalizedRightStickState.Reset();
			leftTriggerState = 0;
			rightTriggerState = 0;
			normalizedLeftTriggerState = 0.0f;
			normalizedRightTriggerState = 0.0f;
		}
	};

	// a sVibrationSequence is defined as:
	// strengths = {1,3,2,1}
	// times = {3.0f, 5,0f, 1.5f, 15.0f}
	//this means play WEAK vibration for 3 sec,
	//		then play STRONG vibration for 5 sec,
	//		then play MEDIUM vibration for 1.5 sec,
	//		then play STRONG vibration for 15 sec
	struct sVibrationSequence
	{
		std::vector<uint16_t> strengths;
		std::vector<float> times;
	};

private:
	static void UpdateState();
	static void UpdateTimer(const float i_elapsedSecondCount_sinceLastUpdate);

public:

	//static eae6320::cResult Initialize();

	//--------Initialization---------
	// call this function on the initialization of the game
	static eae6320::cResult Load(const char* i_controllerDataPath);

	//safe check, make sure to use this before doing anything with the controller to avoid crash, unless you know what you are working with
	static bool IsConnected();

	//--------Updates---------------
	// call this function inside MyGame UpdateSimulationBasedOnTime(i_elapsedSecondCount_sinceLastUpdate)
	static void Update(const float i_elapsedSecondCount_sinceLastUpdate);

	//---------------Vibarations----------------
	//for each vibration function, you can speicify the strength of vibration, you can you use a specific strength number ranging from 0 to 65535
	//or you can simply use the eVibrationStrength ENUM I defined above
	//
	//****IT IS RECOMMENDED TO USE ENUM
	//
	//default vibrations will vibrate for 1 sec
	static void Vibrate(uint16_t i_vibrationStrenght);
	static void Vibrate(eVibrationStrength i_vibrationStrenght);
	// vibration with time specified
	static void VibrateForSeconds(eVibrationStrength i_vibrationStrenght, float i_seconds);
	static void VibrateForSeconds(uint16_t i_vibrationStrenght, float i_seconds);

	// play the vibration sequence defined by the user in the LUA file, the sequneces are stored inside customSequences during initialization, you can assign key press to play a sequence saved at specific index
	//for example: 
	//		if (IsKeyPressed(controller.GAMEPAD_DPAD_DOWN))
	//			VibrateSequence(1);
	//this code will allow the user to play the sequnce saved at index 1, by pressing DPAD DOWN
	static void VibrateSequence(size_t index);

	// stop the current vibration, if there is another vibration in the sequence, play the next, otherwise, just stop play sequence
	static void VibrateStop();


	//-------------------Key Presses------------------
	//return whether a key is pressed or not
	static bool IsKeyPressed(eButtonCode i_code);

	//-------------------Thumbsticks------------------
	//return a short vector with value x and y, each value ranging from -32768 to 32767
	static sShortVector2 GetLeftStickState();
	static sShortVector2 GetRightStickState();
	//return a float vector with value x and y, each value ranging from -1.0f to 1.0f
	static sFloatVector2 GetNormalizedLeftStickState();
	static sFloatVector2 GetNormalizedRightStickState();

	//------------------Triggers---------------------
	//return a short value ranging from 0 to 255
	static short GetLeftTriggerState();
	static short GetRightTriggerState();
	//return a float value ranging from 0.0f to 1.0f
	static float GetNormalizedLeftTriggerState();
	static float GetNormalizedRightTriggerState();

};
