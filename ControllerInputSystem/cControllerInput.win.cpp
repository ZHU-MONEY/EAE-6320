#include "cControllerInput.h"
#include "math.h"
#include "Engine/Platform/Platform.h"

cControllerInput::sControllerState state;
uint16_t vibrationStrengthWeak;
uint16_t vibrationStrengthMedium;
uint16_t vibrationStrengthStrong;
float vibrationTime = 0;
std::vector<cControllerInput::sVibrationSequence> customSequences;
cControllerInput::sVibrationSequence currSequence;
size_t currIndex = 0;

eae6320::cResult cControllerInput::Load(const char* i_controllerDataPath)
{
	auto result = eae6320::Results::Success;

	eae6320::Platform::sDataFromFile dataFromFile;
	eae6320::Platform::LoadBinaryFile(i_controllerDataPath, dataFromFile);

	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	const auto strengthCount = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(strengthCount);

	auto* strengthArray = reinterpret_cast<uint16_t*>(currentOffset);
	currentOffset += sizeof(uint16_t) * strengthCount;

	const auto sequencesCount = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(sequencesCount);

	auto* sizesOfEachSequence = reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(size_t) * sequencesCount;

	const auto allStrengthsCount = *reinterpret_cast<size_t*>(currentOffset);
	currentOffset += sizeof(allStrengthsCount);

	auto* allStrengths = reinterpret_cast<uint16_t*>(currentOffset);
	currentOffset += sizeof(uint16_t) * allStrengthsCount;

	auto* allTimes = reinterpret_cast<float*>(currentOffset);
	//currentOffset += sizeof(uint16_t) * allStrengthsCount;

	vibrationStrengthWeak = strengthArray[0];
	vibrationStrengthMedium = strengthArray[1];
	vibrationStrengthStrong = strengthArray[2];

	size_t offset = 0;
	for (size_t i = 0; i < sequencesCount; i++)
	{
		customSequences.push_back(cControllerInput::sVibrationSequence());
		size_t sqSize = sizesOfEachSequence[i];

		for (size_t j = 0; j < sqSize; j++)
		{
			customSequences.back().strengths.push_back(allStrengths[offset+j]);
			customSequences.back().times.push_back(allTimes[offset + j]);
		}
		offset += sqSize;
	}

	return result;
}

bool cControllerInput::IsConnected()
{
	XINPUT_STATE xinputState;
	ZeroMemory(&xinputState, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	DWORD dwResult = XInputGetState(0, &xinputState);

	if (dwResult == ERROR_SUCCESS)
	{
		// Controller is connected
		return true;
	}
	else
	{
		// Controller is not connected
		return false;
	}
}

void cControllerInput::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
	UpdateState();
	UpdateTimer(i_elapsedSecondCount_sinceLastUpdate);
}
void cControllerInput::UpdateState()
{
	uint16_t vibrationStrenght = 0;
	if(currSequence.strengths.size()>0)
	{
		if (currSequence.strengths[currIndex] == 1)
		{
			vibrationStrenght = vibrationStrengthWeak;
		}
		if (currSequence.strengths[currIndex] == 2)
		{
			vibrationStrenght = vibrationStrengthMedium;
		}
		if (currSequence.strengths[currIndex] == 3)
		{
			vibrationStrenght = vibrationStrengthStrong;
		}
	}

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = vibrationStrenght;
	vibration.wRightMotorSpeed = vibrationStrenght;
	XInputSetState(0, &vibration);

	//state.ResetState();
	XINPUT_STATE xinputState;
	ZeroMemory(&xinputState, sizeof(XINPUT_STATE));
	XInputGetState(0, &xinputState);




	//if the dwPacketNumber has not been changed, then there is no change to the input
	if (xinputState.dwPacketNumber != state.packedNumber)
	{
		state.ResetState();
		//Buttons
		{
			//DPAD
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				state.buttonStates[eButtonCode::GAMEPAD_DPAD_UP] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				state.buttonStates[eButtonCode::GAMEPAD_DPAD_DOWN] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				state.buttonStates[eButtonCode::GAMEPAD_DPAD_LEFT] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				state.buttonStates[eButtonCode::GAMEPAD_DPAD_RIGHT] = true;
			}

			//start and back
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				state.buttonStates[eButtonCode::GAMEPAD_START] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				state.buttonStates[eButtonCode::GAMEPAD_BACK] = true;
			}

			//thumb and shoulder
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
			{
				state.buttonStates[eButtonCode::GAMEPAD_LEFT_THUMB] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
			{
				state.buttonStates[eButtonCode::GAMEPAD_RIGHT_THUMB] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				state.buttonStates[eButtonCode::GAMEPAD_LEFT_SHOULDER] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				state.buttonStates[eButtonCode::GAMEPAD_RIGHT_SHOULDER] = true;
			}

			//ABXY
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				state.buttonStates[eButtonCode::GAMEPAD_A] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				state.buttonStates[eButtonCode::GAMEPAD_B] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				state.buttonStates[eButtonCode::GAMEPAD_X] = true;
			}
			if (xinputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				state.buttonStates[eButtonCode::GAMEPAD_Y] = true;
			}
		}
		//left thumbstick
		{
			float LX = xinputState.Gamepad.sThumbLX;
			float LY = xinputState.Gamepad.sThumbLY;

			//determine how far the controller is pushed
			float magnitude = sqrtf(LX * LX + LY * LY);

			//determine the direction the controller is pushed
			//float normalizedLX = LX / magnitude;
			//float normalizedLY = LY / magnitude;

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767) magnitude = 32767;

				//adjust magnitude relative to the end of the dead zone
				//magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

				state.leftStickState.x = xinputState.Gamepad.sThumbLX;
				state.leftStickState.y = xinputState.Gamepad.sThumbLY;
				state.normalizedLeftStickState.x = LX / 32767;
				state.normalizedLeftStickState.y = LY / 32767;
			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
				state.leftStickState.Reset();
			}

		}
		//right thumbstick
		{
			float RX = xinputState.Gamepad.sThumbRX;
			float RY = xinputState.Gamepad.sThumbRY;

			//determine how far the controller is pushed
			float magnitude = sqrtf(RX * RX + RY * RY);

			//determine the direction the controller is pushed
			//float normalizedRX = RX / magnitude;
			//float normalizedRY = RY / magnitude;

			float normalizedMagnitude = 0;

			//check if the controller is outside a circular dead zone
			if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767) magnitude = 32767;

				//adjust magnitude relative to the end of the dead zone
				//magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

				//optionally normalize the magnitude with respect to its expected range
				//giving a magnitude value of 0.0 to 1.0
				normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

				state.rightStickState.x = xinputState.Gamepad.sThumbRX;
				state.rightStickState.y = xinputState.Gamepad.sThumbRY;
				state.normalizedRightStickState.x = RX / 32767;
				state.normalizedRightStickState.y = RY / 32767;

			}
			else //if the controller is in the deadzone zero out the magnitude
			{
				magnitude = 0.0;
				normalizedMagnitude = 0.0;
				state.rightStickState.Reset();
			}

		}

		//triggers
		{
			//right trigger
			if (xinputState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				state.rightTriggerState = xinputState.Gamepad.bRightTrigger;
				state.normalizedRightTriggerState = xinputState.Gamepad.bRightTrigger / (float)(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			}

			if (xinputState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				state.leftTriggerState = xinputState.Gamepad.bLeftTrigger;
				state.normalizedLeftTriggerState = xinputState.Gamepad.bLeftTrigger / (float)(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			}

		}
	}
	//update the packet number
	state.packedNumber = xinputState.dwPacketNumber;
}

void cControllerInput::UpdateTimer(const float i_elapsedSecondCount_sinceLastUpdate)
{
	if (vibrationTime > 0.0f)
		vibrationTime -= i_elapsedSecondCount_sinceLastUpdate;
	if (vibrationTime <= 0.0f) {
		VibrateStop();
	}

}

void cControllerInput::Vibrate(uint16_t i_vibrationStrenght)
{
	currSequence.times.clear();
	currSequence.strengths.clear();

	currSequence.times.push_back(1.0f);
	currSequence.strengths.push_back(i_vibrationStrenght);
	vibrationTime = 1.0f;
}
void cControllerInput::Vibrate(eVibrationStrength i_vibrationStrenght)
{
	currSequence.times.clear();
	currSequence.strengths.clear();

	currSequence.times.push_back(1.0f);
	currSequence.strengths.push_back(static_cast<uint16_t>(i_vibrationStrenght));
	vibrationTime = 1.0f;
}
void cControllerInput::VibrateForSeconds(eVibrationStrength i_vibrationStrenght, float i_seconds)
{
	//reset currSequence
	currSequence.times.clear();
	currSequence.strengths.clear();

	currSequence.times.push_back(i_seconds);
	currSequence.strengths.push_back(static_cast<uint16_t>(i_vibrationStrenght));
	vibrationTime = i_seconds;

}
void cControllerInput::VibrateForSeconds(uint16_t i_vibrationStrenght, float i_seconds)
{
	//reset currSequence
	currSequence.times.clear();
	currSequence.strengths.clear();

	currSequence.times.push_back(i_seconds);
	currSequence.strengths.push_back(i_vibrationStrenght);
	vibrationTime = i_seconds;
}
void cControllerInput::VibrateSequence(size_t index)
{
	currSequence = customSequences[index];
	vibrationTime = customSequences[index].times[0];
}

void cControllerInput::VibrateStop()
{

	//if there is another vib in sequence then play it, otherwise stop
	currIndex++;
	if(currIndex>=currSequence.times.size())
	{
		currSequence.times.clear();
		currSequence.strengths.clear();
		currIndex = 0;
		vibrationTime = 0.0f;
	}else
	{
		vibrationTime = currSequence.times[currIndex];
	}

}

bool cControllerInput::IsKeyPressed(eButtonCode i_code)
{
	return state.buttonStates[i_code];
}

cControllerInput::sShortVector2 cControllerInput::GetLeftStickState()
{
	return state.leftStickState;
}

cControllerInput::sFloatVector2 cControllerInput::GetNormalizedLeftStickState()
{
	return state.normalizedLeftStickState;
}

cControllerInput::sShortVector2 cControllerInput::GetRightStickState()
{
	return state.rightStickState;
}

cControllerInput::sFloatVector2 cControllerInput::GetNormalizedRightStickState()
{
	return state.normalizedRightStickState;
}

short cControllerInput::GetLeftTriggerState()
{
	return state.leftTriggerState;
}

float cControllerInput::GetNormalizedLeftTriggerState()
{
	return state.normalizedLeftTriggerState;
}

short cControllerInput::GetRightTriggerState()
{
	return state.rightTriggerState;
}
float cControllerInput::GetNormalizedRightTriggerState()
{
	return state.normalizedRightTriggerState;
}
