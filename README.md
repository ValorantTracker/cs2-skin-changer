# External Skinchanger for CS2
This tool uses low level C++ Win32 API to interact with the game’s memory to make skins change.
## Features:
  * dynamic weapon recognition
  * color themes
  * music kits (including new ones)
  * GDI+ skinchanger menu
  * Uses https://github.com/ByMykel/CSGO-API for weapon icons and item rarity
  * Supports old and new weapon models
  * Config manager which loads skins that were last used
  * Trusted Dumper: Uses the https://github.com/a2x/cs2-dumper.git  You can compile the dumper yourself if you prefer.
## Requirements: 
* Visual Studio 2022 with "Desktop development with C++" workload installed.
* Counter-Strike 2
## How to build:
First clone my repo with this command: \
`git clone https://github.com/Batman123n/cs2-skin-changer.git`

### Choose one of the following options to build the tool:

Option 1 (Recommended):
  1. Open CS2
  2. Run build.bat
 * This will automatically fetch fresh offsets and compile the tool.
   
Option 2 (Manual):
  1. Open CS2
  2. Run cs2-dumper.exe to get fresh offsets
  3. Run build.bat
 * This option requires you to run the dumper separately from the build.bat script.
#### All weapon skins work reliably and tool is quite stable in matchmaking.
#### Knives and gloves are not yet supported.
### Disclaimer:
#### Use this skinchanger at your own risk. Even if chances of detection are small they are never zero.
#### Do not use this on any account that has skins of value.
#### In case if game updates you literally just dump the offsets and recompile, or run build.bat with game open to dump offsets automatically.
#### Credits: 
  * wompwomp6 - Provided the base tool.
  * ValorantTracker - Developed the GDI+ menu on top of the base.
  * a2x - Provided the offset dumper.
  * ByMykel - Provided CSGO-API: https://github.com/ByMykel/CSGO-API
## In-game screenshots:
First one is from deathmatch and other is skinchanger menu showcase.
| Deathmatch Showcase | Skinchanger Menu |
| :---: | :---: |
| <img width="550" alt="Deathmatch Showcase" src="https://github.com/user-attachments/assets/5a407dd4-7fae-4beb-bc4a-07f771c54387" /> | <img width="500" alt="Menu Showcase" src="https://github.com/user-attachments/assets/8c0afd52-eb76-493f-8faa-343f4534da00" /> |

