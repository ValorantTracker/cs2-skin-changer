#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "SDK/weapon/C_EconEntity.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

class CConfigManager
{
private:
    std::wstring configDir = L"C:\\Skin2Merde\\";

public:
    void Setup()
    {
        if (!fs::exists(configDir))
        {
            fs::create_directories(configDir);
        }
    }

    std::vector<std::string> GetConfigs()
    {
        std::vector<std::string> configs;
        for (const auto& entry : fs::directory_iterator(configDir))
        {
            if (entry.path().extension() == ".json")
            {
                configs.push_back(entry.path().stem().string());
            }
        }
        return configs;
    }

    void Save(const std::string& name)
    {
        if (name.empty()) return;

        json j;

        // Skins
        for (const auto& skin : skinManager->Skins)
        {
            json s;
            s["weapon"] = (int)skin.weaponType;
            s["paint"] = skin.Paint;
            s["rarity"] = skin.rarity;
            s["name"] = skin.name;
            s["image"] = skin.image_url;
            
            j["skins"].push_back(s);
        }

        // Music Kit
        j["musickit_id"] = skinManager->MusicKit.id;
        j["musickit_name"] = skinManager->MusicKit.name;

        // Write
        std::string path = std::string(configDir.begin(), configDir.end()) + name + ".json";
        std::ofstream o(path);
        o << std::setw(4) << j << std::endl;
    }

    void Load(const std::string& name)
    {
        if (name.empty()) return;
        std::string path = std::string(configDir.begin(), configDir.end()) + name + ".json";
        
        if (!fs::exists(path)) return;

        try {
            std::ifstream i(path);
            json j;
            i >> j;

            skinManager->Skins.clear();

            if (j.find("skins") != j.end())
            {
                for (auto& element : j["skins"])
                {
                    SkinInfo_t skin;
                    skin.weaponType = (WeaponsEnum)element["weapon"].get<int>();
                    skin.Paint = element["paint"].get<int>();
                    

                    if (element.find("rarity") != element.end()) skin.rarity = element["rarity"].get<int>();
                    else skin.rarity = 1;
                    
                    if (element.find("name") != element.end()) skin.name = element["name"].get<std::string>();
                    else skin.name = "Unknown";
                    
                    if (element.find("image") != element.end()) skin.image_url = element["image"].get<std::string>();
                    
                    skinManager->AddSkin(skin);
                }
            }
            
            if (j.find("musickit_id") != j.end()) {
                skinManager->MusicKit.id = j["musickit_id"].get<uint16_t>();
                if (j.find("musickit_name") != j.end())
                    skinManager->MusicKit.name = j["musickit_name"].get<std::string>();
            }

            ForceUpdate = true;
        } catch (...) {
            // Corrupt file
        }
    }
    
    // Auto Logic
    void AutoSave() { Save("default"); }
    void AutoLoad() { Load("default"); }
};

inline CConfigManager* configManager = new CConfigManager();
