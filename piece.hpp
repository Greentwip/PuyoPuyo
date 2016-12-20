#pragma once
class piece {
public:
	enum color {
		red = 0,
		blue,
		green,
		purple
	};

	enum status {
		puyo = 0, 
		normal
	};

	piece(uint16_t x, uint16_t y, 
		  const color& primary_color, 
		  const color& secondary_color = color::purple,
		  bool is_primary = true): _is_primary(is_primary), _is_locked(false), _visited(false) {
		this->_x = x;
		this->_y = y;
		this->_width = 32;
		this->_height = 32;

		this->_color = primary_color;

		if (this->_is_primary) {
			this->_secondary = std::make_shared<piece>(x, y, secondary_color, secondary_color, false);
			this->_secondary->position(x, y - 32);
		}
		
	}

	void move(input input) {
		switch (input.get_direction()) {
		case direction::vertical:
			this->_y += this->_height * input.normal();
			break;
		case direction::horizontal:
			this->_x += this->_width * input.normal();
			break;
		}
	}

	color get_color() {
		return this->_color;
	}

	void set_status(status status) {
		this->_status = status;
	}

	status get_status() {
		return this->_status;
	}

	void position(const uint16_t& x, const uint16_t& y) {
		this->_x = x;
		this->_y = y;
	}

	void size(const uint16_t& width, const uint16_t& height) {
		this->_width = width;
		this->_height = height;
	}

	struct position position() {
		struct position position;

		position.x = this->_x;
		position.y = this->_y;

		return position;
	}

	struct size size() {
		struct size size;

		size.width = this->_width;
		size.height = this->_height;

		return size;
	}

	void render() {

		switch (this->_color) {
		case color::red:
			slSetForeColor(1.0, 0.0, 0.0, 1.0);
			break;

		case color::green:
			slSetForeColor(0.0, 1.0, 0.0, 1.0);
			break;

		case color::blue:
			slSetForeColor(0.0, 0.0, 1.0, 1.0);
			break;

		case color::purple:
			slSetForeColor(1.0, 0.0, 1.0, 1.0);
			break;
		}

		slRectangleFill(_x, _y, _width, _height);
	}

	void rotate(rotation rotation) {
		if (this->_is_primary) {
			switch (rotation) {
			case left:
				if (this->_secondary->position().x > this->_x)
				{
					this->_secondary->position(this->_x, this->_y + this->_height);
				} 
				else if (this->_secondary->position().x < this->_x) 
				{
					this->_secondary->position(this->_x, this->_y - this->_height);
				} 
				else if (this->_secondary->position().x == this->_x) 
				{
					if (this->_secondary->position().y > this->_y) 
					{
						this->_secondary->position(this->_x - this->_width, this->_y);
					} 
					else if (this->_secondary->position().y < this->_y) 
					{
						this->_secondary->position(this->_x + this->_width, this->_y);
					}
				}

				break;

			case right:
				if (this->_secondary->position().x > this->_x)
				{
					this->_secondary->position(this->_x, this->_y - this->_height);
				}
				else if (this->_secondary->position().x < this->_x)
				{
					this->_secondary->position(this->_x, this->_y + this->_height);
				}
				else if (this->_secondary->position().x == this->_x)
				{
					if (this->_secondary->position().y > this->_y)
					{
						this->_secondary->position(this->_x + this->_width, this->_y);
					}
					else if (this->_secondary->position().y < this->_y)
					{
						this->_secondary->position(this->_x - this->_width, this->_y);
					}
				}
				break;
			}
		}
	}

	std::shared_ptr<piece> secondary() {
		return this->_secondary;
	}

	bool locked() {
		return this->_is_locked;
	}

	void locked(bool locked) {
		this->_is_locked = locked;
	}

	bool visited() {
		return this->_visited;
	}

	void visited(const bool& visited) {
		this->_visited = visited;
	}

private:
	uint16_t _x;
	uint16_t _y;
	uint16_t _width;
	uint16_t _height;

	color _color;
	bool _is_primary;
	bool _is_locked;

	status _status;

	bool _visited;

	std::shared_ptr<piece> _secondary;
};

