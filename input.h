#ifndef INPUT_H
#define INPUT_H

#define INPUT_KEY_HOLD 1
#define INPUT_KEY_FREE 0

#include "ff_vector2.h"
#include "ff_stb.h"

typedef enum MouseButton_
{
	INPUT_MOUSE_LEFT = SDL_BUTTON(1),
	INPUT_MOUSE_MIDDLE = SDL_BUTTON(2),
	INPUT_MOUSE_RIGHT = SDL_BUTTON(3)
} MouseButton;

typedef union ActionFunction_
{
	void (*default_func)(void* user_data);
	void (*mouse_move_func)(void* user_data, Vector2f delta_mouse);
	void (*mouse_wheel_func)(void* user_data, int wheel_direction);
} ActionFunction;

typedef enum ActionCode_
{
	ACTION_QUIT,
	ACTION_TOGGLE_CONSOLE,
	ACTION_CONFIRM_CONSOLE,
	ACTION_MOUSE_MOVE,
	ACTION_MOUSE_DRAG_RIGHT,
	ACTION_SCROLL_WHEEL,
	ACTION_FORWARD, 	
	ACTION_BACKWARD, 		
	ACTION_STRAFE_RIGHT, 		
	ACTION_STRAFE_LEFT, 		
	ACTION_TURN_RIGHT, 	
	ACTION_TURN_LEFT, 
} ActionCode;

typedef enum ActionFlag_
{
	ACT_FLAG_NO_FLAG = 0,
	ACT_FLAG_CONTINUOUS
} ActionFlag;

typedef struct Action_
{
	ActionCode action;
	ActionFunction function;
	void* user_data;
	ActionFlag flags;
} Action;

typedef struct Input_
{
	const uint8_t* is_scancode_down;

	Action* registered_actions;
	uint32_t registered_actions_size;

	ActionCode* disabled_actions;
	uint32_t disabled_actions_size;

	ActionCode* enabled_actions;
	uint32_t enabled_actions_size;

	float mouse_sensitivity;

	int mouse_x;
	int mouse_y;
	uint32_t mouse_buttons;

	bool text_input_enabled;
	char* text_input_destination;
	uint32_t text_input_buffer_size;
	uint32_t text_input_character_loc;
} Input;

extern Input input;

typedef struct ActionKeyPair_
{
	uint32_t keycode;
	ActionCode actioncode;
} ActionKeyPair;

void init_input();
void update_input();

void set_input_actions(Action* actions, const uint32_t number_of_actions);

void set_disabled_actions(ActionCode* actions, const uint32_t number_of_actions);
bool check_disabled_actions(ActionCode action);

void set_enabled_actions(ActionCode* actions, const uint32_t number_of_actions);
bool check_enabled_actions(ActionCode action);

void on_keydown(SDL_Keycode keycode, bool continuous);
void on_mouse_movement(const Vector2f amount);

void on_mouse_wheel(const int direction);

void on_text_input(SDL_Keycode keycode);
void start_text_input(char* text_destination, uint32_t buffer_size);
void end_text_input();

static inline uint32_t is_keyheld(SDL_Keycode key)
{
	return input.is_scancode_down[SDL_GetScancodeFromKey(key)];
}

static inline bool is_mouse_held(MouseButton button)
{
	return input.mouse_buttons & button;
}

#endif