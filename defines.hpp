#pragma once
enum direction {
	horizontal,
	vertical
};

enum rotation {
	left, 
	right, 
	none
};

struct size {
	uint16_t width;
	uint16_t height;
};

struct position {
	uint16_t x;
	uint16_t y;
};