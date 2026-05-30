#include "../../../ext/mem.h"

#pragma once

namespace EntityConstants {
    constexpr int EntrySize = 0x8;
    constexpr int ListEntryOffset = 0x10;
    constexpr int EntityOffset = 0x70;
    constexpr int HandleMask = 0x7FFF;
    constexpr int HandleShift = 9;
    constexpr int HandleIndexMask = 0x1FF;
}

inline uintptr_t GetEntityList() {
    static uintptr_t clientBase = 0;
    if (!clientBase) clientBase = mem.GetModuleBase(L"client.dll");
    return mem.Read<uintptr_t>(clientBase + Offsets::dwEntityList);
}

inline uintptr_t GetLocalController()
{
    uintptr_t clientBase = mem.GetModuleBase(L"client.dll");
    if (!clientBase) return 0;
    return mem.Read<uintptr_t>(clientBase + Offsets::dwLocalPlayerController);
}

inline uintptr_t GetEntityByHandle(uint32_t handle)
{
    uintptr_t entityList = GetEntityList();
    if (!entityList) return 0;

    const uintptr_t listEntry = mem.Read<uintptr_t>(entityList + EntityConstants::EntrySize * ((handle & EntityConstants::HandleMask) >> EntityConstants::HandleShift) + EntityConstants::ListEntryOffset);
    if (!listEntry) return 0;

    return mem.Read<uintptr_t>(listEntry + EntityConstants::EntityOffset * (handle & EntityConstants::HandleIndexMask));
}

inline uintptr_t GetLocalPlayer()
{
    uintptr_t clientBase = mem.GetModuleBase(L"client.dll");
    if (!clientBase) return 0;
    uintptr_t pawn = mem.Read<uintptr_t>(clientBase + Offsets::dwLocalPlayerPawn);
    if (!pawn) {
        // Try fallback if dwLocalPlayerPawn is 0 (can happen with some offsets)
        uintptr_t controller = GetLocalController();
        if (controller) {
            uint32_t handle = mem.Read<uint32_t>(controller + 0x6BC); // m_hPawn offset from client_dll.hpp
            if (handle) pawn = GetEntityByHandle(handle);
        }
    }
    return pawn;
}

inline uint16_t GetEntityHandle(const uintptr_t ent)
{
    if (!ent) return 0;
    const uintptr_t identity = mem.Read<uintptr_t>(ent);
    if (!identity) return 0;

	return mem.Read<uint16_t>(identity + 0x10);
}