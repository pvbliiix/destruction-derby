#include <windows.h>
#include <TlHelp32.h>
#include <thread>
#include <iostream>
#include "offsets.hpp"

DWORD clientBase;

bool triggerOn = false;
bool whOn = false;
bool noFlashOn = false;

void wallhack()
{
	while (true)
	{
		Sleep(10);
		if (!whOn)
			continue;

		DWORD glowObj = *(DWORD*)(clientBase + dwGlowObjectManager);
		DWORD playerBase = *(DWORD*)(clientBase + dwLocalPlayer);
		int* myTeam =  (int*)(playerBase + m_iTeamNum);

		for (int x = 0; x < 32; x++)
		{
			DWORD player = *(DWORD*)(clientBase + dwEntityList + x * 0x10);
			if (player == 0)
				continue;

			bool* dormant = (bool*)(player + 0xED);
			if (*dormant)
				continue;

			int* team = (int*)(player + m_iTeamNum);
			if (*team != 2 && *team != 3)
				continue;

			DWORD currentGlowIndex = *(DWORD*)(player + m_iGlowIndex);

			if (*team != *myTeam)
			{
				float* glowR = (float*)(glowObj + currentGlowIndex * 0x38 + 0x4);
				float* glowG = (float*)(glowObj + currentGlowIndex * 0x38 + 0x8);
				float* glowB = (float*)(glowObj + currentGlowIndex * 0x38 + 0xC);
				float* glowA = (float*)(glowObj + currentGlowIndex * 0x38 + 0x10);
				bool* glowOccluded = (bool*)(glowObj + currentGlowIndex * 0x38 + 0x24);
				bool* glowUnoccluded = (bool*)(glowObj + currentGlowIndex * 0x38 + 0x25);

				*glowR = 255;
				*glowG = 0;
				*glowB = 0;
				*glowA = 255;
				*glowOccluded = true;
				*glowUnoccluded = false;
			}
			else
			{
				float* glowR = (float*)(glowObj + currentGlowIndex * 0x38 + 0x4);
				float* glowG = (float*)(glowObj + currentGlowIndex * 0x38 + 0x8);
				float* glowB = (float*)(glowObj + currentGlowIndex * 0x38 + 0xC);
				float* glowA = (float*)(glowObj + currentGlowIndex * 0x38 + 0x10);
				bool* glowOccluded = (bool*)(glowObj + currentGlowIndex * 0x38 + 0x24);
				bool* glowUnoccluded = (bool*)(glowObj + currentGlowIndex * 0x38 + 0x25);

				*glowR = 0;
				*glowG = 255;
				*glowB = 0;
				*glowA = 255;
				*glowOccluded = true;
				*glowUnoccluded = false;
			}
		}
	}
}

void noFlash()
{
	while (true)
	{
		if (!noFlashOn)
			continue;

		DWORD LocalPlayer = *(DWORD*)(clientBase + dwLocalPlayer);
		int* flashDuration = (int*)(LocalPlayer + m_flFlashDuration);

		if (*flashDuration > 0)
		{
			*flashDuration = 0;
		}
	}
}

void trigger()
{
	while (true)
	{
		if (!triggerOn)
			continue;

		DWORD playerbase = *(DWORD*)(clientBase + dwLocalPlayer);
		int *playerteam = (int*)(playerbase + m_iTeamNum);

		DWORD iCrossEntityNum = *(DWORD*)(playerbase + m_iCrosshairId);
		DWORD pEntity = *(DWORD*)(clientBase + dwEntityList + (iCrossEntityNum - 1) * 0x10);
		int *iTeamNum = (int*)(pEntity + m_iTeamNum);
		int* shoot = (int*)(clientBase + dwForceAttack);

		if ((*iTeamNum != *playerteam) && (*iTeamNum != 0))
		{
			*shoot = 5;
			Sleep(20);
			*shoot = 4;
		}
	}
}

DWORD WINAPI HackThread(HMODULE hMod)
{
	clientBase = (DWORD)GetModuleHandle(L"client.dll");

	std::thread triggerThread(trigger);
	std::thread noFlashThread(noFlash);
	std::thread whThread(wallhack);

	while (true)
	{
		if (GetAsyncKeyState(VK_F9))
		{
			triggerOn = !triggerOn;

			Sleep(300);
		}
		if (GetAsyncKeyState(VK_F8))
		{
			noFlashOn = !noFlashOn;

			Sleep(300);
		}
		if (GetAsyncKeyState(VK_F7))
		{
			whOn = !whOn;

			Sleep(300);
		}
	}
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason_for_call, LPVOID lpvReserved)
{
	if (dwReason_for_call == DLL_PROCESS_ATTACH)
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hMod, 0, nullptr));
	}
	return 1;
}
