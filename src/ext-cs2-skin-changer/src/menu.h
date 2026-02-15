#pragma once
#include "SDK/weapon/C_EconEntity.h"
#include "../ext/skindb.h"
#include "window/window.hpp"
#include "ui_engine.hpp"

static WeaponsEnum CurrentWeaponDef;
static int selectedSkinIndex = 0;
static int selectedKnifeIndex = 0;
static int selectedKnifeSkinIndex = 0;
static int selectedMusicKitIndex = 0;
static char searchBuffer[128] = "";

void UpdateSearchInput() {
}

void RenderWeaponTab(float x, float y, float w, float h)
{
    // Search Box
    SC_GUI::TextInput("search_wep", searchBuffer, 128, x + 20, y + 20, 300, 35, "Search Skins...");
    
    // Scroll State
    static float scrollY = 0.0f;
    float contentHeight = 0.0f;

    // Viewport
    float viewX = x;
    float viewY = y + 70;
    float viewW = w;
    float viewH = h - 70;

    // Grid Settings
    float itemW = 140; float itemH = 160; float pad = 15;
    int cols = (int)((viewW - 40) / (itemW + pad));
    if (cols < 1) cols = 1;

    // Handle Input
    if (SC_GUI::Input.mousePos.x >= viewX && SC_GUI::Input.mousePos.x <= viewX + viewW &&
        SC_GUI::Input.mousePos.y >= viewY && SC_GUI::Input.mousePos.y <= viewY + viewH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f; // Scale speed
    }

    // Prepare content
    std::vector<SkinInfo_t> availableSkins = skindb->GetWeaponSkins(CurrentWeaponDef);
    
    // SORT BY RARITY (Descending)
    std::sort(availableSkins.begin(), availableSkins.end(), [](const SkinInfo_t& a, const SkinInfo_t& b) {
        return a.rarity > b.rarity;
    });

    std::string query = searchBuffer;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    // Filter First to get total count
    std::vector<int> filteredIndices;
    for (int i = 0; i < (int)availableSkins.size(); i++) {
        if (!query.empty()) {
             std::string name = availableSkins[i].name;
             std::transform(name.begin(), name.end(), name.begin(), ::tolower);
             if (name.find(query) == std::string::npos) continue;
        }
        filteredIndices.push_back(i);
    }
    
    // Calculate Content Height
    int rows = ((int)filteredIndices.size() + cols - 1) / cols;
    contentHeight = rows * (itemH + pad);

    // Clamp Scroll
    float maxScroll = 0.0f;
    float minScroll = -(contentHeight - viewH + pad);
    if (minScroll > 0) minScroll = 0;
    if (scrollY > maxScroll) scrollY = maxScroll;
    if (scrollY < minScroll) scrollY = minScroll;

    // Determine currently selected skin index
    int selectedSkinIndex = 0;
    SkinInfo_t currentSkin = skinManager->GetSkin(CurrentWeaponDef);
    if (currentSkin.weaponType != WeaponsEnum::none) {
        for (int i = 0; i < (int)availableSkins.size(); i++) {
            if (availableSkins[i].Paint == currentSkin.Paint) {
                selectedSkinIndex = i;
                break;
            }
        }
    }

    // Draw With Clip
    SC_GUI::SetClip(viewX, viewY, viewW, viewH);

    int displayIdx = 0;
    for (int idx : filteredIndices) {
        // Calculate Pos
        float cX = viewX + 20 + (displayIdx % cols) * (itemW + pad);
        float cY = viewY + scrollY + (displayIdx / cols) * (itemH + pad);
        
        // Visibility Check (Culling)
        if (cY + itemH < viewY || cY > viewY + viewH) {
             displayIdx++; continue;
        }

        bool selected = (selectedSkinIndex == idx);
        
        if (SC_GUI::SkinCard("wskin_" + availableSkins[idx].name, availableSkins[idx].name, availableSkins[idx].image_url, availableSkins[idx].rarity, cX, cY, itemW, itemH, selected)) {
            if (idx != 0) skinManager->AddSkin(availableSkins[idx]);
            else {
                skinManager->AddSkin(availableSkins[idx]); 
            }
        }
        displayIdx++;
    }

    SC_GUI::ResetClip();
    
    // Scrollbar (Visual Only)
    if (contentHeight > viewH) {
        float ratio = viewH / contentHeight;
        float barH = viewH * ratio;
        float barY = viewY + (-scrollY / contentHeight) * viewH;
        SC_GUI::DrawRoundedRect(x + w - 8, barY, 4, barH, 2, Color(255, 100, 100, 100));
    }
}

void RenderKnifeTab(float x, float y, float w, float h)
{
     // Smart Auto-Detect Logic
    bool isDefaultKnife = true;
    int autoDetectedIndex = -1;
    auto it = KnifeNames.find(static_cast<uint16_t>(CurrentWeaponDef));
    if (it != KnifeNames.end()) {
        std::string knifeName = it->second;
        for (int i = 0; i < (int)Knifes.size(); i++) {
            if (Knifes[i].name.find(knifeName) != std::string::npos) {
                autoDetectedIndex = i; break;
            }
        }
    }
    if (autoDetectedIndex != -1) {
        if (selectedKnifeIndex != autoDetectedIndex) {
            selectedKnifeIndex = autoDetectedIndex;
            selectedKnifeSkinIndex = 0;
        }
        isDefaultKnife = false;
    }

    float curY = y + 20;

    // Model Selector (Only if Default)
    if (isDefaultKnife) {
        SC_GUI::DrawStringA("Select Knife Type (Scroll to see more):", x + 20, curY, Color::White, SC_GUI::mainFont, false);
        curY += 25;
        
        // Horizontal Scroll for Knives
        static float kScrollX = 0.0f;
        SC_GUI::SetClip(x, curY, w, 35);
        if (SC_GUI::Input.mousePos.y >= curY && SC_GUI::Input.mousePos.y <= curY + 35) {
             kScrollX += SC_GUI::Input.scrollDelta * 0.5f;
        }
        float kContentW = Knifes.size() * 85.0f;
        if (kScrollX > 0) kScrollX = 0;
        if (kScrollX < -(kContentW - w)) kScrollX = -(kContentW - w);

        for(int k=0; k<(int)Knifes.size(); k++) {
             if (SC_GUI::Button("k_model_" + std::to_string(k), Knifes[k].name, x + 20 + kScrollX + (k*85), curY, 80, 25, selectedKnifeIndex == k)) {
                 selectedKnifeIndex = k;
                 selectedKnifeSkinIndex = 0;
             }
        }
        SC_GUI::ResetClip();
        curY += 40;
    } else {
        SC_GUI::DrawStringA("Detected: " + Knifes[selectedKnifeIndex].name, x + 20, curY, Color(255, 100, 255, 100), SC_GUI::titleFont, false);
        curY += 40;
    }

    // Grid (Scrollable)
    std::string FilterName = Knifes[selectedKnifeIndex].name;
    const std::vector<SkinInfo_t>& allKnifeSkins = skindb->GetKnifeSkins();
    std::vector<SkinInfo_t> filteredSkins;
    filteredSkins.push_back(SkinInfo_t{ 0, false, "Vanilla", WeaponsEnum::none, 1 }); // Default rarity
    
    for (const auto& skin : allKnifeSkins) {
        if (skin.name.find(FilterName) == std::string::npos) continue;
        filteredSkins.push_back(skin);
    }
    
    // Sort Knife Skins
    std::sort(filteredSkins.begin(), filteredSkins.end(), [](const SkinInfo_t& a, const SkinInfo_t& b) {
        return a.rarity > b.rarity;
    });
    
    // Draw Grid with Scroll
    static float kSkinScrollY = 0.0f;
    float viewY = curY;
    float viewH = (y + h) - viewY;
    float itemW = 140; float itemH = 160; float pad = 15;
    int cols = (int)((w - 40) / (itemW + pad));
    if (cols < 1) cols = 1;
    
    // Input for Knife Grid
    if (SC_GUI::Input.mousePos.y > viewY) {
         kSkinScrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    
    int rows = ((int)filteredSkins.size() + cols - 1) / cols;
    float contentH = rows * (itemH + pad);
    if (kSkinScrollY > 0) kSkinScrollY = 0;
    if (kSkinScrollY < -(contentH - viewH + pad)) kSkinScrollY = -(contentH - viewH + pad);

    SC_GUI::SetClip(x, viewY, w, viewH);
    int displayIdx = 0;
    for (int i = 0; i < (int)filteredSkins.size(); i++) {
         float cX = x + 20 + (displayIdx % cols) * (itemW + pad);
         float cY = viewY + kSkinScrollY + (displayIdx / cols) * (itemH + pad);
         
         // Cull
         if (cY + itemH < viewY || cY > viewY + viewH) { displayIdx++; continue; }

         bool selected = (selectedKnifeSkinIndex == i);
         if (SC_GUI::SkinCard("kskin_" + filteredSkins[i].name, filteredSkins[i].name, filteredSkins[i].image_url, filteredSkins[i].rarity, cX, cY, itemW, itemH, selected)) {
             selectedKnifeSkinIndex = i;
             if (i!=0) {
                 SkinInfo_t s = filteredSkins[i];
                 s.weaponType = WeaponsEnum::CtKnife; skinManager->AddSkin(s);
                 s.weaponType = WeaponsEnum::Tknife; skinManager->AddSkin(s);
             }
         }
         displayIdx++;
    }
    SC_GUI::ResetClip();
}

void RenderMusicTab(float x, float y, float w, float h)
{
    // Search/Filter
    SC_GUI::TextInput("search_music", searchBuffer, 128, x + 20, y + 20, 300, 35, "Search Music Kits...");
    std::string query = searchBuffer;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    std::vector<int> filteredIndices;
    for (int i = 0; i < (int)musicKits.size(); i++) {
        if (!query.empty()) {
             std::string name = musicKits[i].name;
             std::transform(name.begin(), name.end(), name.begin(), ::tolower);
             if (name.find(query) == std::string::npos) continue;
        }
        filteredIndices.push_back(i);
    }

    // Scroll Logic
    static float mScrollY = 0.0f;
    float itemW = 140; float itemH = 100; float pad = 15;
    int cols = (int)((w - 40) / (itemW + pad));
    if (cols < 1) cols = 1;

    // Content Height
    int rows = ((int)filteredIndices.size() + cols - 1) / cols;
    float contentH = rows * (itemH + pad);
    float viewY = y + 70;
    float viewH = h - 70;

    // Input
    if (SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
        SC_GUI::Input.mousePos.y >= viewY && SC_GUI::Input.mousePos.y <= viewY + viewH) {
         mScrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    
    // Clamp
    if (mScrollY > 0) mScrollY = 0;
    if (mScrollY < -(contentH - viewH + pad)) mScrollY = -(contentH - viewH + pad);

    // Draw
    SC_GUI::SetClip(x, viewY, w, viewH);
    int displayIdx = 0;
    for (int idx : filteredIndices) {
        float cX = x + 20 + (displayIdx % cols) * (itemW + pad);
        float cY = viewY + mScrollY + (displayIdx / cols) * (itemH + pad);

        if (cY + itemH < viewY || cY > viewY + viewH) { displayIdx++; continue; }

        bool selected = (selectedMusicKitIndex == idx);
        // Use Rarity 3 (Rare/Blue) for Music Kits as default
        if (SC_GUI::SkinCard("music_" + std::to_string(idx), musicKits[idx].name, musicKits[idx].image_url, 3, cX, cY, itemW, itemH, selected)) {
             selectedMusicKitIndex = idx;
             skinManager->MusicKit = musicKits[idx];
             ForceUpdate = true;
        }
        displayIdx++;
    }
    SC_GUI::ResetClip();
    
    // Scrollbar
    if (contentH > viewH) {
        float ratio = viewH / contentH;
        float barH = viewH * ratio;
        float barY = viewY + (-mScrollY / contentH) * viewH;
        SC_GUI::DrawRoundedRect(x + w - 8, barY, 4, barH, 2, Color(255, 100, 100, 100));
    }
}

#include "config.h"

void RenderConfigTab(float x, float y, float w, float h)
{
    float curY = y + 20;

    // Config Input
    static char cfgNameBuf[64] = "";
    SC_GUI::TextInput("cfg_name", cfgNameBuf, 64, x + 20, curY, 200, 35, "Config Name");
    
    if (SC_GUI::Button("cfg_create_save", "Create / Save", x + 230, curY, 120, 35)) {
        std::string name = cfgNameBuf;
        if (!name.empty()) {
            configManager->Save(name);
            memset(cfgNameBuf, 0, 64);
        }
    }
    curY += 50;
    
    // Separator
    SC_GUI::DrawRect(x + 20, curY, w - 40, 1, SC_GUI::currentTheme.separator);
    curY += 20;

    // List Configs
    std::vector<std::string> configs = configManager->GetConfigs();
    
    // Scrollable Area
    float listH = h - (curY - y) - 20;
    
    static float scrollY = 0.0f;
    float contentH = configs.size() * 45.0f;
    
    // Input for List
     if (SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
        SC_GUI::Input.mousePos.y >= curY && SC_GUI::Input.mousePos.y <= curY + listH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    if (scrollY > 0) scrollY = 0;
    if (scrollY < -(contentH - listH)) scrollY = -(contentH - listH);
    if (contentH < listH) scrollY = 0;

    SC_GUI::SetClip(x, curY, w, listH);
    
    for (int i=0; i<(int)configs.size(); i++) {
        float itemY = curY + scrollY + (i * 45);
        if (itemY + 40 < curY || itemY > curY + listH) continue;

        // Background
        SC_GUI::DrawRoundedRect(x + 20, itemY, w - 240, 35, 6, SC_GUI::currentTheme.contentBg);
        SC_GUI::DrawStringA(configs[i], x + 35, itemY + 10, SC_GUI::currentTheme.text, SC_GUI::mainFont, false);

        // Buttons
        if (SC_GUI::Button("cfg_load_" + std::to_string(i), "Load", x + w - 200, itemY, 80, 35)) {
            configManager->Load(configs[i]);
        }
        
        if (SC_GUI::Button("cfg_save_" + std::to_string(i), "Save", x + w - 110, itemY, 80, 35)) {
            configManager->Save(configs[i]);
        }
    }
    
    SC_GUI::ResetClip();
}

void RenderSettingsTab(float x, float y, float w, float h)
{
    float curY = y + 20;
    
    // Configs below

    
    // Transparency
    SC_GUI::DrawStringA("Menu Transparency", x + 20, curY, Color(255, 255, 255), SC_GUI::mainFont, false);
    curY += 25;
    
    static float alphaVal = 255.0f;
    if (SC_GUI::Slider("alpha_slider", &alphaVal, 50.0f, 255.0f, x + 20, curY, 300, 20)) {
        overlay::GlobalAlpha = (BYTE)alphaVal;
    }
    curY += 40;

    // Themes
    SC_GUI::DrawStringA("Themes", x + 20, curY, Color(255, 255, 255), SC_GUI::mainFont, false);
    curY += 25;

    if (SC_GUI::Button("theme_darkred", "Dark Red (Default)", x + 20, curY, 200, 30)) {
        SC_GUI::currentTheme = {
            Color(255, 12, 12, 12),
            Color(255, 20, 20, 20),
            Color(255, 30, 30, 30),
            Color(255, 45, 45, 45),
            Color(255, 40, 40, 40),
            Color(255, 100, 100, 255),
            Color(255, 255, 255, 255),
            Color(255, 150, 150, 150) 
        };
    }
    curY += 35;

    if (SC_GUI::Button("theme_midblue", "Midnight Blue", x + 20, curY, 200, 30)) {
        SC_GUI::currentTheme = {
            Color(255, 15, 20, 30),
            Color(255, 20, 25, 40),
            Color(255, 40, 50, 70),
            Color(255, 30, 40, 60),
            Color(255, 30, 45, 65), // Separator
            Color(255, 0, 120, 215), // Blue Accent
            Color(255, 255, 255),
            Color(255, 170, 180, 200)
        };
    }
    curY += 45;
    
    // Config Section Header
    SC_GUI::DrawStringA("Configurations", x + 20, curY, Color(255, 100, 255, 100), SC_GUI::titleFont, false);
    curY += 40;

    RenderConfigTab(x, curY, w, h - (curY - y));
    RenderConfigTab(x, curY, w, h - (curY - y));
}

// --- Glove Tab ---
void RenderGloveTab(float x, float y, float w, float h)
{
    // Layout: Left column for Glove Types, Right area for Skins
    float typeW = 160.0f;
    float typeX = x + 10;
    float typeY = y + 10;
    
    // Background for Types
    SC_GUI::DrawFilledRoundedRect(typeX, typeY, typeW, h - 20, 8.0f, SC_GUI::currentTheme.sidebarBg);
    
    static int selectedGloveIdx = 0;
    float curY = typeY + 10;
    
    // Render Glove Types
    for (int i=0; i < (int)GloveTypes.size(); i++) {
         bool active = (selectedGloveIdx == i);
         // Mini Tab Button
         if (SC_GUI::TabButton("glove_type_" + std::to_string(i), GloveTypes[i].name, typeX + 5, curY, typeW - 10, 40, active)) {
             selectedGloveIdx = i;
             // Reset skin selection offset?
         }
         curY += 45;
    }

    // Right Side: Skins
    float skinX = x + typeW + 20;
    float skinY = y + 10;
    float skinW = w - typeW - 30;
    float skinH = h - 20;
    
    std::string currentGloveName = GloveTypes[selectedGloveIdx].name;
    
    std::vector<SkinInfo_t> gloves = skindb->GetGloveSkins(currentGloveName);
    
    // Grid Layout
    float cardW = 180;
    float cardH = 220;
    float gap = 15;
    int cols = (int)((skinW / (cardW + gap)));
    if (cols < 1) cols = 1;
    
    // Safety
    if (gloves.empty()) {
        SC_GUI::DrawStringA("No Skins Found", skinX + 20, skinY + 20, SC_GUI::currentTheme.text, SC_GUI::mainFont, false);
        return;
    }
    
    float totalH = ((gloves.size() + cols - 1) / cols) * (cardH + gap);
    
    // Scroll
    static float scrollY = 0.0f;
    if (SC_GUI::Input.mousePos.x >= skinX && SC_GUI::Input.mousePos.x <= skinX + skinW &&
        SC_GUI::Input.mousePos.y >= skinY && SC_GUI::Input.mousePos.y <= skinY + skinH) {
         scrollY += SC_GUI::Input.scrollDelta * 0.5f;
    }
    if (scrollY > 0) scrollY = 0;
    float maxScroll = -(totalH - skinH);
    if (maxScroll > 0) maxScroll = 0;
    if (scrollY < maxScroll) scrollY = maxScroll;

    SC_GUI::SetClip(skinX, skinY, skinW, skinH);
    
    int currentPaint = skinManager->Gloves.Paint;
    uint16_t currentDef = skinManager->Gloves.defIndex;
    
    for(int i=0; i<(int)gloves.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        
        float drawX = skinX + col * (cardW + gap);
        float drawY = skinY + scrollY + row * (cardH + gap);
        
        if (drawY + cardH < skinY || drawY > skinY + skinH) continue;
        
        // Selected?
        // We need to check if current Glove Def matches AND paint matches
        bool isSelected = (currentPaint == gloves[i].Paint && currentDef == GloveTypes[selectedGloveIdx].defIndex);
        
        if (SC_GUI::SkinCard("glove_" + std::to_string(i), gloves[i].name, gloves[i].image_url, gloves[i].rarity, drawX, drawY, cardW, cardH, isSelected)) {
            // Apply Glove
            skinManager->Gloves.defIndex = GloveTypes[selectedGloveIdx].defIndex;
            skinManager->Gloves.Paint = gloves[i].Paint;
            skinManager->Gloves.Paint = gloves[i].Paint;
            ForceUpdate = true; // Trigger update in main loop
        }
    }
    
    SC_GUI::ResetClip();
}

static int active_tab = 0;
bool MenuOpen = true;

void RenderMenu()
{
    float w = 1000;
    float h = 700;
    
    static float x = -1;
    static float y = -1;
    // Init Center
    if (x == -1) {
        x = (overlay::G_Width - w) / 2;
        y = (overlay::G_Height - h) / 2;
    }

    // Dragging Logic (Header Area)
    if (SC_GUI::Input.leftClick) {
        // Allow dragging entire top header
        if (SC_GUI::Input.mousePos.x >= x && SC_GUI::Input.mousePos.x <= x + w &&
            SC_GUI::Input.mousePos.y >= y && SC_GUI::Input.mousePos.y <= y + 60) {
            x += SC_GUI::Input.mouseDelta.x;
            y += SC_GUI::Input.mouseDelta.y;
        }
    }

    // Styles from Theme
    float sidebarW = 220.0f;
    float headerH = 60.0f;
    
    // Main Window Background
    SC_GUI::DrawRoundedRect(x, y, w, h, 8, SC_GUI::currentTheme.mainBg);
    
    // Sidebar Background
    SC_GUI::DrawRoundedRect(x, y, sidebarW, h, 8, SC_GUI::currentTheme.sidebarBg);

    // Vertical Separator
    SC_GUI::DrawRect(x + sidebarW, y, 1, h, SC_GUI::currentTheme.separator);

    // Window Border Outline
    SC_GUI::DrawStrokeRoundedRect(x, y, w, h, 8, SC_GUI::currentTheme.border, 1.0f);

    // --- Header / Logo Area (Top Left) ---
    // Dynamic Title
    static std::vector<std::string> titles = {
        "kami", "jeffrey", "azaki", "zotiq", "nir", 
        "jerem", "lbs", "rayene", "sosko", "zoulouu"
    };
    static int titleIdx = 0;
    static ULONGLONG lastChange = 0;
    ULONGLONG now = GetTickCount64();
    
    if (now - lastChange > 3000) {
        titleIdx = (titleIdx + 1) % titles.size();
        lastChange = now;
    }

    // Logo Text with Margin
    SC_GUI::DrawStringA(titles[titleIdx], x + 30, y + 30, SC_GUI::currentTheme.text, SC_GUI::titleFont, false);
    
    // --- Tabs (Sidebar) ---
    float tabY = y + 120;
    float tabH = 45;
    float tabW = sidebarW - 20; // Padding
    float tabX = x + 10;

    if (SC_GUI::TabButton("tab_wep", "Weapons", tabX, tabY, tabW, tabH, active_tab == 0)) active_tab = 0;
    if (SC_GUI::TabButton("tab_knife", "Knives", tabX, tabY + 50, tabW, tabH, active_tab == 2)) active_tab = 2;
    if (SC_GUI::TabButton("tab_gloves", "Gloves", tabX, tabY + 100, tabW, tabH, active_tab == 4)) active_tab = 4;
    if (SC_GUI::TabButton("tab_music", "Music Kits", tabX, tabY + 150, tabW, tabH, active_tab == 1)) active_tab = 1;
    if (SC_GUI::TabButton("tab_settings", "Settings", tabX, tabY + 200, tabW, tabH, active_tab == 3)) active_tab = 3;

    // --- Content Area ---
    float cX = x + sidebarW;
    float cY = y;
    float cW = w - sidebarW;
    float cH = h;

    // Clip Content Area
    SC_GUI::SetClip(cX, cY, cW, cH);
    switch (active_tab) {
        case 0: RenderWeaponTab(cX, cY, cW, cH); break;
        case 1: RenderMusicTab(cX, cY, cW, cH); break;
        case 2: RenderKnifeTab(cX, cY, cW, cH); break;
        case 3: RenderSettingsTab(cX, cY, cW, cH); break;
        case 4: RenderGloveTab(cX, cY, cW, cH); break;
    }
    SC_GUI::ResetClip();
}

void OnFrame()
{
     // Toggle Logic
    static bool prevInsert = false;
    bool insert = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    if (insert && !prevInsert) {
        MenuOpen = !MenuOpen;
        overlay::SetInput(MenuOpen);
    }
    prevInsert = insert;

    // Pass our RenderMenu function to the overlay loop
    overlay::Render(RenderMenu, MenuOpen); 
}

void MenuThread()
{
	while (!overlay::ShouldQuit)
	{
		OnFrame(); 
	}
}

void UpdateActiveMenuDef(const uintptr_t localPlayer)
{
	const uintptr_t activeWeapon = mem.Read<uintptr_t>(localPlayer + Offsets::m_pClippingWeapon);
	const uintptr_t activeItem = activeWeapon + Offsets::m_AttributeManager + Offsets::m_Item;
	CurrentWeaponDef = mem.Read<WeaponsEnum>(activeItem + Offsets::m_iItemDefinitionIndex);
}

void InitMenu(const bool autoThread = false)
{
	overlay::Setup();
    if (autoThread) {
        std::thread menuthread(MenuThread);
        menuthread.detach();
    }
	std::cout << "GDI+ Menu Setup\n";
}