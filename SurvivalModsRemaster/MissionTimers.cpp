#include "pch.h"
#include "MissionTimers.hpp"

int TIMERS::TriggerPed::CurrentTime;
int TIMERS::TriggerPed::StartTime;
bool TIMERS::TriggerPed::Started;
int TIMERS::Intermission::CurrentTime;
int TIMERS::Intermission::StartTime;
bool TIMERS::Intermission::Started;
int TIMERS::Intermission::timeLeft;
int TIMERS::LeavingZone::CurrentTime;
int TIMERS::LeavingZone::StartTime;
bool TIMERS::LeavingZone::Started;
int TIMERS::SpawnEnemy::CurrentTime;
int TIMERS::SpawnEnemy::StartTime;
bool TIMERS::SpawnEnemy::Started;
int TIMERS::SpawnAircraft::CurrentTime;
int TIMERS::SpawnAircraft::StartTime;
bool TIMERS::SpawnAircraft::Started;
int TIMERS::SpawnVehicle::CurrentTime;
int TIMERS::SpawnVehicle::StartTime;
bool TIMERS::SpawnVehicle::Started;
int TIMERS::TimedSurvival::CurrentTime;
int TIMERS::TimedSurvival::StartTime;
int TIMERS::TimedSurvival::InterWaveTime;
bool TIMERS::TimedSurvival::Started;
int TIMERS::TimedSurvival::timeLeft;
int TIMERS::DogTimer::CurrentTime;
int TIMERS::DogTimer::StartTime;
bool TIMERS::DogTimer::Started;
int soundId = -1;
int lastSoundTime;

bool TIMERS::ProcessTimedSurvivalTimer(int endTime)
{
	if (!TimedSurvival::Started)
	{
		TimedSurvival::Started = true;
		TimedSurvival::StartTime = MISC::GET_GAME_TIMER();
		TimedSurvival::InterWaveTime = TimedSurvival::StartTime;
		return false;
	}
	else
	{
		TimedSurvival::CurrentTime = MISC::GET_GAME_TIMER();

		if (TimedSurvival::CurrentTime - TimedSurvival::InterWaveTime >= 120000)
		{
			TimedSurvival::InterWaveTime = MISC::GET_GAME_TIMER();
			INTERMISSION::Start();
			INTERMISSION::Process();
		}

		TimedSurvival::timeLeft = endTime - (TimedSurvival::CurrentTime - TimedSurvival::StartTime);

		if (TimedSurvival::CurrentTime - TimedSurvival::StartTime >= endTime)
		{
			TimedSurvival::Started = false;
			return true;
		}
		return false;
	}
}

void TIMERS::RestartTriggerPedTimer()
{
	TriggerPed::Started = false;
}

bool TIMERS::ProcessTriggerPedTimer()
{
	if (!TriggerPed::Started)
	{
		TriggerPed::Started = true;
		TriggerPed::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		TriggerPed::CurrentTime = MISC::GET_GAME_TIMER();
		if (TriggerPed::CurrentTime - TriggerPed::StartTime >= 10000)
		{
			TriggerPed::Started = false;
			return true;
		}
		return false;
	}
}

void TIMERS::RestartTimers()
{
	TimedSurvival::Started = false;
	Intermission::Started = false;
	SpawnAircraft::Started = false;
	LeavingZone::Started = false;
	SpawnEnemy::Started = false;
	SpawnVehicle::Started = false;
	DogTimer::Started = false;
	
	if (soundId != -1)
	{
		AUDIO::RELEASE_SOUND_ID(soundId);
		soundId = -1;
	}
}

void TIMERS::RestartLeavingZoneTimer()
{
	LeavingZone::Started = false;
}

void TIMERS::RestartSpawnTimers()
{
	SpawnAircraft::Started = false;
	SpawnVehicle::Started = false;
	SpawnEnemy::Started = false;
	DogTimer::Started = false;
}

bool TIMERS::LeavingZoneTimerStarted()
{
	return LeavingZone::Started;
}

void TIMERS::RestartIntermissionTimer()
{
	Intermission::Started = false;
}

const char* GetSoundGroup()
{
	if (SURVIVAL::SurvivalData::zombies)
	{
		return "DLC_24-1_YK_Survival_Sounds";
	}

	return "DLC_VW_Survival_Sounds";
}

void PlaySound(const char* soundName, bool bypass = false)
{
	if (soundId >= 0 && !AUDIO::HAS_SOUND_FINISHED(soundId) && !bypass)
	{
		return;
	}

	if (soundId == -1)
	{
		soundId = AUDIO::GET_SOUND_ID();
	}

	AUDIO::PLAY_SOUND_FRONTEND(soundId, soundName, GetSoundGroup(), true);
}

bool TIMERS::ProcessIntermissionTimer()
{
	if (!Intermission::Started)
	{
		if (SURVIVAL::SurvivalData::CurrentWave == 0)
		{
			PlaySound("Survival_Failed", true);
		}
		else
		{
			PlaySound("Round_Passed", true);
		}
		
		Intermission::Started = true;
		Intermission::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		Intermission::CurrentTime = MISC::GET_GAME_TIMER();
		Intermission::timeLeft = (Data::intermissionDuration / 1000) - ((Intermission::CurrentTime - Intermission::StartTime) / 1000);

		if (Intermission::CurrentTime - Intermission::StartTime >= Data::intermissionDuration)
		{
			PlaySound("Countdown_Tick_Last", true);

			return true;
		}

		if (Intermission::timeLeft < 9 && Intermission::CurrentTime - lastSoundTime >= 1000)
		{
			lastSoundTime = Intermission::CurrentTime;
			PlaySound("Countdown_Tick", true);
		}

		return false;
	}
}

bool TIMERS::ProcessLeavingZoneTimer()
{
	if (!LeavingZone::Started)
	{
		LeavingZone::Started = true;
		LeavingZone::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		LeavingZone::CurrentTime = MISC::GET_GAME_TIMER();
		char subtitle[300];
		strcpy_s(subtitle, "Leaving survival in ~r~");
		strcat_s(subtitle, std::to_string(20 - ((LeavingZone::CurrentTime - LeavingZone::StartTime) / 1000)).c_str());
		strcat_s(subtitle, "~w~ seconds");
		SCREEN::ShowSubtitle(subtitle, 500);

		if (LeavingZone::CurrentTime - LeavingZone::StartTime >= 20000)
		{
			LeavingZone::Started = false;
			return true;
		}

		if (soundId >= 0 && AUDIO::HAS_SOUND_FINISHED(soundId))
		{
			AUDIO::PLAY_SOUND_FRONTEND(soundId, "Out_of_Bounds", "MP_MISSION_COUNTDOWN_SOUNDSET", false);
		}

		return false;
	}
}

bool TIMERS::ProcessDogTimer()
{
	if (!DogTimer::Started)
	{
		DogTimer::Started = true;
		DogTimer::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		DogTimer::CurrentTime = MISC::GET_GAME_TIMER();

		if (DogTimer::CurrentTime - DogTimer::StartTime >= 30000)
		{
			return true;
		}

		return false;
	}
}

void TIMERS::RestartDogTimer()
{
	DogTimer::Started = false;
}

bool TIMERS::ProcessSpawnMoreTimer()
{
	if (!SpawnEnemy::Started)
	{
		SpawnEnemy::Started = true;
		SpawnEnemy::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnEnemy::CurrentTime = MISC::GET_GAME_TIMER();
		if (SpawnEnemy::CurrentTime - SpawnEnemy::StartTime >= 1500)
		{
			return true;
		}
		return false;
	}
}

bool TIMERS::ProcessAircraftTimer()
{
	if (!SpawnAircraft::Started)
	{
		SpawnAircraft::Started = true;
		SpawnAircraft::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnAircraft::CurrentTime = MISC::GET_GAME_TIMER();
		if (SpawnAircraft::CurrentTime - SpawnAircraft::StartTime >= 25000)
		{
			return true;
		}
		return false;
	}
}

bool TIMERS::ProcessVehicleTimer()
{
	if (!SpawnVehicle::Started)
	{
		SpawnVehicle::Started = true;
		SpawnVehicle::StartTime = MISC::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnVehicle::CurrentTime = MISC::GET_GAME_TIMER();
		if (SpawnVehicle::CurrentTime - SpawnVehicle::StartTime >= 15000)
		{
			return true;
		}
		return false;
	}
}

void TIMERS::RestartAircraftTimer()
{
	SpawnAircraft::Started = false;
}

void TIMERS::RestartEnemyTimer()
{
	SpawnEnemy::Started = false;
}

void TIMERS::RestartVehicleTimer()
{
	SpawnVehicle::Started = false;
}
