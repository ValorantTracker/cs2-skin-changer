#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "../ext/offsets.h"

using json = nlohmann::json;

namespace Updater {

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string FetchURL(const std::string& url) {
        CURL* curl = curl_easy_init();
        std::string buffer;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            if(res != CURLE_OK) return "";
        }
        return buffer;
    }

    void UpdateOffsets() {
        std::cout << "[Updater] Checking for offset updates..." << std::endl;
        
        // 1. Client DLL Offsets (schemas)
        std::string clientData = FetchURL("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json");
        if (!clientData.empty()) {
            try {
                json j = json::parse(clientData);
                auto& classes = j["client.dll"]["classes"];

                // Helpers
                auto get = [&](const std::string& cls, const std::string& field) -> std::ptrdiff_t {
                    if (classes.find(cls) != classes.end() && classes[cls]["fields"].find(field) != classes[cls]["fields"].end()) {
                        return static_cast<std::ptrdiff_t>(classes[cls]["fields"][field].get<uint64_t>());
                    }
                    return 0; // Failure
                };

                // Apply Updates
                Offsets::m_pInventoryServices = get("CCSPlayerController", "m_pInventoryServices");
                Offsets::m_unMusicID = get("CCSPlayerController_InventoryServices", "m_unMusicID");

                Offsets::m_pClippingWeapon = get("C_CSPlayerPawn", "m_pClippingWeapon");
                Offsets::m_pWeaponServices = get("C_BasePlayerPawn", "m_pWeaponServices");
                Offsets::m_hHudModelArms = get("C_CSPlayerPawn", "m_hHudModelArms");
                Offsets::m_hOwnerEntity = get("C_BaseEntity", "m_hOwnerEntity");
                Offsets::m_pEntity = get("CEntityInstance", "m_pEntity");
                Offsets::m_flags = get("CEntityIdentity", "m_flags");

                Offsets::m_hMyWeapons = get("CPlayer_WeaponServices", "m_hMyWeapons");
                Offsets::m_hActiveWeapon = get("CPlayer_WeaponServices", "m_hActiveWeapon");

                Offsets::m_pGameSceneNode = get("C_BaseEntity", "m_pGameSceneNode");
                Offsets::m_pChild = get("CGameSceneNode", "m_pChild");
                Offsets::m_pNextSibling = get("CGameSceneNode", "m_pNextSibling");
                Offsets::m_pOwner = get("CGameSceneNode", "m_pOwner");

                Offsets::m_bNeedToReApplyGloves = get("C_CSPlayerPawn", "m_bNeedToReApplyGloves");
                Offsets::m_EconGloves = get("C_CSPlayerPawn", "m_EconGloves");
                Offsets::m_bInitialized = get("C_EconItemView", "m_bInitialized");
                Offsets::m_bRestoreCustomMaterialAfterPrecache = get("C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache");
                Offsets::m_iEntityQuality = get("C_EconItemView", "m_iEntityQuality");

                Offsets::m_modelState = get("CSkeletonInstance", "m_modelState");
                Offsets::m_MeshGroupMask = get("CModelState", "m_MeshGroupMask");
                
                Offsets::m_nSubclassID = get("C_BaseEntity", "m_nSubclassID");
                Offsets::m_bMeleeWeapon = get("CCSWeaponBaseVData", "m_bMeleeWeapon");
                Offsets::m_WeaponType = get("CCSWeaponBaseVData", "m_WeaponType");

                Offsets::m_nFallbackPaintKit = get("C_EconEntity", "m_nFallbackPaintKit");
                Offsets::m_nFallbackStatTrak = get("C_EconEntity", "m_nFallbackStatTrak");
                Offsets::m_flFallbackWear = get("C_EconEntity", "m_flFallbackWear");
                Offsets::m_nFallbackSeed = get("C_EconEntity", "m_nFallbackSeed");
                Offsets::m_OriginalOwnerXuidLow = get("C_EconEntity", "m_OriginalOwnerXuidLow");

                Offsets::m_AttributeManager = get("C_EconEntity", "m_AttributeManager");
                Offsets::m_Item = get("C_AttributeContainer", "m_Item");

                Offsets::m_AttributeList = get("C_EconItemView", "m_AttributeList");
                Offsets::m_NetworkedDynamicAttributes = get("C_EconItemView", "m_NetworkedDynamicAttributes");
                Offsets::m_Attributes = get("CAttributeList", "m_Attributes");
                Offsets::m_iItemDefinitionIndex = get("C_EconItemView", "m_iItemDefinitionIndex");
                Offsets::m_iAccountID = get("C_EconItemView", "m_iAccountID");
                Offsets::m_szCustomNameOverride = get("C_EconItemView", "m_szCustomNameOverride");
                Offsets::m_iItemIDHigh = get("C_EconItemView", "m_iItemIDHigh");

                std::cout << "[Updater] Schemas updated." << std::endl;
            } catch(...) {
                std::cout << "[Updater] Failed to parse client.dll.json" << std::endl;
            }
        }

        // 2. Global Offsets
        std::string offsetsData = FetchURL("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json");
        if(!offsetsData.empty()) {
             try {
                json j = json::parse(offsetsData);
                auto& client = j["client.dll"];
                
                if (client.find("dwEntityList") != client.end()) Offsets::dwEntityList = static_cast<std::ptrdiff_t>(client["dwEntityList"].get<uint64_t>());
                if (client.find("dwGameEntitySystem_highestEntityIndex") != client.end()) Offsets::dwGameEntitySystem_highestEntityIndex = static_cast<std::ptrdiff_t>(client["dwGameEntitySystem_highestEntityIndex"].get<uint64_t>());
                if (client.find("dwLocalPlayerController") != client.end()) Offsets::dwLocalPlayerController = static_cast<std::ptrdiff_t>(client["dwLocalPlayerController"].get<uint64_t>());
                if (client.find("dwLocalPlayerPawn") != client.end()) Offsets::dwLocalPlayerPawn = static_cast<std::ptrdiff_t>(client["dwLocalPlayerPawn"].get<uint64_t>());

                std::cout << "[Updater] Global offsets updated." << std::endl;
             } catch (...) {
                 std::cout << "[Updater] Failed to parse offsets.json" << std::endl;
             }
        }
    }
}
