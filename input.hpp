#pragma once
class input {
public:
	input(const direction& direction, 
		  const int& normal, 
		  const rotation& rotation) {
		this->_direction = direction;
		this->_normal = normal;
		this->_rotation = rotation;
	}

	int normal() {
		return this->_normal;
	}

	direction get_direction() {
		return this->_direction;
	}

	rotation get_rotation() {
		return this->_rotation;
	}

private:
	direction _direction;
	rotation _rotation;
	int _normal;

};