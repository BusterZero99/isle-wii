#include "config.h"

#include <SDL3/SDL_log.h>
#include <iniparser.h>

void WII_SetupDefaultConfigOverrides(dictionary* p_dictionary)
{
	SDL_Log("Overriding default config for Wii");

	iniparser_set(p_dictionary, "isle:diskpath", "sd:/apps/isle/LEGO/");
	iniparser_set(p_dictionary, "isle:cdpath", "sd:/apps/isle/");
	iniparser_set(p_dictionary, "isle:savepath", "sd:/apps/isle/");

	iniparser_set(p_dictionary, "isle:Cursor Sensitivity", "16.000000");

	iniparser_set(p_dictionary, "isle:Full Screen", "true");
	iniparser_set(p_dictionary, "isle:Exclusive Full Screen", "false");
	iniparser_set(p_dictionary, "isle:Wide View Angle", "false");

	iniparser_set(p_dictionary, "isle:Touch Scheme", "0");

	iniparser_set(p_dictionary, "isle:3DSound", "true");
	iniparser_set(p_dictionary, "isle:Music", "true");
	iniparser_set(p_dictionary, "isle:Display Bit Depth", "16");
	iniparser_set(p_dictionary, "isle:Back Buffers in Video RAM", "false");
	iniparser_set(p_dictionary, "isle:MSAA", "0");
	iniparser_set(p_dictionary, "isle:Anisotropic", "0");
	iniparser_set(p_dictionary, "isle:Render Style", "0");
	iniparser_set(p_dictionary, "isle:Active in background", "false");
}