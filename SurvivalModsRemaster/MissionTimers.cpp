#include "pch.h"
#include "MissionTimers.hpp"

int TIMERS::TriggerPed::CurrentTime;
int TIMERS::TriggerPed::StartTime;
bool TIMERS::TriggerPed::Started;
int TIMERS::InterMission::CurrentTime;
int TIMERS::InterMission::StartTime;
bool TIMERS::InterMission::Started;
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
int TIMERS::DogTimer::CurrentTime;
int TIMERS::DogTimer::StartTime;
bool TIMERS::DogTimer::Started;

bool TIMERS::ProcessTimedSurvivalTimer(int endTime)
{
	if (!TimedSurvival::Started)
	{
		TimedSurvival::Started = true;
		TimedSurvival::StartTime = GAMEPLAY::GET_GAME_TIMER();
		TimedSurvival::InterWaveTime = TimedSurvival::StartTime;
		return false;
	}
	else
	{
		TimedSurvival::CurrentTime = GAMEPLAY::GET_GAME_TIMER();

		if (TimedSurvival::CurrentTime - TimedSurvival::InterWaveTime >= 120000)
		{
			TimedSurvival::InterWaveTime = GAMEPLAY::GET_GAME_TIMER();
			INTERMISSION::Start();
			INTERMISSION::Process();

			if (SURVIVAL::SurvivalData::CurrentWave < 10)
			{
				SCREEN::ShowSubtitle("~g~Difficulty increased!", 8000);
			}
			else
			{
				SCREEN::ShowSubtitle("~g~Pickups regenerated.", 8000);
			}
		}

		char helpText[300];
		strcpy_s(helpText, "Remaining time: ");
		strcat_s(helpText, std::to_string((endTime / 1000) - ((TimedSurvival::CurrentTime - TimedSurvival::StartTime) / 1000)).c_str());
		strcat_s(helpText, " seconds");

		SCREEN::ShowHelpTextThisFrame(helpText, false);
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
		TriggerPed::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		TriggerPed::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
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
	InterMission::Started = false;
	SpawnAircraft::Started = false;
	LeavingZone::Started = false;
	SpawnEnemy::Started = false;
	SpawnVehicle::Started = false;
	DogTimer::Started = false;
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
	InterMission::Started = false;
}

bool TIMERS::ProcessIntermissionTimer()
{
	if (!InterMission::Started)
	{
		InterMission::Started = true;
		InterMission::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		InterMission::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
		char helpText[300];
		int timeLeft = (Data::intermissionDuration / 1000) - ((InterMission::CurrentTime - InterMission::StartTime) / 1000);
		const char* colorModifier;

		if (timeLeft > 20 / 1.5f)
		{
			colorModifier = "~g~";
		}
		else if (timeLeft > 20 / 3)
		{
			colorModifier = "~y~";
		}
		else
		{
			colorModifier = "~r~";
		}

		if (!SURVIVAL::SurvivalData::timed)
		{
			strcpy_s(helpText, "Wave ");
			strcat_s(helpText, std::to_string(SURVIVAL::SurvivalData::CurrentWave + 1).c_str());
		}
		else
		{
			strcpy_s(helpText, "Survival ");
		}

		strcat_s(helpText, " starting in ");
		strcat_s(helpText, colorModifier);
		strcat_s(helpText, std::to_string(timeLeft).c_str());
		strcat_s(helpText, "~w~");
		strcat_s(helpText, " seconds");
		SCREEN::ShowHelpTextThisFrame(helpText, false);

		if (InterMission::CurrentTime - InterMission::StartTime >= Data::intermissionDuration)
		{
			return true;
		}

		return false;
	}
}

bool TIMERS::ProcessLeavingZoneTimer()
{
	if (!LeavingZone::Started)
	{
		LeavingZone::Started = true;
		LeavingZone::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		LeavingZone::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
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

		return false;
	}
}

bool TIMERS::ProcessDogTimer()
{
	if (!DogTimer::Started)
	{
		DogTimer::Started = true;
		DogTimer::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		DogTimer::CurrentTime = GAMEPLAY::GET_GAME_TIMER();

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
		SpawnEnemy::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnEnemy::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
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
		SpawnAircraft::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnAircraft::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
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
		SpawnVehicle::StartTime = GAMEPLAY::GET_GAME_TIMER();
		return false;
	}
	else
	{
		SpawnVehicle::CurrentTime = GAMEPLAY::GET_GAME_TIMER();
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