#pragma once
#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <Engine/Results/Results.h>
#include <Engine/ControllerInputSystem/cControllerInput.h>
#include <External/Lua/Includes.h>
namespace eae6320
{
	namespace Assets
	{
		class cControllerDataBuilder : public iBuilder
		{
			cResult Build(const std::vector<std::string>& i_arguments) final;

			eae6320::cResult LoadAsset(const char* const i_path, std::vector<uint16_t>& io_strengthArray, std::vector<cControllerInput::sVibrationSequence>&  io_sequencesArray);

			eae6320::cResult LoadTableValues(lua_State& io_luaState, std::vector<uint16_t>& io_strengthArray, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray);

			eae6320::cResult LoadTableValues_strengthArray(lua_State& io_luaState, std::vector<uint16_t>& io_strengthArray);

			eae6320::cResult LoadTableValues_sequencesArray(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray);
			eae6320::cResult LoadTableValues_sequencesArray_strength(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray);
			eae6320::cResult LoadTableValues_sequencesArray_time(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray);
		};
	}
}

