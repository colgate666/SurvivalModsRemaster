#pragma once
#include "natives.h"
#include "Calculations.hpp"
#include <string>

namespace MUSIC
{
	enum class TrackGroups : int
	{
		SurvivalPrimary,
		BusinessBattle,
		BikerContracts,
		Casino,
		Casino2,
		Tuner,
		Hacker
	};

	struct MusicData
	{
		static TrackGroups currentTrackGroup;
		static bool finalWaveHI;
	};

	void PrepareTracks();
	void StopTrack();
	void StartTrack();
	void LowIntensityTrack();
	void Process(int enemyCount, int maxCount);
	void MissionCompletedSound();
	void FinalWaveMusic();
	void FinalWaveMusicHighIntensity();
}
