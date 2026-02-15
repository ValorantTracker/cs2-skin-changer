#include "dwEntityListManager.h"

//inline uintptr_t GetActiveWeapon(const uintptr_t& pPawn)
//{
//	const uintptr_t& pWeaponServices = mem.Read<uintptr_t>(pPawn + Offsets::m_pWeaponServices);
//	//std::cout << std::hex << mem->Read<uint32_t>(pWeaponServices + Offsets::m_hActiveWeapon) << std::endl;
//	//std::cout << std::hex << mem->Read<uintptr_t>(pPawn + Offsets::m_pClippingWeapon) << std::endl;
//	return GetEntityByHandle(mem.Read<uint32_t>(pWeaponServices + Offsets::m_hActiveWeapon));
//	//return mem->Read<uintptr_t>(pPawn + Offsets::m_pClippingWeapon);
//}

//inline uintptr_t GetActiveWeapon(const uintptr_t& pPawn)
//{
//	return mem.Read<uintptr_t>(pPawn + Offsets::m_pClippingWeapon);
//}
//
inline std::vector<uintptr_t> GetWeapons(const uintptr_t& pPawn)
{
	const uintptr_t pWeaponServices = mem.Read<uintptr_t>(pPawn + Offsets::m_pWeaponServices);
	const uint64_t& weaponCount = mem.Read<uint64_t>(pWeaponServices + Offsets::m_hMyWeapons);
	const uintptr_t weaponEntry = mem.Read<uintptr_t>(pWeaponServices + Offsets::m_hMyWeapons + sizeof(weaponCount));

	std::vector<uintptr_t> weapons;

	for (uint64_t i = 0; i < weaponCount; i++)
	{
		const uintptr_t weapon = GetEntityByHandle(mem.Read<uint32_t>(weaponEntry + (sizeof(uint32_t) * i)));
		if (!weapon)
			continue;
	
		weapons.push_back(weapon);
	}

	return weapons;
}