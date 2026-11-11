#include "input.h"

#include <ogc/pad.h>
#include <ogc/wpad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

static SDL_JoystickID g_wiiGamepadId = 0;

static void Wii_PushGamepadButton(SDL_GamepadButton button, bool pressed)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = pressed ? SDL_EVENT_GAMEPAD_BUTTON_DOWN : SDL_EVENT_GAMEPAD_BUTTON_UP;
	event.gbutton.which = g_wiiGamepadId;
	event.gbutton.button = button;
	SDL_PushEvent(&event);
}

static void Wii_PushGamepadAxis(SDL_GamepadAxis axis, Sint16 value)
{
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
	event.gaxis.which = g_wiiGamepadId;
	event.gaxis.axis = axis;
	event.gaxis.value = value;
	SDL_PushEvent(&event);
}

static void Wii_MapGameCubePad(SDL_JoystickID id, PADStatus* pad)
{
	if (!pad) {
		return;
	}

	g_wiiGamepadId = id;

	// Main stick to right-stick emulation.
	Sint16 stickX = PAD_StickX(0);
	Sint16 stickY = PAD_StickY(0);
	const int deadzone = 10;
	if (abs(stickX) < deadzone) {
		stickX = 0;
	}
	if (abs(stickY) < deadzone) {
		stickY = 0;
	}
	Wii_PushGamepadAxis(SDL_GAMEPAD_AXIS_RIGHTX, SDL_clamp(stickX * 32767 / 100, -32767, 32767));
	Wii_PushGamepadAxis(SDL_GAMEPAD_AXIS_RIGHTY, SDL_clamp(-stickY * 32767 / 100, -32767, 32767));

	// D-pad to standard SDL gamepad D-pad buttons.
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_DPAD_UP, (pad->button & PAD_BUTTON_UP) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN, (pad->button & PAD_BUTTON_DOWN) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_DPAD_LEFT, (pad->button & PAD_BUTTON_LEFT) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, (pad->button & PAD_BUTTON_RIGHT) != 0);

	// A / B / Start map into the same semantic buttons the app already expects.
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH, (pad->button & PAD_BUTTON_A) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_EAST, (pad->button & PAD_BUTTON_B) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_START, (pad->button & PAD_BUTTON_START) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_BACK, (pad->button & PAD_BUTTON_MENU) != 0);
}

static void Wii_MapWiiRemote(SDL_JoystickID id, s32 chan)
{
	if (WPAD_Probe(chan, NULL) != 0) {
		return;
	}

	g_wiiGamepadId = id;

	u32 buttons = WPAD_ButtonsHeld(chan);
	WPADData* data = WPAD_Data(chan);

	if (data && data->exp.type == WPAD_EXP_NUNCHUK) {
		s16 stickX = data->exp.nunchuk.js.pos.x;
		s16 stickY = data->exp.nunchuk.js.pos.y;
		const int deadzone = 10;
		if (abs(stickX) < deadzone) {
			stickX = 0;
		}
		if (abs(stickY) < deadzone) {
			stickY = 0;
		}
		Wii_PushGamepadAxis(SDL_GAMEPAD_AXIS_RIGHTX, SDL_clamp(stickX * 32767 / 100, -32767, 32767));
		Wii_PushGamepadAxis(SDL_GAMEPAD_AXIS_RIGHTY, SDL_clamp(-stickY * 32767 / 100, -32767, 32767));
	}

	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH, (buttons & WPAD_BUTTON_A) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_EAST, (buttons & WPAD_BUTTON_B) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_START, (buttons & WPAD_BUTTON_PLUS) != 0);
	Wii_PushGamepadButton(SDL_GAMEPAD_BUTTON_BACK, (buttons & WPAD_BUTTON_MINUS) != 0);
}

void WII_InputInit(void)
{
	SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
	PAD_Init();
	WPAD_Init();
	SDL_InitSubSystem(SDL_INIT_GAMEPAD);

	g_wiiGamepadId = 0;
}

void WII_InputShutdown(void)
{
	g_wiiGamepadId = 0;
	WPAD_Shutdown();
	PAD_Shutdown();
	SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

void WII_InputPoll(void)
{
	PAD_ScanPads();
	WPAD_ScanPads();

	// Poll the first available GameCube controller.
	PADStatus* pad = PAD_Read(0);
	if (pad) {
		Wii_MapGameCubePad(0, pad);
		return;
	}

	// Fall back to a Wii Remote mapping for pointer-style controls.
	for (int i = 0; i < 4; ++i) {
		if (WPAD_Probe(i, NULL) == 0) {
			Wii_MapWiiRemote(i, i);
			return;
		}
	}

	g_wiiGamepadId = 0;
}
