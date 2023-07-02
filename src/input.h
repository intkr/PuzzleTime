#pragma once

#include <map>
#include <vector>

#include "SDL.h"

#include "core.h"
#include "graphics.h"
#include "sprite.h"

enum MouseStatus {
	 _MOUSE_UP, _MOUSE_ACTIVE, _MOUSE_PASSIVE
};

class Input {
public:
	friend class Core;

	Input(Core* _core);
	~Input();

	// Gets mouse / keyboard input from SDL2, and stores its data accordingly.
	void update(SDL_Event& e);
	
	// Contains all keyboard scancodes that has been pressed.
	// map.first : Object name identifier
	// map.second : Click status (true = active, false = passive)
	std::map<SDL_Scancode, bool>* getPressedKeys();

	// NOTE: Add released keys if needed
	
	// Contains all object identifiers that has been clicked.
	// map.first : Object name identifier
	// map.second : Click status (true = active, false = passive)
	std::map<std::string, bool>* getClickedObject();

	// Contains all object identifiers that were released this frame.
	std::vector<std::string>* getReleasedObject();

	std::vector<std::string>* getHoveredObject();

	// TODO: Function name kinda sucks, rewrite it to a better one later
	//
	// Checks if the Scancode corresponds to any of the 12-keys for both hands
	// If found, returns the index of said key (row * 3 + column, 0~11)
	// Otherwise, returns -1.
	int checkKeybinds(SDL_Scancode key);

	bool isQuitTriggered() { return quitTriggered; }

private:
	// Updates all active keys/objects in pressedKeys and clickedObjects to passive ones.
	// Erases all previously released objects from releasedObjects.
	void flushInput();

	void pollInput(SDL_Event& e);

	// Handles keyboard input.
	// type should be either SDL_KEYDOWN or SDL_KEYUP.
	void pollKey(SDL_Scancode inputKey, Uint32 type);

	// Handles mouse input. (Currently left click only)
	void pollMouse(int x, int y);

	void handleInput();

	// Stores the current status of the left mouse button.
	// 0 : Up
	// 1 : Down (active click)
	// 2 : Down (passive click)
	short mouseStatus;

	int curX, curY;
	SDL_Scancode lefthandKeys[4][3];
	SDL_Scancode righthandKeys[4][3];
	
	// Stores all keys that are being pressed.
	// If it's an active key (it was just pressed this frame), then the bool value is false.
	// If it's a passive key (it has been pressed since before), then the bool value is true.
	std::map<SDL_Scancode, bool> pressedKeys;

	// Stores all names of objects where the mouse button is held down within its hitbox.
	// If it's an active click (it was just clicked this frame), then the bool value is false.
	// If it's a passive click (it has been clicked since before), then the bool value is true.
	std::map<std::string, bool> clickedObject;

	// Stores all names of objects where the mouse button is released within its hitbox.
	// Use this to figure out which buttons were clicked, and make sure to clear it before the next frame.
	std::vector<std::string> releasedObject;

	std::vector<std::string> hoveredObject;

	// There's probably a better idea to implement this but idk at the time being
	bool quitTriggered;
	
	Core* core;
};