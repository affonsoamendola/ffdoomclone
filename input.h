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
	ACTION_MOUSE_MOVE,
	ACTION_MOUSE_DRAG_RIGHT,
	ACTION_SCROLL_WHEEL
} ActionCode;

typedef struct Action_
{
	ActionCode action;
	ActionFunction function;
	void* user_data;
} Action;

typedef struct Input_
{
	const uint8_t* is_scancode_down;

	Action* registered_actions;
	uint32_t registered_actions_size;

	float mouse_sensitivity;

	int mouse_x;
	int mouse_y;
	uint32_t mouse_buttons;
} Input;

extern Input input;

typedef struct ActionKeyPair_
{
	uint32_t keycode;
	ActionCode action;
} ActionKeyPair;

void init_input();
void update_input();

void set_input_actions(Action* actions, const uint32_t number_of_actions);

void on_keydown(SDL_Keycode keycode);
void on_mouse_movement(const Vector2f amount);

void on_mouse_wheel(const int direction);

static inline uint32_t is_keyheld(SDL_Keycode key)
{
	return input.is_scancode_down[SDL_GetScancodeFromKey(key)];
}

static inline bool is_mouse_held(MouseButton button)
{
	return input.mouse_buttons & button;
}

#endif