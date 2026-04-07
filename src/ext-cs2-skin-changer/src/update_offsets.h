#pragma once
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <winhttp.h>
#include <vector>
#include "../ext/offsets.h"

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

namespace Updater {

    std::string FetchURL(const std::string& url) {
        std::string result;
        HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
        URL_COMPONENTS urlComp = { 0 };
        urlComp.dwStructSize = sizeof(urlComp);
        
        std::wstring wurl(url.begin(), url.end());
        wchar_t host[256], path[1024];
        urlComp.lpszHostName = host;
        urlComp.dwHostNameLength = 256;
        urlComp.lpszUrlPath = path;
        urlComp.dwUrlPathLength = 1024;

        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &urlComp)) return "";

        hSession = WinHttpOpen(L"SkinChanger/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (hSession) hConnect = WinHttpConnect(hSession, host, urlComp.nPort, 0);
        if (hConnect) hRequest = WinHttpOpenRequest(hConnect, L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, (urlComp.nPort == 443) ? WINHTTP_FLAG_SECURE : 0);
        
        if (hRequest && WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD dwSize = 0;
            do {
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                if (dwSize == 0) break;
                std::vector<char> buffer(dwSize);
                DWORD dwDownloaded = 0;
                if (WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                    result.append(buffer.data(), dwDownloaded);
                }
            } while (dwSize > 0);
        }

        if (hRequest) WinHttpCloseHandle(hRequest);
        if (hConnect) WinHttpCloseHandle(hConnect);
        if (hSession) WinHttpCloseHandle(hSession);

        return result;
    }

    void UpdateOffsets() {
        std::cout << "[Updater] Checking for offset updates..." << std::endl;
        
        std::string cacheDir;
        char appDataPath[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
            cacheDir = std::string(appDataPath) + "\\SkinChanger\\cache";
        } else {
            cacheDir = ".\\cache";
        }

        std::filesystem::create_directories(cacheDir);
        std::string clientPath = cacheDir + "\\client_dll.json";
        std::string offsetsPath = cacheDir + "\\offsets.json";

        std::string clientData = FetchURL("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json");
        if (clientData.empty() && std::filesystem::exists(clientPath)) {
            std::ifstream f(clientPath);
            clientData.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            std::cout << "[Updater] Using cached client_dll.json" << std::endl;
        } else if (!clientData.empty()) {
            std::ofstream f(clientPath);
            f << clientData;
        }

        if (!clientData.empty()) {
            try {
                json j = json::parse(clientData);
                auto& classes = j["client.dll"]["classes"];

                auto get = [&](const std::string& cls, const std::string& field) -> std::ptrdiff_t {
                    if (classes.find(cls) != classes.end() && classes[cls]["fields"].find(field) != classes[cls]["fields"].end()) {
                        return static_cast<std::ptrdiff_t>(classes[cls]["fields"][field].get<uint64_t>());
                    }
                    return 0;
                };

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
            } catch(const std::exception& e) {
                std::cout << "[Updater] Failed to parse client.dll.json: " << e.what() << std::endl;
            }
        }

        std::string offsetsData = FetchURL("https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json");
        if (offsetsData.empty() && std::filesystem::exists(offsetsPath)) {
            std::ifstream f(offsetsPath);
            offsetsData.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            std::cout << "[Updater] Using cached offsets.json" << std::endl;
        } else if (!offsetsData.empty()) {
            std::ofstream f(offsetsPath);
            f << offsetsData;
        }

        if(!offsetsData.empty()) {
             try {
                json j = json::parse(offsetsData);
                auto& client = j["client.dll"];
                
                if (client.find("dwEntityList") != client.end()) Offsets::dwEntityList = static_cast<std::ptrdiff_t>(client["dwEntityList"].get<uint64_t>());
                if (client.find("dwGameEntitySystem_highestEntityIndex") != client.end()) Offsets::dwGameEntitySystem_highestEntityIndex = static_cast<std::ptrdiff_t>(client["dwGameEntitySystem_highestEntityIndex"].get<uint64_t>());
                if (client.find("dwLocalPlayerController") != client.end()) Offsets::dwLocalPlayerController = static_cast<std::ptrdiff_t>(client["dwLocalPlayerController"].get<uint64_t>());
                if (client.find("dwLocalPlayerPawn") != client.end()) Offsets::dwLocalPlayerPawn = static_cast<std::ptrdiff_t>(client["dwLocalPlayerPawn"].get<uint64_t>());

                std::cout << "[Updater] Global offsets updated." << std::endl;
             } catch (const std::exception& e) {
                 std::cout << "[Updater] Failed to parse offsets.json: " << e.what() << std::endl;
             }
        }
    }
}
