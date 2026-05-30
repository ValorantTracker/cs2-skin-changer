#pragma once
#include "dwEntityListManager.h"

inline uintptr_t GetActiveWeapon_Standard(uintptr_t pPawn)
{
	const uintptr_t pWeaponServices = mem.Read<uintptr_t>(pPawn + Offsets::m_pWeaponServices);
	if (!pWeaponServices) return 0;

	const uint32_t handle = mem.Read<uint32_t>(pWeaponServices + Offsets::m_hActiveWeapon);
	return GetEntityByHandle(handle);
}

inline std::vector<uintptr_t> GetWeapons_Standard(uintptr_t pPawn)
{
	const uintptr_t pWeaponServices = mem.Read<uintptr_t>(pPawn + Offsets::m_pWeaponServices);
	if (!pWeaponServices) return {};

    // CNetworkUtlVectorBase structure:
    // 0x0000 m_nSize (int32)
    // 0x0008 m_pElements (T*)
	const uint32_t weaponCount = mem.Read<uint32_t>(pWeaponServices + Offsets::m_hMyWeapons);
	const uintptr_t weaponPtr = mem.Read<uintptr_t>(pWeaponServices + Offsets::m_hMyWeapons + 0x8);

	std::vector<uintptr_t> weapons;
    if (weaponCount > 0 && weaponCount < 64 && weaponPtr)
    {
        for (uint32_t i = 0; i < weaponCount; i++)
        {
            const uint32_t handle = mem.Read<uint32_t>(weaponPtr + (sizeof(uint32_t) * i));
            const uintptr_t weapon = GetEntityByHandle(handle);
            if (weapon)
                weapons.push_back(weapon);
        }
    }

	return weapons;
}