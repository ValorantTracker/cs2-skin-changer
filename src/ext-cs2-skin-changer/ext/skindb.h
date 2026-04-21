#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <winhttp.h>
#include <shlobj.h>
#include <fstream>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <shared_mutex>

#include "../src/SDK/weapon/weapon.h"
#include "../src/SDK/musicKits.h"

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

#pragma once

// WinHTTP based downloader for environments without CURL
static std::string WinHttpFetch(const std::string& url) {
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

std::unordered_map<uint16_t, std::string> KnifeModels = {
    {500, "phase2/weapons/models/knife/knife_bayonet/weapon_knife_bayonet_ag2.vmdl"},
    {503, "phase2/weapons/models/knife/knife_css/weapon_knife_css_ag2.vmdl"},
    {505, "phase2/weapons/models/knife/knife_flip/weapon_knife_flip_ag2.vmdl"},
    {506, "phase2/weapons/models/knife/knife_gut/weapon_knife_gut_ag2.vmdl"},
    {507, "phase2/weapons/models/knife/knife_karambit/weapon_knife_karambit_ag2.vmdl"},
    {508, "phase2/weapons/models/knife/knife_m9/weapon_knife_m9_ag2.vmdl"},
    {509, "phase2/weapons/models/knife/knife_tactical/weapon_knife_tactical_ag2.vmdl"},
    {512, "phase2/weapons/models/knife/knife_falchion/weapon_knife_falchion_ag2.vmdl"},
    {514, "phase2/weapons/models/knife/knife_bowie/weapon_knife_bowie_ag2.vmdl"},
    {515, "phase2/weapons/models/knife/knife_butterfly/weapon_knife_butterfly_ag2.vmdl"},
    {516, "phase2/weapons/models/knife/knife_push/weapon_knife_push_ag2.vmdl"},
    {517, "phase2/weapons/models/knife/knife_cord/weapon_knife_cord_ag2.vmdl"},
    {518, "phase2/weapons/models/knife/knife_canis/weapon_knife_canis_ag2.vmdl"},
    {519, "phase2/weapons/models/knife/knife_ursus/weapon_knife_ursus_ag2.vmdl"},
    {520, "phase2/weapons/models/knife/knife_navaja/weapon_knife_navaja_ag2.vmdl"},
    {521, "phase2/weapons/models/knife/knife_nomad/weapon_knife_nomad_ag2.vmdl"},
    {522, "phase2/weapons/models/knife/knife_stiletto/weapon_knife_stiletto_ag2.vmdl"},
    {523, "phase2/weapons/models/knife/knife_talon/weapon_knife_talon_ag2.vmdl"},
    {525, "phase2/weapons/models/knife/knife_skeleton/weapon_knife_skeleton_ag2.vmdl"},
    {526, "phase2/weapons/models/knife/knife_kukri/weapon_knife_kukri_ag2.vmdl"}
};
std::map<uint16_t, std::string> KnifeNames = {
    {500, "Bayonet"},
    {503, "Classic"},
    {505, "Flip"},
    {506, "Gut"},
    {507, "Karambit"},
    {508, "M9 Bayonet"},
    {509, "Huntsman"},
    {512, "Falchion"},
    {514, "Bowie"},
    {515, "Butterfly"},
    {516, "Daggers"},
    {517, "Paracord"},
    {518, "Survival"},
    {519, "Ursus"},
    {520, "Navaja"},
    {521, "Nomad"},
    {522, "Stiletto"},
    {523, "Talon"},
    {525, "Skeleton"},
    {526, "Kukri"}
};

class Knife_t
{
public:
    Knife_t(const uint16_t def = 0)
    {
        defIndex = def;
        if (def)
        {
            if (KnifeNames.find(def) != KnifeNames.end()) name = KnifeNames.at(def);
            if (KnifeModels.find(def) != KnifeModels.end()) model = KnifeModels.at(def);
        }
    }

    uint16_t defIndex;
	std::string name;
	std::string model;

};

std::vector<Knife_t> Knifes = {
    Knife_t(500), Knife_t(503), Knife_t(505), Knife_t(506), Knife_t(507),
    Knife_t(508), Knife_t(509), Knife_t(512), Knife_t(514), Knife_t(515), Knife_t(516),
    Knife_t(517), Knife_t(518), Knife_t(519), Knife_t(520), Knife_t(521),
    Knife_t(522), Knife_t(523), Knife_t(525), Knife_t(526)
};

struct SkinInfo_t {
    int Paint = 0;
    bool bUsesOldModel = false;
    std::string name;
    WeaponsEnum weaponType = WeaponsEnum::none;
    int rarity = 0;
    std::string image_url;
};

struct Glove_t
{
    uint16_t defIndex;
    int Paint;
    std::string name;

    Glove_t(uint16_t _def = 0, int _paint = 0, std::string _name = "") : defIndex(_def), Paint(_paint), name(_name) {}
};

static std::map<uint16_t, std::string> GloveNames = {
    {5027, "Bloodhound"},
    {5028, "T-Default"},
    {5029, "CT-Default"},
    {5030, "Sport"},
    {5031, "Driver"},
    {5032, "Hand Wraps"},
    {5033, "Moto"},
    {5034, "Specialist"},
    {5035, "Hydra"},
    {5025, "Broken Fang"}
};

static std::vector<Glove_t> GloveTypes = {
    Glove_t(5027, 0, "Bloodhound Gloves"),
    Glove_t(5030, 0, "Sport Gloves"),
    Glove_t(5031, 0, "Driver Gloves"),
    Glove_t(5032, 0, "Hand Wraps"),
    Glove_t(5033, 0, "Moto Gloves"),
    Glove_t(5034, 0, "Specialist Gloves"),
    Glove_t(5035, 0, "Hydra Gloves"),
    Glove_t(5025, 0, "Broken Fang Gloves")
};

extern bool ForceUpdate;

class SkinManager
{
public:
    std::vector<SkinInfo_t> Skins;
    Glove_t Gloves = Glove_t();
    Knife_t Knife = Knife_t();
    MusicKit_t MusicKit = MusicKit_t(static_cast<uint16_t>(MusicKit::CounterStrike2), "Counter-Strike 2");
    mutable std::shared_mutex mtx;

    void AddSkin(SkinInfo_t AddedSkin)
    {
        std::unique_lock lock(mtx);
        for (SkinInfo_t& skin : Skins)
        {
            if (skin.weaponType == AddedSkin.weaponType)
            {
                if (skin.Paint == AddedSkin.Paint)
                    return;

                skin = AddedSkin;
                ForceUpdate = true;
                return;
            }
        }

        Skins.push_back(AddedSkin);
        ForceUpdate = true;
    }

    SkinInfo_t GetSkin(const WeaponsEnum def) const
    {
        std::shared_lock lock(mtx);
        for (const SkinInfo_t& skin : Skins)
            if (skin.weaponType == def)
                return skin;
        return SkinInfo_t{0, false, std::string(), WeaponsEnum::none};
    }

    void SetGloves(Glove_t g)
    {
        std::unique_lock lock(mtx);
        Gloves = g;
        ForceUpdate = true;
    }

    Glove_t GetGloves() const
    {
        std::shared_lock lock(mtx);
        return Gloves;
    }

    void SetKnife(Knife_t k)
    {
        std::unique_lock lock(mtx);
        Knife = k;
        ForceUpdate = true;
    }

    Knife_t GetKnife() const
    {
        std::shared_lock lock(mtx);
        return Knife;
    }
};

extern SkinManager* skinManager;

class CSkinDB {
private:
    std::vector<SkinInfo_t> knifeSkins;
    std::vector<SkinInfo_t> gloveSkins;
    std::vector<SkinInfo_t> weaponSkins;

    std::string GetStringSafe(const nlohmann::json& j, const std::string& key) {
        if (j.find(key) == j.end()) return "";
        if (j[key].is_string()) return j[key].get<std::string>();
        if (j[key].is_object() && j[key].find("en") != j[key].end()) return j[key]["en"].get<std::string>();
        return "";
    }

    int GetPaintIndexSafe(const nlohmann::json& skin) {
        if (skin.find("paint_index") == skin.end()) return 0;
        if (skin["paint_index"].is_number_integer()) return skin["paint_index"].get<int>();
        if (skin["paint_index"].is_string()) {
            try { return std::stoi(skin["paint_index"].get<std::string>()); }
            catch (...) { return 0; }
        }
        return 0;
    }

    int GetRarityIdSafe(const nlohmann::json& skin) {
        if (skin.find("rarity") == skin.end()) return 1;
        auto& r = skin["rarity"];
        if (r.is_object() && r.find("id") != r.end()) {
            if (r["id"].is_string()) {
                std::string idStr = r["id"].get<std::string>();
                if (idStr.find("contraband") != std::string::npos) return 7;
                if (idStr.find("ancient") != std::string::npos) return 6;
                if (idStr.find("legendary") != std::string::npos) return 5;
                if (idStr.find("mythical") != std::string::npos) return 4;
                if (idStr.find("rare") != std::string::npos) return 3;
                if (idStr.find("uncommon") != std::string::npos) return 2;
                if (idStr.find("common") != std::string::npos) return 1;
                return 1;
            }
            if (r["id"].is_number_integer()) return r["id"].get<int>();
        }
        return 1;
    }

    std::vector<std::string> knifeTypes = {
        "Bayonet", "Classic Knife", "Flip Knife", "Gut Knife",
        "Karambit", "M9 Bayonet", "Huntsman Knife", "Falchion Knife",
        "Bowie Knife", "Butterfly Knife", "Shadow Daggers", "Paracord Knife",
        "Survival Knife", "Ursus Knife", "Navaja Knife", "Nomad Knife",
        "Stiletto Knife", "Talon Knife", "Skeleton Knife", "Kukri Knife"
    };

    std::vector<std::string> gloveTypes = {
        "Bloodhound Gloves", "Broken Fang Gloves",
        "Driver Gloves", "Hand Wraps",
        "Hydra Gloves", "Moto Gloves",
        "Specialist Gloves", "Sport Gloves"
    };

    WeaponsEnum GetDefPerString(const std::string& name)
    {
        static const std::unordered_map<std::string, WeaponsEnum> weaponMap = {
            {"AK-47", WeaponsEnum::Ak47}, {"AUG", WeaponsEnum::Aug}, {"AWP", WeaponsEnum::Awp},
            {"PP-Bizon", WeaponsEnum::Bizon}, {"CZ75-Auto", WeaponsEnum::Cz65A},
            {"Desert Eagle", WeaponsEnum::Deagle}, {"Dual Berettas", WeaponsEnum::Elite},
            {"FAMAS", WeaponsEnum::Famas}, {"Five-SeveN", WeaponsEnum::FiveSeven},
            {"G3SG1", WeaponsEnum::G3Sg1}, {"Galil AR", WeaponsEnum::Galil},
            {"Glock-18", WeaponsEnum::Glock}, {"P2000", WeaponsEnum::P200},
            {"M249", WeaponsEnum::M249}, {"M4A1-S", WeaponsEnum::M4A1Silencer},
            {"M4A4", WeaponsEnum::M4A4}, {"MAC-10", WeaponsEnum::Mac10},
            {"MAG-7", WeaponsEnum::Mag7}, {"MP5-SD", WeaponsEnum::Mp5SD},
            {"MP7", WeaponsEnum::Mp7}, {"MP9", WeaponsEnum::Mp9},
            {"Negev", WeaponsEnum::Negev}, {"Nova", WeaponsEnum::Nova},
            {"XM1014", WeaponsEnum::Xm1014}, {"USP-S", WeaponsEnum::UspS},
            {"Tec-9", WeaponsEnum::Tec9}, {"Zeus x27", WeaponsEnum::Zeus},
            {"SSG 08", WeaponsEnum::Ssg08}, {"SG 553", WeaponsEnum::Sg556},
            {"SCAR-20", WeaponsEnum::Scar20}, {"Sawed-Off", WeaponsEnum::Sawedoof},
            {"R8 Revolver", WeaponsEnum::Revolver}, {"P90", WeaponsEnum::P90},
            {"P250", WeaponsEnum::p250}
        };

        for (const auto& [key, value] : weaponMap)
            if (name.find(key) != std::string::npos) return value;
        return WeaponsEnum::none;
    }

public:
    void DumpSkindb()
    {
        std::cout << "[SkinDB] Downloading skins database..." << std::endl;
        std::string readBuffer = WinHttpFetch("https://raw.githubusercontent.com/ByMykel/CSGO-API/main/public/api/en/skins.json");
        if (readBuffer.empty()) {
            std::cout << "[SkinDB] Failed to download skins database." << std::endl;
            return;
        }

        try {
            auto jsonData = nlohmann::json::parse(readBuffer);
            for (auto& skin : jsonData) {
                SkinInfo_t info;
                info.Paint = GetPaintIndexSafe(skin);
                info.name = GetStringSafe(skin, "name");
                info.weaponType = GetDefPerString(info.name);
                info.rarity = GetRarityIdSafe(skin);
                info.image_url = GetStringSafe(skin, "image");

                if (skin.find("legacy_model") != skin.end() && skin["legacy_model"].is_boolean())
                    info.bUsesOldModel = skin["legacy_model"].get<bool>();

                bool isKnife = false, isGlove = false;
                for (auto& k : knifeTypes) if (info.name.find(k) != std::string::npos) { isKnife = true; break; }
                for (auto& g : gloveTypes) if (info.name.find(g) != std::string::npos) { isGlove = true; break; }

                if (isKnife) knifeSkins.push_back(info);
                else if (isGlove) gloveSkins.push_back(info);
                else weaponSkins.push_back(info);
            }
            std::cout << "[SkinDB] Skins parsed: " << (weaponSkins.size() + knifeSkins.size() + gloveSkins.size()) << std::endl;
        } catch (...) { std::cerr << "[SkinDB] JSON parse error" << std::endl; }
    }

    inline void Dump() { DumpSkindb(); }

    std::vector<SkinInfo_t> GetWeaponSkins(WeaponsEnum type = WeaponsEnum::none)
    {
        std::vector<SkinInfo_t> results;
        results.push_back(SkinInfo_t{ 0, false, "Vanilla", WeaponsEnum::none, 1 });
        if (type == WeaponsEnum::none) return results;
        for (const auto& skin : weaponSkins) if (skin.weaponType == type) results.push_back(skin);
        return results;
    }

    std::vector<SkinInfo_t> GetGloveSkins(const std::string& gloveType)
    {
        std::vector<SkinInfo_t> results;
        for (const auto& skin : gloveSkins) if (skin.name.find(gloveType) != std::string::npos) results.push_back(skin);
        return results;
    }

    const std::vector<SkinInfo_t>& GetKnifeSkins() { return knifeSkins; }
};

extern CSkinDB* skindb;
