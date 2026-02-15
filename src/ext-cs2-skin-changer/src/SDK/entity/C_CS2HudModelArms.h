#include "dwEntityListManager.h"
#include "../../../ext/offsets.h"

#pragma once

uintptr_t GetHudArms()
{
    return GetEntityByHandle(mem.Read<uint32_t>(GetLocalPlayer() + Offsets::m_hHudModelArms));
}

uintptr_t GetHudWeapon(const uintptr_t weapon)
{
    const auto& armsBase = GetHudArms();
    const auto& armsNode = mem.Read<uintptr_t>(armsBase + Offsets::m_pGameSceneNode);
    for (uintptr_t viewModel = mem.Read<uintptr_t>(armsNode + Offsets::m_pChild); viewModel; viewModel = mem.Read<uintptr_t>(viewModel + Offsets::m_pNextSibling))
    {
        if (!viewModel || !mem.Read<uintptr_t>(viewModel + Offsets::m_pOwner))
            continue;

        if (GetEntityByHandle(mem.Read<uint32_t>(mem.Read<uintptr_t>(viewModel + Offsets::m_pOwner) + Offsets::m_hOwnerEntity)) != weapon)
            continue;

        return mem.Read<uintptr_t>(viewModel + Offsets::m_pOwner);
    }

    return GetHudWeapon(weapon);
}

//uintptr_t GetHudWeapon(const uintptr_t& weapon)
//{
//	uintptr_t hudWeapon = NULL;
//
//	while (!hudWeapon) { hudWeapon = GetEntityByHandle(mem.Read<uint16_t>(weapon + Offsets::m_hViewModelWeapon)); }
//
//	return hudWeapon;
//}

//std::array<uintptr_t, 8> GetAWeapons(uintptr_t LocalPlayer)
//{
//    std::array<uintptr_t, 8> weapons{};
//    int idx = 0;
//
//    const uintptr_t LocalNode = mem->Read<uintptr_t>(LocalPlayer + Offsets::m_pGameSceneNode);
//
//    uintptr_t weapon = mem->Read<uintptr_t>(LocalNode + Offsets::m_pChild);
//    while (weapon && idx < weapons.size())
//    {
//        uintptr_t weaponEntity = mem->Read<uintptr_t>(weapon + Offsets::m_pOwner);
//
//        if (weaponEntity != 0)
//            weapons[idx++] = weaponEntity;
//
//        weapon = mem->Read<uintptr_t>(weapon + Offsets::m_pNextSibling);
//    }
//
//    return weapons;
//}

static const uintptr_t pActiveHud = mem.Read<uintptr_t>(mem.ResolveRelativeAddress(mem.SigScan(L"client.dll", "48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? 33 D2")) + 0x8);

inline uintptr_t GetActiveHudWeapon()
{
    return mem.Read<uintptr_t>(pActiveHud);
}