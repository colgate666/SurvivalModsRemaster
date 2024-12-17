#include "pch.h"
#include "Intermission.hpp"

bool INTERMISSION::Data::Active;

void INTERMISSION::Start()
{
    GRAPHICS::ANIMPOSTFX_PLAY((char*)"SuccessNeutral", 0, false);
    Data::Active = true;

    if (!SURVIVAL::SurvivalData::timed)
    {
        if (SURVIVAL::SurvivalData::CurrentWave == 0)
            UIScript::Data::scaleformType = UIScript::ScaleformType::SURVIVAL_START;
        else
            UIScript::Data::scaleformType = UIScript::ScaleformType::WAVE_SURVIVED;
    }
    else
    {
        if (SURVIVAL::SurvivalData::CurrentWave == 0)
            UIScript::Data::scaleformType = UIScript::ScaleformType::SURVIVAL_START;
        else if (SURVIVAL::SurvivalData::CurrentWave < 10)
            UIScript::Data::scaleformType = UIScript::ScaleformType::DIFFICULTY_INC;
        else
            UIScript::Data::scaleformType = UIScript::ScaleformType::PICKUPS_REGEN;
    }

    UIScript::Data::showScaleform = true;

    if (SURVIVAL::SurvivalData::CurrentWave > 0 && !SURVIVAL::SurvivalData::timed)
    {
        MUSIC::LowIntensityTrack();
    }
    else if (SURVIVAL::SurvivalData::CurrentWave == 9 && !SURVIVAL::SurvivalData::InfiniteWaves && !SURVIVAL::SurvivalData::timed)
    {
        MUSIC::StopTrack();
        MUSIC::FinalWaveMusic();
    }
}

void INTERMISSION::Process()
{
    if (!TIMERS::ProcessIntermissionTimer() && !SURVIVAL::SurvivalData::timed)
    {
        return;
    }
    else if (SURVIVAL::SurvivalData::CurrentWave == 0 && SURVIVAL::SurvivalData::timed && !TIMERS::ProcessIntermissionTimer())
    {
        return;
    }

    if (!SURVIVAL::SurvivalData::timed || (SURVIVAL::SurvivalData::timed && SURVIVAL::SurvivalData::CurrentWave == 0))
    {
        TIMERS::RestartIntermissionTimer();
    }

    if (SURVIVAL::SurvivalData::CurrentWave > 0)
    {
        PICKUPS::Regenerate();
    }

    TIMERS::RestartSpawnTimers();
    SURVIVAL::IncrementWave();
    ENEMIES::ResetCounters();
    SURVIVAL::SurvivalData::MaxWaveSize = 10 + (3 * SURVIVAL::SurvivalData::CurrentWave);

    if (SURVIVAL::SurvivalData::hardcore)
        SURVIVAL::SurvivalData::MaxWaveSize += 8;

    SURVIVAL::UpdateModels(SURVIVAL::SurvivalData::CurrentWave);

    if (SURVIVAL::SurvivalData::hardcore)
    {
        //MUSIC::HighIntensityTrack(10);
        SURVIVAL::SurvivalData::MaxVehicles = 5;
        SURVIVAL::SurvivalData::MaxAircraft = 3;
    }
    else
    {
        /*if (SURVIVAL::SurvivalData::CurrentWave <= 6)
        {
            MUSIC::MidIntensityTrack(SURVIVAL::SurvivalData::CurrentWave);
        }
        else if (SURVIVAL::SurvivalData::CurrentWave < 10 || SURVIVAL::SurvivalData::InfiniteWaves || SURVIVAL::SurvivalData::timed)
        {
            MUSIC::HighIntensityTrack(SURVIVAL::SurvivalData::CurrentWave);
        }*/

        switch (SURVIVAL::SurvivalData::CurrentWave)
        {
            case 3:
            {
                SURVIVAL::SurvivalData::MaxVehicles = SURVIVAL::SurvivalData::timed ? 1 : 2;
                break;
            }
            case 5:
            {
                SURVIVAL::SurvivalData::MaxAircraft = 1;
                SURVIVAL::SurvivalData::MaxVehicles = SURVIVAL::SurvivalData::timed ? 2 : 3;
                break;
            }
            case 7:
            {
                SURVIVAL::SurvivalData::MaxAircraft = 2;
                SURVIVAL::SurvivalData::MaxVehicles = SURVIVAL::SurvivalData::timed ? 3 : 4;
                break;
            }
            case 9:
            {
                SURVIVAL::SurvivalData::MaxAircraft = SURVIVAL::SurvivalData::timed ? 2 : 3;
                SURVIVAL::SurvivalData::MaxVehicles = SURVIVAL::SurvivalData::timed ? 3 : 5;
                break;
            }
        }
    }

    INTERMISSION::Data::Active = false;
}
