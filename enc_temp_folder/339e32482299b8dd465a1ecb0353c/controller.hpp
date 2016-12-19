#pragma once

#include "sl.h"

#include "defines.hpp"
#include "input.hpp"

class controller {
public:
	virtual input get_input() {
		return input(direction::horizontal, 0, rotation::none);
	}
};

class player_controller : public controller {
	virtual input get_input() override {
		auto key_left = slGetKey(SL_KEY_LEFT);
		auto key_right = slGetKey(SL_KEY_RIGHT);
		auto key_down = slGetKey(SL_KEY_DOWN);

		auto rotate_left = slGetKey('Z');
		auto rotate_right = slGetKey('X');

		if (key_left) {
			return input(direction::horizontal, -1, rotation::none);
		}

		if (key_right) {
			return input(direction::horizontal, 1, rotation::none);
		}

		if (key_down) {
			return input(direction::vertical, -1, rotation::none);
		}

		if (rotate_left) {
			return input(direction::horizontal, 0, rotation::left);
		}

		if (rotate_right) {
			return input(direction::horizontal, 0, rotation::right);
		}

		return input(direction::horizontal, 0, rotation::none);
	}
};

class ai_controller : public controller {
	virtual input get_input() override {
		return input(direction::horizontal, -1, rotation::none);
	}
};
