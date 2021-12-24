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
		Contract
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
	void MidIntensityTrack(int wave);
	void HighIntensityTrack(int wave);
	void MissionCompletedSound();
	void FinalWaveMusic();
	void FinalWaveMusicHighIntensity();
}
