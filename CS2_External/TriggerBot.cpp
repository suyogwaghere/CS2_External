#include "TriggerBot.h"
#include "Head.h"
#include "Game.h"
#include <Windows.h>
#include <array>

void TriggerBot::Run(const CEntity& LocalEntity)
{
	DWORD dw_LocalPlayer = 0x187AC48;
	DWORD dwEntityList = 0x178D8E8;
	DWORD m_iItemDefinitionIndex = 0x1BA;
	DWORD m_hActiveWeapon = 0x48;
	DWORD m_bIsScoped = 0x1388;
	DWORD m_pClippingWeapon = 0x1290;
	DWORD teamNum = 0x3BF;
	DWORD m_bSpotted = 0x8;
	
	Memory::Memory CS_Mem = { "cs2.exe" };
	uintptr_t client = CS_Mem.Get_Module("client.dll");
	const auto PlayerBase = CS_Mem.Read<uintptr_t>(client + dw_LocalPlayer);
	
	DWORD uHandle = 0;
	if (!ProcessMgr.ReadMemory<DWORD>(LocalEntity.Pawn.Address + Offset::Pawn.iIDEntIndex, uHandle))
		return;
	if (uHandle == -1)
		return;

	DWORD64 ListEntry = 0;
	ListEntry = ProcessMgr.TraceAddress(gGame.GetEntityListAddress(), { 0x8 * (uHandle >> 9) + 0x10,0x0 });
	if (ListEntry == 0)
		return;

	DWORD64 PawnAddress = 0;
	if (!ProcessMgr.ReadMemory<DWORD64>(ListEntry + 0x78 * (uHandle & 0x1FF), PawnAddress))
		return;

	CEntity Entity;
	if (!Entity.UpdatePawn(PawnAddress))
		return;

	bool AllowShoot = false;

	if (MenuConfig::TeamCheck)
		AllowShoot = LocalEntity.Pawn.TeamID != Entity.Pawn.TeamID && Entity.Pawn.Health > 0;
	else
		AllowShoot = Entity.Pawn.Health > 0;

	if (AllowShoot)
	{
		static std::chrono::time_point LastTimePoint = std::chrono::steady_clock::now();
		auto CurTimePoint = std::chrono::steady_clock::now();
		if (CurTimePoint - LastTimePoint >= std::chrono::milliseconds(TriggerDelay))
		{	 
			const auto IsScoped = CS_Mem.Read<bool>(PlayerBase + m_bIsScoped);

			const auto ClippigWeaponEnemy = CS_Mem.Read<uint64_t>(PlayerBase + m_pClippingWeapon);
			const auto weapon = CS_Mem.Read<uint16_t>(ClippigWeaponEnemy + m_iItemDefinitionIndex + 0x50 + 0x1040);
			const int WeaponId = std::stoi(std::to_string(weapon), nullptr, 16);
			//std::cout << "[Weapon] WeaponId:" << WeaponId << std::endl;

			if (weapon == 9 || weapon == 40)
			{
				if (IsScoped)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					Sleep(100);
					mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
					Sleep(250);
					mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
				}
			}

			if (weapon != 42)
			{ 
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}

			 LastTimePoint = CurTimePoint;
		}
	}





		
 
}