#include <windows.h>
#include <TlHelp32.h>
#include <thread>
#include "offsets.h"

DWORD clientBase;

bool triggerOn = false;
bool noFlashOn = false;

void noflash()
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

	while (true)
	{
		if (GetAsyncKeyState(VK_F9))
		{
			triggerOn = !triggerOn;

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
