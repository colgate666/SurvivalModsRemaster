#pragma once

#include <types.h>
#include "Calculations.hpp"
#include "SurvivalTypes.hpp"
#include "natives.h"
#include "enums.h"

namespace JESUS
{
	class Jesus
	{
	public:
		Ped ped;
		Enemy targetPed;
		bool movingToPed;
		bool revivingPed;
		bool waiting = true;

		explicit Jesus(Ped handle);
		Jesus();
		void SetHandle(Ped handle);
		void SetTarget(Enemy target);
		void MoveToTarget();
		void StartReviving();
		void ReviveTarget();
		void StartWaiting();
		bool HasTarget() const;
		bool IsInRange() const;
		bool CanRevive();

	private:
		bool animTimerStarted = false;
		int animTimerStartTime = 0;
		int animTimerCurrentTime = 0;
	};
}
