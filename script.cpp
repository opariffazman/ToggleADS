#include "script.h"
#include "keyboard.h"
#include <fstream>

using namespace std;

int getGameTimer()
{
	return MISC::GET_GAME_TIMER();
}

Hash key(const char* input)
{
	return MISC::GET_HASH_KEY(input);
}

bool isFileExist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

// both of these need to be called each frame
void forceAimDownSights()
{
	PLAYER::SET_PLAYER_FORCED_AIM(PLAYER::PLAYER_ID(), true, NULL, 0, true);
}

void cancelAimDownSights()
{
	PLAYER::SET_PLAYER_FORCED_AIM(PLAYER::PLAYER_ID(), false, NULL, 0, true);
}

void forceFirstPerson()
{
	CAM::_0x90DA5BA5C2635416();
}

void main()
{
	bool configIni = isFileExist(".\\ToggleADS.ini");
	int shortPress = GetPrivateProfileInt("Settings", "ShortPress", 50, ".\\ToggleADS.ini");
	int longPress = GetPrivateProfileInt("Settings", "LongPress", 100, ".\\ToggleADS.ini");
	bool holdADS = GetPrivateProfileInt("Settings", "HoldADS", 0, ".\\ToggleADS.ini");

	bool firstPersonMode{ false }; // a totally separate way of keep tracking when this mod is activated as no native to check for when in first person
	bool isUnarmed{ false };
	Hash weaponUnarmed;

	int timeTapped{ 0 };

	while (true)
	{
		if (PAD::IS_CONTROL_JUST_PRESSED(25, key("INPUT_AIM")) && !GRAPHICS::ANIMPOSTFX_IS_RUNNING("WheelHUDin")) // a new press event occurs, aim key just pressed
		{
			timeTapped = getGameTimer();
		}

		if (PAD::IS_CONTROL_JUST_RELEASED(25, key("INPUT_AIM")) && !GRAPHICS::ANIMPOSTFX_IS_RUNNING("WheelHUDin")) // no longer pressed for how long
		{
			if ((getGameTimer() - timeTapped >= shortPress) && !(getGameTimer() - timeTapped >= longPress)) // TRUE & TRUE handle short press, if pass, then its a tap, else a hold
			{
				firstPersonMode = (firstPersonMode == 1) ? 0 : 1; // toggle to first person whenever in third person and vice versa
			}
		}

		if (!holdADS)
		{
			if (firstPersonMode) // a condition thats toggled by each tap on the aim input (RMB/LT/L2/ZL)
			{
				forceFirstPerson(); // sets to first person camera forcefully every frame
				if (PAD::IS_CONTROL_PRESSED(21, key("INPUT_SPRINT")) || isUnarmed) // so player can still sprint and loot in firstpersonmode when unarmed
					cancelAimDownSights();
				else
					forceAimDownSights(); // to ensure its toggled, still need to zoom manually to ADS for the first time
			}
			else
				cancelAimDownSights(); // set back to no force and return back to player last perspective

			if (WEAPON::GET_CURRENT_PED_WEAPON(PLAYER::PLAYER_PED_ID(), &weaponUnarmed, false, 0, false))
				isUnarmed = (weaponUnarmed == key("WEAPON_UNARMED"));
		}
		else
		{
			if (firstPersonMode) 
				forceFirstPerson();
		}
		
		if (firstPersonMode && PAD::IS_CONTROL_PRESSED(0, key("INPUT_NEXT_CAMERA")) && !GRAPHICS::ANIMPOSTFX_IS_RUNNING("WheelHUDin")) // in firstpersonmode, pressed v when not in wheelhud
			firstPersonMode = false; // so can return back from firstperson mode using input next camera

		WAIT(0);
	}
}

void ScriptMain()
{
	srand(static_cast<int>(GetTickCount64()));
	main();
}
