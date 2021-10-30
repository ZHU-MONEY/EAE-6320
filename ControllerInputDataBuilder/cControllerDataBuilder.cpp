#include "cControllerDataBuilder.h"
#include <iostream>
#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <fstream>
#include <Engine/Asserts/Asserts.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Assets::cControllerDataBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = Results::Success;

	std::vector<uint16_t> strengthArray;
	std::vector<cControllerInput::sVibrationSequence> sequencesArray;



	if (!(result = LoadAsset(m_path_source, strengthArray, sequencesArray)))
	{
		EAE6320_ASSERTF(false, "Failed to read controller file");
		eae6320::Assets::OutputErrorMessage("Failed to read controller file");
		return result;
	}

	std::vector<size_t> sizesOfEachSequence;
	std::vector<uint16_t> allStrengths;
	std::vector<float> allTimes;
	for (auto sequence : sequencesArray)
	{
		sizesOfEachSequence.push_back(sequence.times.size());

		for (size_t i = 0; i < sequence.times.size(); ++i)
		{
			allStrengths.push_back(sequence.strengths[i]);
			allTimes.push_back(sequence.times[i]);
		}
	}


	std::ofstream outFile(m_path_target, std::ofstream::binary);

	size_t strengthCount = strengthArray.size();
	size_t sequencesCount = sequencesArray.size();
	size_t allStrengthsCount = allStrengths.size();


	//strength level count
	outFile.write((char*)&strengthCount, sizeof(size_t));

	//strength levels
	outFile.write((char*)&strengthArray[0], strengthCount*sizeof(uint16_t));

	//sequence count
	outFile.write((char*)&sequencesCount, sizeof(size_t));
	//array of each sequence size
	outFile.write((char*)&sizesOfEachSequence[0], sequencesCount * sizeof(size_t));

	//all sequences strength combined count
	outFile.write((char*)&allStrengthsCount, sizeof(size_t));
	//all sequences strength combined
	outFile.write((char*)&allStrengths[0], allStrengthsCount * sizeof(uint16_t));

	//no need for all times count because it is the same of the  allStrengthsCount

	//all sequences time combined
	outFile.write((char*)&allTimes[0], allStrengthsCount * sizeof(float));

	outFile.close();
	return result;

}

eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadAsset(const char* const i_path, std::vector<uint16_t>& io_strengthArray, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray)
{
	auto result = eae6320::Results::Success;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
		{
			if (luaState)
			{
				// If I haven't made any mistakes
				// there shouldn't be anything on the stack
				// regardless of any errors
				EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua state" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});
	result = LoadTableValues(*luaState, io_strengthArray, io_sequencesArray);

	return result;
}

eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadTableValues(lua_State& io_luaState, std::vector<uint16_t>& io_strengthArray, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray)
{
	auto result = eae6320::Results::Success;

	if (!(result = LoadTableValues_strengthArray(io_luaState, io_strengthArray)))
	{
		return result;
	}
	if (!(result = LoadTableValues_sequencesArray(io_luaState, io_sequencesArray)))
	{
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadTableValues_strengthArray(lua_State& io_luaState, std::vector<uint16_t>& io_strengthArray)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "vibrationStrength";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every vibrationStrength" << std::endl;
		const auto strengthCount = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= strengthCount; ++i) {
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			io_strengthArray.push_back((uint16_t)lua_tonumber(&io_luaState, -1));
			cScopeGuard pop([&io_luaState] {lua_pop(&io_luaState, 1); });
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}


///**
eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadTableValues_sequencesArray(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "vibrationSequences";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every vibrationSequence" << std::endl;
		const auto sequenceCount = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= sequenceCount; ++i)
		{
			io_sequencesArray.push_back(cControllerInput::sVibrationSequence());
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			std::cout << "about to decode sequence: "<< i << std::endl;
			if (lua_istable(&io_luaState, -1))
			{
				if (!(result = LoadTableValues_sequencesArray_strength(io_luaState, io_sequencesArray)))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "The value at \"" << key << "\" must be a table "
						"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
					return result;
				}
				if (!(result = LoadTableValues_sequencesArray_time(io_luaState, io_sequencesArray)))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "The value at \"" << key << "\" must be a table "
						"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
					return result;
				}
				lua_pop(&io_luaState, 1);
			}
			else
			{
				std::cout << "FAILED to decode sequence: " << i << std::endl;
				result = eae6320::Results::InvalidFile;
				std::cerr << "The value at \"" << key << "\" must be a table "
					"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
				return result;
			}

		}

	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadTableValues_sequencesArray_strength(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "strength";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every vibrationSequences_strength" << std::endl;
		const auto vibrationSequences_strengthCount = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= vibrationSequences_strengthCount; ++i) {
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			io_sequencesArray.back().strengths.push_back((uint16_t)lua_tonumber(&io_luaState, -1));
			std::cout << "strength is " << io_sequencesArray.back().strengths.back() << std::endl;
			cScopeGuard pop([&io_luaState] {lua_pop(&io_luaState, 1); });
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cControllerDataBuilder::LoadTableValues_sequencesArray_time(lua_State& io_luaState, std::vector<cControllerInput::sVibrationSequence>& io_sequencesArray)
{
	auto result = eae6320::Results::Success;
	constexpr auto* const key = "time";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});
	if (lua_istable(&io_luaState, -1))
	{
		std::cout << "Iterating through every vibrationSequences_time" << std::endl;
		const auto vibrationSequences_strengthCount = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= vibrationSequences_strengthCount; ++i) {
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			io_sequencesArray.back().times.push_back((float)lua_tonumber(&io_luaState, -1));
			std::cout << "time is " << io_sequencesArray.back().times.back() << std::endl;
			cScopeGuard pop([&io_luaState] {lua_pop(&io_luaState, 1); });
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
		return result;
	}

	return result;
}

//*/
