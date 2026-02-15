#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <gdiplus.h>
#include <cctype>

#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;

namespace SC_GUI {
    // Animation State
    struct AnimState {
        float value = 0.0f;
        float target = 0.0f;
        
        void Update(float speed = 0.1f) {
            value = value + (target - value) * speed;
        }
    };

    inline std::map<std::string, AnimState> animations;
    
    // Theme Support
    struct Theme {
        Color mainBg;
        Color sidebarBg;
        Color contentBg;
        Color border;
        Color separator;
        Color accent;
        Color text;
        Color textDim;
    };
    
    inline Theme currentTheme = {
        Color(255, 12, 12, 12),      // mainBg (Deep Dark)
        Color(255, 20, 20, 20),      // sidebarBg (Slightly lighter)
        Color(255, 30, 30, 30),      // contentBg (Cards/Inputs)
        Color(255, 45, 45, 45),      // border
        Color(255, 40, 40, 40),      // separator
        Color(255, 100, 100, 255),   // accent (Vibrant Blue/Purple default) - actually lets go Red as per user history, but modern: #FF4444
        Color(255, 255, 255, 255),   // text
        Color(255, 150, 150, 150)    // textDim
    };

    // Global Input State (Same as before)
    struct InputState {
        POINT mousePos;
        POINT prevMousePos = {0, 0};
        POINT mouseDelta = {0, 0};
        bool leftClick = false;
        bool leftClicked = false;
        bool prevLeftClick = false;
        char keys[256] = {0};
        int scrollDelta = 0;
        bool firstUpdate = true;
        
        char lastChar = 0;
        std::string activeID = "";

        void Update() {
            if (firstUpdate) {
                GetCursorPos(&mousePos);
                prevMousePos = mousePos;
                firstUpdate = false;
            }

            prevMousePos = mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(GetForegroundWindow(), &mousePos); // Map to client area roughly

            mouseDelta.x = mousePos.x - prevMousePos.x;
            mouseDelta.y = mousePos.y - prevMousePos.y;

            prevLeftClick = leftClick;
            leftClick = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
            leftClicked = leftClick && !prevLeftClick;
        }
    } Input;

    // GDI+ Globals
    inline ULONG_PTR gdiplusToken;
    inline Graphics* gfx = nullptr;
    inline Font* mainFont = nullptr;
    inline Font* largeFont = nullptr;
    inline Font* smallFont = nullptr;
    inline Font* titleFont = nullptr; // Bold Header
    inline SolidBrush* brush = nullptr;
    inline Pen* pen = nullptr;
    inline StringFormat* centerFormat = nullptr;
    inline StringFormat* leftFormat = nullptr;
    inline StringFormat* vCenterFormat = nullptr;

    inline void Init() {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }

    inline void BeginFrame(Graphics* g) {
        gfx = g;
        gfx->SetSmoothingMode(SmoothingModeAntiAlias);
        gfx->SetTextRenderingHint(TextRenderingHintClearTypeGridFit); // Better text
        gfx->SetInterpolationMode(InterpolationModeHighQualityBicubic);

        if (!mainFont) {
            // Modern Fonts: Segoe UI
            FontFamily fontFamily(L"Segoe UI");
            mainFont = new Font(&fontFamily, 10, FontStyleRegular, UnitPoint);
            largeFont = new Font(&fontFamily, 12, FontStyleBold, UnitPoint);
            smallFont = new Font(&fontFamily, 9, FontStyleRegular, UnitPoint);
            titleFont = new Font(&fontFamily, 24, FontStyleBold, UnitPoint); 

            brush = new SolidBrush(Color(255, 255, 255));
            pen = new Pen(Color(255, 255, 255));
            
            centerFormat = new StringFormat();
            centerFormat->SetAlignment(StringAlignmentCenter);
            centerFormat->SetLineAlignment(StringAlignmentCenter);

            leftFormat = new StringFormat();
            leftFormat->SetAlignment(StringAlignmentNear);
            leftFormat->SetLineAlignment(StringAlignmentNear); // Top-Left

            vCenterFormat = new StringFormat();
            vCenterFormat->SetAlignment(StringAlignmentNear);
            vCenterFormat->SetLineAlignment(StringAlignmentCenter);
        }
        
        Input.Update();
    }

    // Helpers & Primitives
    inline void SetClip(float x, float y, float w, float h) {
        RectF rect((REAL)x, (REAL)y, (REAL)w, (REAL)h);
        gfx->SetClip(rect);
    }

    inline void ResetClip() {
        gfx->ResetClip();
    }

    inline void UpdateAnimation(const std::string& id, float target, float speed) {
        if (animations.find(id) == animations.end()) animations[id] = {0.f, 0.f};
        animations[id].target = target;
        animations[id].Update(speed);
    }

    inline Color InterpColor(Color c1, Color c2, float t) {
        // Clamp t
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return Color(
            (BYTE)(c1.GetA() + (c2.GetA() - c1.GetA()) * t),
            (BYTE)(c1.GetR() + (c2.GetR() - c1.GetR()) * t),
            (BYTE)(c1.GetG() + (c2.GetG() - c1.GetG()) * t),
            (BYTE)(c1.GetB() + (c2.GetB() - c1.GetB()) * t)
        );
    }
    
    // Modern Rounded Rect
    inline GraphicsPath* CreateRoundedRectPath(float x, float y, float w, float h, float r) {
        GraphicsPath* path = new GraphicsPath();
        path->AddArc((REAL)x, (REAL)y, (REAL)r, (REAL)r, 180.0f, 90.0f);
        path->AddArc((REAL)(x + w - r), (REAL)y, (REAL)r, (REAL)r, 270.0f, 90.0f);
        path->AddArc((REAL)(x + w - r), (REAL)(y + h - r), (REAL)r, (REAL)r, 0.0f, 90.0f);
        path->AddArc((REAL)x, (REAL)(y + h - r), (REAL)r, (REAL)r, 90.0f, 90.0f);
        path->CloseFigure();
        return path;
    }

    inline void DrawFilledRoundedRect(float x, float y, float w, float h, float r, Color color) {
        GraphicsPath* path = CreateRoundedRectPath(x, y, w, h, r * 2); 

        float d = r * 2;
        GraphicsPath p;
        p.AddArc((REAL)x, (REAL)y, (REAL)d, (REAL)d, 180.0f, 90.0f);
        p.AddArc((REAL)(x + w - d), (REAL)y, (REAL)d, (REAL)d, 270.0f, 90.0f);
        p.AddArc((REAL)(x + w - d), (REAL)(y + h - d), (REAL)d, (REAL)d, 0.0f, 90.0f);
        p.AddArc((REAL)x, (REAL)(y + h - d), (REAL)d, (REAL)d, 90.0f, 90.0f);
        p.CloseFigure();

        brush->SetColor(color);
        gfx->FillPath(brush, &p);
    }

    inline void DrawRoundedRect(float x, float y, float w, float h, float r, Color color) {
        DrawFilledRoundedRect(x, y, w, h, r, color);
    }

    inline void DrawStrokeRoundedRect(float x, float y, float w, float h, float r, Color color, float thickness = 1.0f) {
        float d = r * 2;
        GraphicsPath p;
        p.AddArc((REAL)x, (REAL)y, (REAL)d, (REAL)d, 180.0f, 90.0f);
        p.AddArc((REAL)(x + w - d), (REAL)y, (REAL)d, (REAL)d, 270.0f, 90.0f);
        p.AddArc((REAL)(x + w - d), (REAL)(y + h - d), (REAL)d, (REAL)d, 0.0f, 90.0f);
        p.AddArc((REAL)x, (REAL)(y + h - d), (REAL)d, (REAL)d, 90.0f, 90.0f);
        p.CloseFigure();

        pen->SetColor(color);
        pen->SetWidth(thickness);
        gfx->DrawPath(pen, &p);
    }
    
    inline void DrawStringA(const std::string& text, float x, float y, Color color, Font* f = mainFont, StringFormat* format = leftFormat) {
        std::wstring wtext(text.begin(), text.end());
        brush->SetColor(color);
        PointF p((REAL)x, (REAL)y);
        gfx->DrawString(wtext.c_str(), -1, f, p, format, brush);
    }

    // Unsigned bool overload for backward compatibility with menu.h
    inline void DrawStringA(const std::string& text, float x, float y, Color color, Font* f, bool center) {
        DrawStringA(text, x, y, color, f, center ? centerFormat : leftFormat);
    }
    
    inline void DrawRect(float x, float y, float w, float h, Color color) {
        brush->SetColor(color);
        gfx->FillRectangle(brush, (REAL)x, (REAL)y, (REAL)w, (REAL)h);
    }

    // Widgets
    inline bool Button(const std::string& id, const std::string& text, float x, float y, float w, float h, bool selected = false) {
        bool hovered = (Input.mousePos.x >= x && Input.mousePos.x <= x + w && Input.mousePos.y >= y && Input.mousePos.y <= y + h);
        bool clicked = hovered && Input.leftClicked;

        UpdateAnimation(id, hovered ? 1.0f : 0.0f, 0.2f);
        float anim = animations[id].value;

        // Colors
        Color base = currentTheme.contentBg;
        Color hover = Color(255, 60, 60, 60);
        Color active = currentTheme.accent;

        Color bg = selected ? active : InterpColor(base, hover, anim);
        
        DrawFilledRoundedRect(x, y, w, h, 6.0f, bg);
        // Center text
        DrawStringA(text, x + w/2, y + h/2, Color(255, 255, 255), mainFont, centerFormat);

        return clicked;
    }

    // --- Widgets ---

    // Modern Sidebar Tab
    inline bool TabButton(const std::string& id, const std::string& text, float x, float y, float w, float h, bool active, const std::string& icon = "") {
        bool hovered = (Input.mousePos.x >= x && Input.mousePos.x <= x + w && Input.mousePos.y >= y && Input.mousePos.y <= y + h);
        bool clicked = hovered && Input.leftClicked;

        UpdateAnimation(id, (hovered || active) ? 1.0f : 0.0f, 0.2f);
        float anim = animations[id].value;

        // Background (Subtle highlight)
        if (anim > 0.01f) {
            Color bg = Color((BYTE)(20 * anim), 255, 255, 255); // Very subtle white overlay
            DrawFilledRoundedRect(x, y, w, h, 8.0f, bg);
        }

        // Accent Indicator (Left Side Pill)
        if (active) {
            DrawFilledRoundedRect(x, y + 8, 4, h - 16, 2.0f, currentTheme.accent);
        }

        // Icon + Text
        float contentX = x + 20;
        // If we had icons, draw here.
        
        Color txtColor = InterpColor(currentTheme.textDim, currentTheme.text, active ? 1.0f : anim);
        if (active) txtColor = currentTheme.accent; // Highlight text if active? Or just white? Let's go Accent.

        // Use vCenterFormat
        brush->SetColor(txtColor);
        RectF layout((REAL)contentX, (REAL)y, (REAL)(w - 30), (REAL)h);
        std::wstring wtext(text.begin(), text.end());
        gfx->DrawString(wtext.c_str(), -1, largeFont, layout, vCenterFormat, brush);

        return clicked;
    }

    inline Color GetRarityColor(int rarity) {
        // Updated for vibrancy
        switch(rarity) {
            case 1: return Color(255, 170, 170, 170); // Common - Grey
            case 2: return Color(255, 130, 200, 250); // Uncommon - Light Blue
            case 3: return Color(255, 75, 105, 255);  // Rare - Blue
            case 4: return Color(255, 136, 71, 255);  // Mythical - Purple
            case 5: return Color(255, 211, 44, 230);  // Legendary - Pink
            case 6: return Color(255, 235, 75, 75);   // Ancient - Red
            case 7: return Color(255, 228, 174, 57);  // Immortal - Gold
            default: return Color(255, 150, 150, 150);
        }
    }

    // Redesigned SkinCard
    inline bool SkinCard(const std::string& id, const std::string& name, const std::string& image_url, int rarity, float x, float y, float w, float h, bool selected) {
        bool hovered = (Input.mousePos.x >= x && Input.mousePos.x <= x + w && Input.mousePos.y >= y && Input.mousePos.y <= y + h);
        bool clicked = hovered && Input.leftClicked;

        UpdateAnimation(id, (hovered || selected) ? 1.0f : 0.0f, 0.2f);
        float anim = animations[id].value;

        // Base Setup
        float radius = 12.0f;
        
        // Background
        Color bgNormal = currentTheme.contentBg;
        Color bgActive = Color(255, 50, 50, 60); // Slightly lighter
        
        // If selected, maybe use a tinted background or just a border
        // Let's use a subtle border for selected, and light bg for hover
        Color bg = InterpColor(bgNormal, Color(255, 45, 45, 50), anim);
        if (selected) bg = Color(255, 40, 40, 45);

        DrawFilledRoundedRect(x, y, w, h, radius, bg);

        // Border
        Color borderColor = currentTheme.border;
        if (selected) borderColor = currentTheme.accent;
        else if (hovered) borderColor = Color(255, 100, 100, 100);
        
        // If Rarity is high (RED/GOLD), maybe glow the border?
        Color rarityCol = GetRarityColor(rarity);
        if (selected || hovered) {
             // Mix accent with rarity? 
             borderColor = rarityCol;
        }

        DrawStrokeRoundedRect(x, y, w, h, radius, borderColor, selected ? 2.0f : 1.0f);

        // --- Image Handling ---
        static std::map<std::string, Image*> imageCache;
        static std::map<std::string, Image*> thumbCache;
        static std::map<std::string, bool> downloadStarted;

         // Helper for raw bytes
        struct MemoryStruct {
            char* memory;
            size_t size;
        };
        
        // Download Check
        if (thumbCache.find(name) == thumbCache.end() && !downloadStarted[name] && !image_url.empty()) {
             downloadStarted[name] = true;
             std::thread([name, image_url]() {
                CURL* curl = curl_easy_init();
                if (curl) {
                    MemoryStruct chunk;
                    chunk.memory = (char*)malloc(1);
                    chunk.size = 0;
                    curl_easy_setopt(curl, CURLOPT_URL, image_url.c_str());
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
                        size_t realsize = size * nmemb;
                        MemoryStruct* mem = (MemoryStruct*)userp;
                        char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
                        if(!ptr) return 0;
                        mem->memory = ptr;
                        memcpy(&(mem->memory[mem->size]), contents, realsize);
                        mem->size += realsize;
                        mem->memory[mem->size] = 0;
                        return realsize;
                    });
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    CURLcode res = curl_easy_perform(curl);
                    if (res == CURLE_OK) {
                        IStream* pStream = NULL;
                        if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK) {
                            pStream->Write(chunk.memory, (ULONG)chunk.size, NULL);
                            pStream->Seek({0}, STREAM_SEEK_SET, NULL);
                            Image* pImg = Image::FromStream(pStream);
                            if (pImg && pImg->GetLastStatus() == Ok) {
                                // Create Scaled Thumbnail (Larger for quality)
                                int tW = 200; int tH = 150; 
                                Bitmap* thumb = new Bitmap(tW, tH, PixelFormat32bppARGB);
                                Graphics* g = Graphics::FromImage(thumb);
                                g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
                                g->DrawImage(pImg, 0, 0, tW, tH);
                                delete g;
                                delete pImg; // Free original
                                thumbCache[name] = thumb;
                            } else { delete pImg; }
                            pStream->Release(); 
                        }
                    }
                    free(chunk.memory);
                    curl_easy_cleanup(curl);
                }
             }).detach();
        }

        // Draw Image
        float imgAreaH = h * 0.65f;
        float imgPad = 10.0f;
        if (thumbCache.find(name) != thumbCache.end() && thumbCache[name]) {
             Image* img = thumbCache[name];
             float imgW = (float)img->GetWidth();
             float imgH = (float)img->GetHeight();
             
             // Fit within area
             float maxW = w - (imgPad * 2);
             float maxH = imgAreaH - (imgPad * 2);
             
             float ratio = min(maxW / imgW, maxH / imgH);
             float drawW = imgW * ratio;
             float drawH = imgH * ratio;
             
             float drawX = x + (w - drawW) / 2;
             float drawY = y + imgPad + (maxH - drawH) / 2; // Vertically center in top area

             gfx->DrawImage(img, (REAL)drawX, (REAL)drawY, (REAL)drawW, (REAL)drawH);
        } else {
             // Placeholder
        }

        // Text Area
        float textY = y + imgAreaH;
        
        // Name
        brush->SetColor(currentTheme.text);
        RectF nameLayout((REAL)(x + 8), (REAL)textY, (REAL)(w - 16), (REAL)20);
        // Truncate if too long? StringFormat handles wrapping/trimming usually.
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentNear);
        format.SetTrimming(StringTrimmingEllipsisCharacter);
        
        // Clean Name (Remove "Weapon | ")
        std::string cleanName = name;
        size_t pipePos = cleanName.find('|');
        if (pipePos != std::string::npos) {
            cleanName = cleanName.substr(pipePos + 1);
            // Trim leading space
            if (!cleanName.empty() && cleanName[0] == ' ') cleanName.erase(0, 1);
        }

        std::wstring wname(cleanName.begin(), cleanName.end());
        gfx->DrawString(wname.c_str(), -1, mainFont, nameLayout, &format, brush);

        // Rarity Dot/Text ???
        // Maybe just a small colored dot
        float dotSize = 6.0f;
        float dotX = x + w / 2 - dotSize / 2;
        float dotY = textY + 24;
        DrawFilledRoundedRect(dotX, dotY, dotSize, dotSize, dotSize, rarityCol);

        return clicked;
    }

    // Modern Text Input
    inline void TextInput(const std::string& id, char* buffer, size_t size, float x, float y, float w, float h, const std::string& displayHint) {
        bool hovered = (Input.mousePos.x >= x && Input.mousePos.x <= x + w && Input.mousePos.y >= y && Input.mousePos.y <= y + h);
        bool focused = (Input.activeID == id);

        if (Input.leftClicked) {
             if (hovered) Input.activeID = id;
             else if (focused) Input.activeID = ""; 
        }

        // Logic
        if (focused && Input.lastChar != 0) {
            size_t len = strlen(buffer);
            if (Input.lastChar == VK_BACK) {
                if (len > 0) buffer[len - 1] = '\0';
            }
            else if (Input.lastChar == VK_RETURN) { 
                Input.activeID = ""; // Commit
            }
            else if (len < size - 1 && isprint(Input.lastChar)) {
                buffer[len] = Input.lastChar;
                buffer[len + 1] = '\0';
            }
        }

        // Visual
        Color bg = currentTheme.contentBg;
        Color border = currentTheme.border;
        if (focused) border = currentTheme.accent;
        else if (hovered) border = Color(255, 80, 80, 80);

        DrawFilledRoundedRect(x, y, w, h, 8.0f, bg);
        DrawStrokeRoundedRect(x, y, w, h, 8.0f, border, focused ? 1.5f : 1.0f);

        // Icon (Search Magnifier) - Fake it with text "O" or similar? Nah just Padding.
        float textPad = 12.0f;
        
        std::string content = buffer;
        Color txtCol = currentTheme.text;
        
        if (content.empty() && !focused) {
            content = displayHint;
            txtCol = currentTheme.textDim;
        }
        if (focused) content += "|";

        RectF layout((REAL)(x + textPad), (REAL)y, (REAL)(w - textPad * 2), (REAL)h);
        brush->SetColor(txtCol);
        gfx->DrawString(std::wstring(content.begin(), content.end()).c_str(), -1, mainFont, layout, vCenterFormat, brush);
    }
    
    // Modern Slider
    inline bool Slider(const std::string& id, float* value, float min, float max, float x, float y, float w, float h) {
        bool hovered = (Input.mousePos.x >= x && Input.mousePos.x <= x + w && Input.mousePos.y >= y && Input.mousePos.y <= y + h);
        bool down = hovered && Input.leftClick;
        bool changed = false;

        if (down || (Input.leftClick && Input.activeID == id)) {
            Input.activeID = id;
            float mouseX = (float)Input.mousePos.x;
            float ratio = (mouseX - x) / w;
            if (ratio < 0.0f) ratio = 0.0f;
            if (ratio > 1.0f) ratio = 1.0f;
            *value = min + ratio * (max - min);
            changed = true;
        } else if (Input.activeID == id && !Input.leftClick) {
            Input.activeID = "";
        }

        // Rail
        float railH = 4.0f;
        float railY = y + (h - railH) / 2;
        DrawFilledRoundedRect(x, railY, w, railH, railH, Color(255, 40, 40, 40));

        // Active Rail
        float ratio = (*value - min) / (max - min);
        DrawFilledRoundedRect(x, railY, w * ratio, railH, railH, currentTheme.accent);

        // Knob (Glowy)
        float knobSize = 16.0f;
        float knobX = x + (w * ratio) - (knobSize / 2);
        float knobY = y + (h - knobSize) / 2;
        
        // Shadow/Glow under knob?
        
        DrawFilledRoundedRect(knobX, knobY, knobSize, knobSize, knobSize, Color(255, 255, 255, 255));
        
        return changed;
    }
}
