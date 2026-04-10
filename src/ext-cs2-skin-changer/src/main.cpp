#include "menu.h"
#include "SDK/entity/CWeaponServices.h"
#include "SDK/weapon/C_EconEntity.h"
#include "SDK/CEconItemAttributeManager.h"
#include "update_offsets.h"
#include <set>

bool ForceUpdate = false;
SkinManager* skinManager = new SkinManager();
CSkinDB* skindb = new CSkinDB();

void SkinChangerThread()
{
    uintptr_t lastActiveWeapon = 0;

    while (true)
    {
        Sleep(15);
        const uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) continue;

        const uintptr_t activeWeapon = mem.Read<uintptr_t>(localPlayer + Offsets::m_pClippingWeapon);
        const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);

        bool anyUpdated = false;

        // Only full scan if active weapon changed or force update
        if (activeWeapon != lastActiveWeapon || ForceUpdate)
        {
            for (const uintptr_t& weapon : weapons)
            {
                const uintptr_t item = weapon + Offsets::m_AttributeManager + Offsets::m_Item;
                WeaponsEnum defIndex = mem.Read<WeaponsEnum>(item + Offsets::m_iItemDefinitionIndex);
                SkinInfo_t skin = skinManager->GetSkin(defIndex);

                // Knife Logic
                Knife_t customKnife = skinManager->GetKnife();
                bool isKnife = (defIndex == WeaponsEnum::CtKnife || defIndex == WeaponsEnum::Tknife || (defIndex >= 500 && defIndex <= 526));

                if (isKnife && customKnife.defIndex != 0) {
                     skin = skinManager->GetSkin(WeaponsEnum::CtKnife); // Get the skin assigned to knives
                     if (skin.Paint == 0) skin = skinManager->GetSkin(WeaponsEnum::Tknife);
                }

                if (skin.weaponType == WeaponsEnum::none && !isKnife) continue;

                const uint64_t mask = skin.bUsesOldModel ? 2 : 1;
                const auto& node = mem.Read<uintptr_t>(weapon + Offsets::m_pGameSceneNode);
                const auto model = node + Offsets::m_modelState;

                const uintptr_t hudWeapon = GetHudWeapon(weapon);
                bool hudMaskOk = true;
                if (hudWeapon) {
                    const auto& hudNode = mem.Read<uintptr_t>(hudWeapon + Offsets::m_pGameSceneNode);
                    const auto hudModel = hudNode + Offsets::m_modelState;
                    hudMaskOk = (mem.Read<uint64_t>(hudModel + Offsets::m_MeshGroupMask) == mask);
                }

                bool needsUpdate = ForceUpdate ||
                                   (isKnife && customKnife.defIndex != 0 && (uint16_t)defIndex != customKnife.defIndex) ||
                                   (mem.Read<uint32_t>(item + Offsets::m_iItemIDHigh) != 1337) ||
                                   (mem.Read<uint32_t>(weapon + Offsets::m_nFallbackPaintKit) != skin.Paint) ||
                                   (mem.Read<uint64_t>(model + Offsets::m_MeshGroupMask) != mask) ||
                                   !hudMaskOk;

                if (needsUpdate)
                {
                    if (isKnife && customKnife.defIndex != 0) {
                        mem.Write<uint16_t>(item + Offsets::m_iItemDefinitionIndex, customKnife.defIndex);
                        mem.Write<uint16_t>(weapon + Offsets::m_iItemDefinitionIndex, customKnife.defIndex);
                        mem.Write<int>(item + Offsets::m_iEntityQuality, 3);
                    }

                    mem.Write<uint32_t>(item + Offsets::m_iItemIDHigh, 1337);
                    mem.Write<uint32_t>(weapon + Offsets::m_nFallbackPaintKit, skin.Paint);

                    SetMeshMask(weapon, mask);
                    if (hudWeapon) SetMeshMask(hudWeapon, mask);

                    econItemAttributeManager.Create(item, skin);
                    anyUpdated = true;
                }
            }
            lastActiveWeapon = activeWeapon;
        }

        if (anyUpdated || ForceUpdate)
        {
            UpdateWeapons(weapons);
            ForceUpdate = false;
        }
    }
}

void GloveChangerThread()
{
    while (true)
    {
        Sleep(100); // 100ms is plenty for gloves
        const uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) continue;

        Glove_t currentGloves = skinManager->GetGloves();
        if (currentGloves.defIndex == 0) continue;

        const uintptr_t econGloves = localPlayer + Offsets::m_EconGloves;
        uint16_t currentDef = mem.Read<uint16_t>(econGloves + Offsets::m_iItemDefinitionIndex);
        int currentPaint = mem.Read<int>(econGloves + Offsets::m_nFallbackPaintKit);

        if (currentDef != currentGloves.defIndex || currentPaint != currentGloves.Paint)
        {
            mem.Write<bool>(econGloves + Offsets::m_bInitialized, false);
            mem.Write<uint16_t>(econGloves + Offsets::m_iItemDefinitionIndex, currentGloves.defIndex);
            mem.Write<int>(econGloves + Offsets::m_iItemIDHigh, -1);
            mem.Write<int>(econGloves + Offsets::m_iItemIDHigh + 4, -1);
            mem.Write<int>(econGloves + Offsets::m_iEntityQuality, 3);

            // XUID Fix
            const std::vector<uintptr_t> weapons = GetWeapons(localPlayer);
            if (!weapons.empty()) {
                int xuidLow = mem.Read<int>(weapons[0] + Offsets::m_OriginalOwnerXuidLow);
                int xuidHigh = mem.Read<int>(weapons[0] + Offsets::m_OriginalOwnerXuidLow + 4);
                mem.Write<int>(econGloves + Offsets::m_OriginalOwnerXuidLow, xuidLow);
                mem.Write<int>(econGloves + Offsets::m_OriginalOwnerXuidLow + 4, xuidHigh);
                mem.Write<int>(econGloves + Offsets::m_iAccountID, xuidLow);
            }

            SkinInfo_t gloveSkin;
            gloveSkin.Paint = currentGloves.Paint;
            econItemAttributeManager.Create(econGloves, gloveSkin);

            mem.Write<bool>(econGloves + Offsets::m_bInitialized, true);
            mem.Write<bool>(localPlayer + Offsets::m_bNeedToReApplyGloves, true);
        }
    }
}

int main()
{
    if (Sigs::RegenerateWeaponSkins) {
        mem.Write<uint16_t>(Sigs::RegenerateWeaponSkins + 0x52, (uint16_t)(Offsets::m_AttributeManager + Offsets::m_Item + Offsets::m_AttributeList + Offsets::m_Attributes));
    }

    skindb->Dump();
    Updater::UpdateOffsets();

    InitMenu();

    configManager->Setup();
    configManager->AutoLoad();

    std::thread(SkinChangerThread).detach();
    std::thread(GloveChangerThread).detach();

    std::cout << "SkinChanger Started (Multi-threaded)\n";

    while (true)
    {
        Sleep(10);

        const uintptr_t localController = GetLocalController();
        if (!localController) continue;

        const uintptr_t inventoryServices = mem.Read<uintptr_t>(localController + Offsets::m_pInventoryServices);
        const uintptr_t localPlayer = GetLocalPlayer();

        if (inventoryServices)
            mem.Write<uint16_t>(inventoryServices + Offsets::m_unMusicID, skinManager->MusicKit.id);

        UpdateActiveMenuDef(localPlayer);
        OnFrame();

        if (ForceUpdate) {
            configManager->AutoSave();
        }
    }

    return 0;
}
