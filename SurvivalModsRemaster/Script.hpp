#pragma once

#include "Controls.hpp"
#include "natives.h"
#include "Screen.hpp"
#include "Calculations.hpp"
#include "Survival.hpp"
#include "Initialization.hpp"
#include <map>
#include <vector>
#include <string>

enum class SurvivalModes {
    TenWaves,
    Endless,
    Timed,
    Hardcore
};

struct Data
{
	static Hash enemiesRelGroup;
	static Hash neutralRelGroup;
	static int intermissionDuration;
    static Controls infiniteWaveControl;
	static Controls timedSurvivalControl;
	static Controls hardcoreSurvivalControl;
	static Controls cancelControl;
	static Controls reloadTriggerPedsControl;
	static bool showControls;
	static int TPIndex;
};

struct EntityPosition
{
	Vector3 coords;
	float heading;
};

struct SurvivalAllies
{
	std::string MissionID;
	std::string RelGroupName;
};

struct TriggerPedsData
{
	static std::vector<SurvivalAllies> allies;
	static std::vector<std::string> names;
	static std::vector<std::string> models;
	static std::vector<Ped> peds;
	static std::vector<EntityPosition> positions;
	static std::vector<Blip> blips;
	static std::vector<bool> timerActive;
	static std::vector<int> starTime;
	static std::vector<bool> killedFlags;
	static std::vector<std::string> tasks;
    static std::vector<int> playerRel;

	static void ClearTriggerPeds()
	{
		size_t size = blips.size();

		for (size_t i = 0; i < size; i++)
		{
			Blip blip = blips.at(i);
			Ped ped = peds.at(i);

			if (blip != 0)
				HUD::REMOVE_BLIP(&blip);

			if (ped != 0)
				ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}

		names.clear();
		models.clear();
		positions.clear();
		blips.clear();
		timerActive.clear();
		starTime.clear();
		starTime.clear();
		tasks.clear();
		peds.clear();
		killedFlags.clear();
		allies.clear();
        playerRel.clear();
	}
};

void LoadNY();
void UnloadNY();
void UnloadBunker();
void ScriptMain();
void OnAbort();
