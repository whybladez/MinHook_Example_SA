#define _CRT_SECURE_NO_WARNINGS

#include "main.h"
#include <iostream>
#include <string>

bool state = false;
bool waitWASD = false;
bool waitUnWASD = false;
bool lkmpkm = false;
bool canpress = true;
int wasdTick = 0;

nsASI::getASIPlugin::CWeaponInfo* __cdecl MH_CWeaponInfo__GetWeaponInfo(nsASI::getASIPlugin::eWeaponType WeaponType, unsigned char ucSkill)
{
	nsASI::getASIPlugin::CWeaponInfo* pCWeaponInfo = nsASI::getASIPlugin::pCWeaponInfo__GetWeaponInfo_t(WeaponType, ucSkill);
	if (WeaponType != nsASI::getASIPlugin::eWeaponType::WEAPON_DESERT_EAGLE)
		state = false;

	if (WeaponType == nsASI::getASIPlugin::eWeaponType::WEAPON_DESERT_EAGLE)
	{
		state = true;
	} return pCWeaponInfo;


}

DWORD WINAPI clickThread(LPVOID lpParam)
{
	while (true)
	{
		if (state)
		{
			if (GetAsyncKeyState(0x01) < 0 && GetAsyncKeyState(0x02) < 0)
				lkmpkm = true;
			else if (!GetAsyncKeyState(0x01) || !GetAsyncKeyState(0x02))
				lkmpkm = false;

			if (lkmpkm)
			{

				std::cout << "LKM + PKM" << std::endl;
				// ПКМ ЛКМ
				// Ждем W/A/S/D
				// Ждем отпуска W/A/S/D
				// Жмем C через 80 мс

				waitWASD = true;

				while (waitWASD)
				{
					if (GetAsyncKeyState(0x41) < 0)
						waitWASD = false;

					if (GetAsyncKeyState(0x57) < 0)
						waitWASD = false;

					if (GetAsyncKeyState(0x53) < 0)
						waitWASD = false;

					if (GetAsyncKeyState(0x44) < 0)
						waitWASD = false;
				}

				std::cout << "Нажата WASD, ждем отпуск" << std::endl;
				waitUnWASD = true;

				while (waitUnWASD)
				{
					wasdTick++;

					std::cout << "Tick: " << wasdTick << std::endl;
					if (wasdTick >= 300)
					{
						std::cout << "Tickrate was reset! " << std::endl;
						waitUnWASD = false;
						wasdTick = 0;
						canpress = false;

					}

					if (!GetAsyncKeyState(0x41) && !GetAsyncKeyState(0x57) && !GetAsyncKeyState(0x53) && !GetAsyncKeyState(0x44))
					{
						wasdTick = 0;
						std::cout << "WASD ok! Press C.." << std::endl;
						waitUnWASD = false;
						canpress = true;
					}

					Sleep(1);
				}

				if (canpress)
				{
					INPUT ip;
					ip.type = INPUT_KEYBOARD;
					ip.ki.wScan = 0; // hardware scan code for key
					ip.ki.time = 0;
					ip.ki.dwExtraInfo = 0;

					ip.ki.wVk = 0x43; // virtual-key code for the "c" key
					ip.ki.dwFlags = 0; // 0 for key press
					SendInput(1, &ip, sizeof(INPUT));

					Sleep(50);

					ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
					SendInput(1, &ip, sizeof(INPUT));

				}

				canpress = true;
			}
		}
	}
}

BOOL CALLBACK DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ PVOID lpvReserved)
{
	if (hinstDLL)
	{
		if (fdwReason == DLL_PROCESS_ATTACH)
		{
			//AllocConsole();
			//freopen("CONOUT$", "w", stdout);

			if (MH_Initialize() == MH_OK)
			{
				void* pGetWeaponInfo = reinterpret_cast<void*>(nsASI::eSAFuncs::eFUNC_CWeaponInfo__GetWeaponInfo);
				if (pGetWeaponInfo != nullptr)
				{
					if (MH_CreateHook(pGetWeaponInfo, &MH_CWeaponInfo__GetWeaponInfo, reinterpret_cast<void**>(&nsASI::getASIPlugin::pCWeaponInfo__GetWeaponInfo_t)) == MH_OK) MH_EnableHook(pGetWeaponInfo);
					CreateThread(NULL, 0, clickThread, 0, 0, 0);
				}
			}
		}
		else if (fdwReason == DLL_PROCESS_DETACH) MH_Uninitialize();
	} return TRUE;
}