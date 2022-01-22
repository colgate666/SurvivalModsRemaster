#include "pch.h"
#include "Enemies.hpp"

int ENEMIES::EnemiesData::currentAircraft;
int ENEMIES::EnemiesData::currentVehicles;
bool ENEMIES::EnemiesData::canSpawnMore;
bool ENEMIES::EnemiesData::limitReached;
int ENEMIES::EnemiesData::currentWaveSize;
int ENEMIES::EnemiesData::kills;
bool ENEMIES::EnemiesData::jugSpawned;
int ENEMIES::EnemiesData::currentDogCount;
bool ENEMIES::EnemiesData::dogLimitReached = false;
std::vector<Ped> ENEMIES::EnemiesData::footEnemies;
std::vector<Vehicle> ENEMIES::EnemiesData::enemyVehicles;
std::vector<Ped> ENEMIES::EnemiesData::deadEnemies;
JESUS::Jesus ENEMIES::EnemiesData::enemyJesus;
bool ENEMIES::EnemiesData::jesusSpawned;
Ped ENEMIES::EnemiesData::enemyJuggernaut;

std::vector<Hash> ENEMIES::EnemiesData::alienWeapons =
{
    0x476BF155,
    0xB62D1F67
};

void ENEMIES::ResetCounters()
{
    EnemiesData::currentAircraft = 0;
    EnemiesData::currentVehicles = 0;
    EnemiesData::canSpawnMore = true;
    EnemiesData::limitReached = false;
    EnemiesData::jesusSpawned = false;
    EnemiesData::currentDogCount = 0;
    EnemiesData::dogLimitReached = false;
    EnemiesData::jugSpawned = false;
    EnemiesData::currentWaveSize = 0;
    EnemiesData::kills = 0;
    EnemiesData::enemyJuggernaut = 0;
}

void ENEMIES::ClearVectors()
{
    for (Ped enemy : EnemiesData::footEnemies)
    {
        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&enemy);
    }

    for (Vehicle vehicle : EnemiesData::enemyVehicles)
    {
        ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&vehicle);
    }

    for (Ped deadEnemy : EnemiesData::deadEnemies)
    {
        ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&deadEnemy);
    }

    EnemiesData::footEnemies.clear();
    EnemiesData::enemyVehicles.clear();
    EnemiesData::deadEnemies.clear();
}

int ENEMIES::GetKillTime(Ped ped)
{
    Entity killer = PED::_GET_PED_KILLER(ped);

    if (killer != PLAYER::PLAYER_PED_ID())
    {
        return 0;
    }

    Hash model = ENTITY::GET_ENTITY_MODEL(ped);

    switch (model)
    {
    case 0x90EF5134:
        SCREEN::ShowNotification("Juggernaut: ~o~+20 seconds");
        return 15000;
    case 0xCE2CB751:
        SCREEN::ShowNotification("Jesus: ~o~+15 seconds");
        return 15000;
    case 0xAD340F5A:
    case 0x61D4C771:
        if (PED::GET_PED_MAX_HEALTH(ped) == 1250)
        {
            SCREEN::ShowNotification("Ragemode Sasquatch: ~o~+15 seconds");
            return 15000;
        }
        else
        {
            break;
        }
    case 0x9563221D:
        SCREEN::ShowNotification("Rottweiler: ~o~+8 seconds");
        return 8000;
    default:
        break;
    }

    if (FIRE::IS_ENTITY_ON_FIRE(ped))
    {
        SCREEN::ShowNotification("Burn!: ~r~+4 seconds");
        return 4000;
    }

    Any lastDamagedBone;
    PED::GET_PED_LAST_DAMAGE_BONE(ped, &lastDamagedBone);

    switch (lastDamagedBone)
    {
        case eBone::SKEL_Head:
            SCREEN::ShowNotification("Headshot: ~g~+5 seconds");
            return 5000;
        case eBone::SKEL_Spine_Root:
        case eBone::SKEL_Spine0:
        case eBone::SKEL_Spine1:
        case eBone::SKEL_Spine2:
        case eBone::SKEL_Spine3:
            SCREEN::ShowNotification("Torso: ~y~+2.5 seconds");
            return 2500;
        case eBone::SKEL_L_UpperArm:
        case eBone::SKEL_R_UpperArm:
        case eBone::SKEL_L_Forearm:
        case eBone::SKEL_R_Forearm:
        case eBone::SKEL_L_Thigh:
        case eBone::SKEL_R_Thigh:
        case eBone::SKEL_L_Calf:
        case eBone::SKEL_R_Calf:
            SCREEN::ShowNotification("Limb: ~r~+1.25 seconds");
            return 1250;
        default:
            SCREEN::ShowNotification("Other: ~c~+1 second");
            return 1000;
    }
}

void ENEMIES::RemoveDeadEnemies()
{
    for (size_t i = 0; i < EnemiesData::footEnemies.size(); i++)
    {
        if (!PED::IS_PED_DEAD_OR_DYING(EnemiesData::footEnemies.at(i), 1))
        {
            if (PED::IS_PED_FLEEING(EnemiesData::footEnemies.at(i)) && !CALC::IsInRange_2(ENTITY::GET_ENTITY_COORDS(EnemiesData::footEnemies.at(i), true), SURVIVAL::SpawnerData::location, 160))
            {
                ENTITY::SET_ENTITY_HEALTH(EnemiesData::footEnemies.at(i), 0);
            }
            else
            {
                continue;
            }
        }

        if (SURVIVAL::SurvivalData::timed)
        {
            SURVIVAL::SurvivalData::timedTimeLeft += GetKillTime(EnemiesData::footEnemies.at(i));
        }

        if (EnemiesData::jesusSpawned && ENTITY::GET_ENTITY_MODEL(EnemiesData::footEnemies.at(i)) == 0xCE2CB751)
        {
            EnemiesData::jesusSpawned = false;
        }

        if (!PED::CAN_PED_RAGDOLL(EnemiesData::footEnemies.at(i)))
        {
            PED::SET_PED_CAN_RAGDOLL(EnemiesData::footEnemies.at(i), true);
        }

        Blip blip = UI::GET_BLIP_FROM_ENTITY(EnemiesData::footEnemies.at(i));
        if (UI::DOES_BLIP_EXIST(blip))
        {
            UI::REMOVE_BLIP(&blip);
        }

        if (!EnemiesData::jesusSpawned)
        {
            if (EnemiesData::footEnemies.at(i) == EnemiesData::enemyJuggernaut)
            {
                EnemiesData::enemyJuggernaut = 0;
            }

            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&EnemiesData::footEnemies.at(i));
            EnemiesData::kills += 1;
        }
        else
        {
            EnemiesData::deadEnemies.push_back(EnemiesData::footEnemies[i]);
        }

        EnemiesData::footEnemies.erase(EnemiesData::footEnemies.begin() + i);
        break;
    }

    if (!EnemiesData::jesusSpawned && !EnemiesData::deadEnemies.empty())
    {
        for (size_t i = 0; i < EnemiesData::deadEnemies.size(); i++)
        {
            if (EnemiesData::footEnemies.at(i) == EnemiesData::enemyJuggernaut)
            {
                EnemiesData::enemyJuggernaut = 0;
            }

            ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&EnemiesData::deadEnemies.at(i));
            EnemiesData::kills += 1;
        }

        EnemiesData::deadEnemies.clear();
    }
}

void ENEMIES::RemoveUnusedVehicles()
{
    bool vehicleDestroyed = false;
    bool driverGone = false;
    int vehiclePassengers = 0;

    for (size_t i = 0; i < EnemiesData::enemyVehicles.size(); i++)
    {
        vehicleDestroyed = ENTITY::IS_ENTITY_DEAD(EnemiesData::enemyVehicles.at(i));
        vehiclePassengers = VEHICLE::GET_VEHICLE_NUMBER_OF_PASSENGERS(EnemiesData::enemyVehicles.at(i));
        driverGone = VEHICLE::IS_VEHICLE_SEAT_FREE(EnemiesData::enemyVehicles.at(i), -1);

        if (vehicleDestroyed || (vehiclePassengers == 0 && driverGone))
        {
            BLIPS::DeleteBlipForEntity(EnemiesData::enemyVehicles.at(i));

            if (vehicleDestroyed && SURVIVAL::SurvivalData::timed)
            {
                SURVIVAL::SurvivalData::timedTimeLeft += 10000;
                SCREEN::ShowNotification("Vehicle: ~p~+10 seconds");
            }

            ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&EnemiesData::enemyVehicles.at(i));
            EnemiesData::enemyVehicles.erase(EnemiesData::enemyVehicles.begin() + i);
            return;
        }
        else if (!driverGone)
        {
            Ped ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(EnemiesData::enemyVehicles.at(i), -1);
            if (PED::IS_PED_DEAD_OR_DYING(ped, 1))
            {
                BLIPS::DeleteBlipForEntity(EnemiesData::enemyVehicles.at(i));
                ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&EnemiesData::enemyVehicles.at(i));
                EnemiesData::enemyVehicles.erase(EnemiesData::enemyVehicles.begin() + i);
                return;
            }
        }
    }
}

std::vector<Hash> ENEMIES::GetWeapons(Hash pedModel)
{
    if (pedModel == 0x64611296)
    {
        return EnemiesData::alienWeapons;
    }

    if (SURVIVAL::SurvivalData::hardcore)
        return SURVIVAL::SpawnerData::strongWeapons;

    switch (SURVIVAL::SurvivalData::CurrentWave)
    {
        case 1:
        case 2:
        case 3:
            return SURVIVAL::SpawnerData::weakWeapons;
        case 4:
        case 5:
        case 6:
        case 7:
            return SURVIVAL::SpawnerData::medWeapons;
        default:
            return SURVIVAL::SpawnerData::strongWeapons;
    }
}

void ENEMIES::InitializeJesus(Ped ped)
{
    EnemiesData::enemyJesus = JESUS::Jesus(ped);
    EnemiesData::jesusSpawned = true;
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
    PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
    PED::SET_PED_CAN_RAGDOLL(ped, false);
    PED::SET_PED_CONFIG_FLAG(ped, 281, false);
    PED::SET_PED_MAX_HEALTH(ped, 1000);
    ENTITY::SET_ENTITY_HEALTH(ped, 1000);
    BLIPS::CreateForEnemyPed(ped, 305, "Jesus Christ");
}

void ENEMIES::InitializeAnimal(Ped ped)
{
    BLIPS::CreateForEnemyPed(ped, 463, "Enemy Animal");
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, GAMEPLAY::GET_HASH_KEY((char*)"COUGAR"));
    AI::TASK_PUT_PED_DIRECTLY_INTO_MELEE(ped, PLAYER::PLAYER_PED_ID(), 0, 0, 0, 0);
    EnemiesData::currentDogCount += 1;

    if (EnemiesData::currentDogCount >= 3)
    {
        EnemiesData::dogLimitReached = true;
    }
}

void ENEMIES::InitializeJuggernaut(Ped ped)
{
    EnemiesData::enemyJuggernaut = ped;
    PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
    PED::SET_PED_CAN_RAGDOLL(ped, false);
    PED::SET_PED_CONFIG_FLAG(ped, 281, false);
    PED::SET_PED_MAX_HEALTH(ped, 1000);
    ENTITY::SET_ENTITY_HEALTH(ped, 1000);
    BLIPS::CreateForEnemyPed(ped, 543, "Enemy Juggernaut");
    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 300 : 150);
    PED::SET_PED_ACCURACY(ped, accuracyModifier * 3);
    WEAPON::GIVE_WEAPON_TO_PED(ped, eWeapon::WeaponMinigun, 9000, true, true);
    PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    AI::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);
}

void ENEMIES::InitializeRageEnemy(Ped ped)
{
    PED::SET_PED_SUFFERS_CRITICAL_HITS(ped, false);
    PED::SET_PED_CAN_RAGDOLL(ped, false);
    PED::SET_PED_CONFIG_FLAG(ped, 281, false);
    PED::SET_PED_MAX_HEALTH(ped, 1250);
    ENTITY::SET_ENTITY_HEALTH(ped, 1250);
    AI::TASK_PUT_PED_DIRECTLY_INTO_MELEE(ped, PLAYER::PLAYER_PED_ID(), 0, -1, 0, 0);
    PED::SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(100);
    BLIPS::CreateForEnemyPed(ped, 671, "Ragemode Sasquatch");
}

void ENEMIES::InitializeEnemy(Ped ped)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(ped);

    if (pedModel == 0xCE2CB751 &&  !EnemiesData::jesusSpawned)
    {
        InitializeJesus(ped);
        return;
    }

    if ((pedModel == 0xAD340F5A || pedModel == 0x61D4C771))
    {
        int rageChance = CALC::RanInt(100, 1);

        if (rageChance <= SURVIVAL::SurvivalData::CurrentWave * 2)
        {
            InitializeRageEnemy(ped);
            return;
        }
    }

    switch (pedModel)
    {
        case 0x585C0B52:
        case 0x3AE4A33B:
        case 0x5CDEF405:
        case 0xEDBC7546:
        case 0x26F067AD:
        case 0x7B8B434B:
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(ped, 7 + (accuracyModifier * 3));
    std::vector<Hash> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    Hash weaponHash = weapons.at(index);
    WEAPON::GIVE_WEAPON_TO_PED(ped, weaponHash, 1000, true, true);
    AI::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);
    BLIPS::CreateForEnemyPed(ped);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void ENEMIES::InitializeEnemyInAircraft(Ped ped, bool passenger)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(ped);

    switch (pedModel)
    {
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(ped, 7 + (accuracyModifier * 3));
    std::vector<DWORD> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    DWORD weaponHash = weapons.at(index);
    WEAPON::GIVE_WEAPON_TO_PED(ped, weaponHash, 1000, true, true);

    if (passenger)
        AI::TASK_SHOOT_AT_ENTITY(ped, PLAYER::PLAYER_PED_ID(), 18000000, eFiringPattern::FiringPatternBurstFire);
    else
        AI::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);

    BLIPS::CreateForEnemyPed(ped);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void ENEMIES::InitializeEnemyInVehicle(Ped ped, bool passenger)
{
    Hash pedModel = ENTITY::GET_ENTITY_MODEL(ped);

    switch (pedModel)
    {
        case 0x5E3DA4A4:
        case 0x15F8700D:
        case 0x616C97B9:
        case 0x72C0CAD2:
        case 0x8D8F1B10:
            PED::SET_PED_CONFIG_FLAG(ped, 155, false);
            PED::SET_PED_CONFIG_FLAG(ped, 42, true);
            PED::SET_PED_CONFIG_FLAG(ped, 301, true);
            break;
        default:
            break;
    }

    if (pedModel == 0x64611296)
    {
        PED::SET_PED_DEFAULT_COMPONENT_VARIATION(ped);
        AUDIO::DISABLE_PED_PAIN_AUDIO(ped, true);
    }

    PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, Data::enemiesRelGroup);
    int accuracyModifier = SURVIVAL::SurvivalData::CurrentWave > 10 ? 10 : SURVIVAL::SurvivalData::CurrentWave;

    if (SURVIVAL::SurvivalData::hardcore)
        accuracyModifier = 10;

    PED::SET_PED_ARMOUR(ped, SURVIVAL::SurvivalData::hardcore ? 150 : SURVIVAL::SurvivalData::CurrentWave * 10);
    PED::SET_PED_ACCURACY(ped, 7 + (accuracyModifier * 3));
    std::vector<Hash> weapons = GetWeapons(pedModel);
    size_t index = CALC::RanInt(weapons.size() - (size_t)1, (size_t)0);
    Hash weaponHash = weapons.at(index);
    WEAPON::GIVE_WEAPON_TO_PED(ped, weaponHash, 1000, true, true);

    if (passenger)
        AI::TASK_VEHICLE_SHOOT_AT_PED(ped, PLAYER::PLAYER_PED_ID(), 40.0f);
    else
        AI::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);

    BLIPS::CreateForEnemyPed(ped);
    PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
    int i = CALC::RanInt(100, 1);

    if (i <= accuracyModifier * 3)
    {
        PED::SET_PED_FIRING_PATTERN(ped, eFiringPattern::FiringPatternFullAuto);
    }
}

void ENEMIES::ProcessJesus()
{
    if (EnemiesData::jesusSpawned)
    {
        if (EnemiesData::deadEnemies.empty() && !EnemiesData::enemyJesus.waiting)
        {
            EnemiesData::enemyJesus.StartWaiting();
        }
        else if (!EnemiesData::deadEnemies.empty())
        {
            if (EnemiesData::enemyJesus.waiting)
            {
                EnemiesData::enemyJesus.SetTarget(EnemiesData::deadEnemies.front());
                EnemiesData::enemyJesus.MoveToTarget();
            }
            else if (EnemiesData::enemyJesus.movingToPed && EnemiesData::enemyJesus.IsInRange())
            {
                EnemiesData::enemyJesus.StartReviving();
            }
            else if (EnemiesData::enemyJesus.revivingPed && EnemiesData::enemyJesus.CanRevive())
            {
                EnemiesData::enemyJesus.ReviveTarget();
                WAIT(1);

                for (int n = 0; n < EnemiesData::deadEnemies.size(); n++)
                {
                    if (EnemiesData::deadEnemies[n] == EnemiesData::enemyJesus.targetPed)
                    {
                        EnemiesData::deadEnemies.erase(EnemiesData::deadEnemies.begin() + n);
                        break;
                    }
                }

                if (EnemiesData::enemyJesus.targetPed == EnemiesData::enemyJuggernaut)
                {
                    InitializeJuggernaut(EnemiesData::enemyJesus.targetPed);
                }
                else if (ENTITY::GET_ENTITY_MODEL(EnemiesData::enemyJesus.targetPed) == 0x9563221D)
                {
                    InitializeAnimal(EnemiesData::enemyJesus.targetPed);
                }
                else
                {
                    InitializeEnemy(EnemiesData::enemyJesus.targetPed);
                }

                EnemiesData::footEnemies.push_back(EnemiesData::enemyJesus.targetPed);
            }
        }
    }
}

void ENEMIES::Process()
{
    if (!MUSIC::MusicData::finalWaveHI && SURVIVAL::SurvivalData::CurrentWave == 10 && !SURVIVAL::SurvivalData::InfiniteWaves && !SURVIVAL::SurvivalData::timed && EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize / 2)
    {
        MUSIC::FinalWaveMusicHighIntensity();
        MUSIC::MusicData::finalWaveHI = true;
    }

    if (EnemiesData::canSpawnMore && !EnemiesData::limitReached)
    {
        if ((SURVIVAL::SurvivalData::CurrentWave >= 7 || SURVIVAL::SurvivalData::hardcore) && SURVIVAL::SpawnerData::hasDogs && !EnemiesData::dogLimitReached)
        {
            if (TIMERS::ProcessDogTimer())
            {
                Ped ped = SURVIVAL::SpawnDog();
                InitializeAnimal(ped);
                EnemiesData::footEnemies.push_back(ped);
                EnemiesData::currentDogCount += 1;

                if (EnemiesData::currentDogCount > 2)
                    EnemiesData::dogLimitReached = true;

                TIMERS::RestartDogTimer();
            }
        }

        if (TIMERS::ProcessSpawnMoreTimer())
        {
            EnemiesData::currentWaveSize += 1;
            Ped ped;

            if ((SURVIVAL::SurvivalData::CurrentWave >= 8 || SURVIVAL::SurvivalData::hardcore) && SURVIVAL::SpawnerData::hasJuggernaut && !EnemiesData::jugSpawned && EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize / 2)
            {
                ped = SURVIVAL::SpawnJuggernaut();
                InitializeJuggernaut(ped);
                EnemiesData::jugSpawned = true;
            }
            else
            {
                ped = SURVIVAL::SpawnEnemy(SURVIVAL::SurvivalData::CurrentWave, !EnemiesData::jesusSpawned);
                InitializeEnemy(ped);
            }

            EnemiesData::footEnemies.push_back(ped);
            TIMERS::RestartEnemyTimer();

            if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
            {
                EnemiesData::canSpawnMore = false;
            }
        }

        if (SURVIVAL::SpawnerData::hasVehicles && (SURVIVAL::SurvivalData::CurrentWave >= 3 || SURVIVAL::SurvivalData::hardcore) && EnemiesData::currentVehicles < SURVIVAL::SurvivalData::MaxVehicles)
        {
            if (TIMERS::ProcessVehicleTimer())
            {
                Vehicle vehicle = SURVIVAL::SpawnVehicle();
                std::vector<Ped> peds = SURVIVAL::SpawnEnemiesInVehicle(vehicle, SURVIVAL::SurvivalData::CurrentWave);

                for (size_t i = 0; i < peds.size(); i++)
                {
                    InitializeEnemyInVehicle(peds.at(i), i != 0);
                    EnemiesData::footEnemies.push_back(peds.at(i));
                }

                BLIPS::CreateForEnemyVehicle(vehicle);
                EnemiesData::enemyVehicles.push_back(vehicle);
                EnemiesData::currentVehicles += 1;
                EnemiesData::currentWaveSize += peds.size();
                TIMERS::RestartVehicleTimer();

                if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
                {
                    EnemiesData::canSpawnMore = false;
                }
            }
        }

        if (SURVIVAL::SpawnerData::hasAircraft && (SURVIVAL::SurvivalData::CurrentWave >= 5 || SURVIVAL::SurvivalData::hardcore) && EnemiesData::currentAircraft < SURVIVAL::SurvivalData::MaxAircraft)
        {
            if (TIMERS::ProcessAircraftTimer())
            {
                Vehicle aircraft = SURVIVAL::SpawnAircraft();
                std::vector<Ped> peds = SURVIVAL::SpawnEnemiesInVehicle(aircraft, SURVIVAL::SurvivalData::CurrentWave);

                for (size_t i = 0; i < peds.size(); i++)
                {
                    InitializeEnemyInAircraft(peds.at(i), i != 0);
                    EnemiesData::footEnemies.push_back(peds.at(i));
                }

                BLIPS::CreateForEnemyVehicle(aircraft);
                EnemiesData::enemyVehicles.push_back(aircraft);

                EnemiesData::currentAircraft += 1;
                EnemiesData::currentWaveSize += peds.size();
                TIMERS::RestartAircraftTimer();

                if (EnemiesData::currentWaveSize >= SURVIVAL::SurvivalData::MaxWaveSize && !SURVIVAL::SurvivalData::timed)
                {
                    EnemiesData::canSpawnMore = false;
                }
            }
        }
    }

    if (SURVIVAL::SurvivalData::timed)
    {
        if (TIMERS::ProcessTimedSurvivalTimer((SURVIVAL::SurvivalData::timedTimeLeft)))
        {
            SURVIVAL::QuitSurvival(true);
            return;
        }
    }

    if (!EnemiesData::footEnemies.empty())
    {
        if (!TIMERS::LeavingZone::Started)
            SCREEN::ShowSubtitle("Kill the ~r~enemies.", 8000);

        RemoveDeadEnemies();
        ProcessJesus();
        MUSIC::Process(EnemiesData::currentWaveSize, SURVIVAL::SurvivalData::MaxWaveSize);
        
        if (!SURVIVAL::SurvivalData::hardcore)
        {
            switch (SURVIVAL::SurvivalData::CurrentWave)
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    EnemiesData::limitReached = SURVIVAL::SurvivalData::timed ? EnemiesData::footEnemies.size() >= 14 : EnemiesData::footEnemies.size() >= 15;
                    break;
                case 6:
                case 7:
                    EnemiesData::limitReached = SURVIVAL::SurvivalData::timed ? EnemiesData::footEnemies.size() >= 13 : EnemiesData::footEnemies.size() >= 14;
                    break;
                default:
                    EnemiesData::limitReached = SURVIVAL::SurvivalData::timed ? EnemiesData::footEnemies.size() >= 12 : EnemiesData::footEnemies.size() >= 13;
                    break;
            }
        }

        return;
    }
    else if (!EnemiesData::canSpawnMore)
    {
        if (SURVIVAL::SurvivalData::InfiniteWaves || SURVIVAL::SurvivalData::CurrentWave < 10)
        {
            SCREEN::ShowSubtitle("", 1);
            INTERMISSION::Start();
        }
        else
        {
            SURVIVAL::CompleteSurvival();
        }
    }
}
