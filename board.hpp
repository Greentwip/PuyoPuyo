#pragma once
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <map>

#include "sl.h"

#include "defines.hpp"
#include "controller.hpp"
#include "piece.hpp"
#include "input.hpp"


class board {
public:

	enum post_player_status {
		combo = 0,
		combo_falling,
		post_combo_fall
	};

	board(uint16_t x, uint16_t y, std::shared_ptr<controller> controller) {

		this->_x = x;
		this->_y = y;

		this->_score_board_x = x + 191;
		this->_score_board_y = y;

		this->_controller = controller;
		this->_current_piece = std::make_shared<piece>(piece::color::red);
		this->_pieces.push_back(this->_current_piece);
		this->_pieces.push_back(this->_current_piece->secondary());

		// total width minus score board width
		this->_game_width = 384;

		// time in seconds
		this->_fall_speed = 1.0;
		this->_fall_counter = 0.0;

		this->_move_speed = 0.1; 
		this->_move_counter = 0.0;

		this->_combo_fall_speed = 0.25;

		_post_player_status = combo;

		this->_playing = true;
	}

	void move() {

		auto controller_input = _controller->get_input();
		
		bool is_primary_locked = this->_current_piece->locked();
		bool is_secondary_locked = this->_current_piece->secondary()->locked();

		if (can_move(controller_input) && !is_primary_locked && !is_secondary_locked) {

			this->_current_piece->move(controller_input);
			this->_current_piece->secondary()->move(controller_input);

		}

		if (can_rotate(controller_input) && !is_primary_locked && !is_secondary_locked) {
			this->_current_piece->rotate(controller_input.get_rotation());
		}
	}

	bool can_rotate(input input) {

		auto left_bound_left_rotation = this->_current_piece->position().x -
			this->_current_piece->size().width <= 0 &&
			this->_current_piece->secondary()->position().y >
			this->_current_piece->position().y &&
			input.get_rotation() == rotation::left; // rotation on the left boundary

		auto left_bound_right_rotation = this->_current_piece->position().x -
			this->_current_piece->size().width <= 0 &&
			this->_current_piece->secondary()->position().y <
			this->_current_piece->position().y &&
			input.get_rotation() == rotation::right; // rotation on the left boundary

		auto lower_bound_left_rotation = this->_current_piece->position().y -
			this->_current_piece->size().height <= 0 &&
			this->_current_piece->secondary()->position().x <
			this->_current_piece->position().x &&
			input.get_rotation() == rotation::left;

		auto lower_bound_right_rotation = this->_current_piece->position().y -
			this->_current_piece->size().height <= 0 &&
			this->_current_piece->secondary()->position().x >
			this->_current_piece->position().x &&
			input.get_rotation() == rotation::right;

		auto right_bound_right_rotation = this->_current_piece->position().x +
			this->_current_piece->size().width >= this->_game_width  &&
			this->_current_piece->secondary()->position().y >
			this->_current_piece->position().y &&
			input.get_rotation() == rotation::right; // rotation on the right boundary

		auto right_bound_left_rotation = this->_current_piece->position().x +
			this->_current_piece->size().width >= this->_game_width &&
			this->_current_piece->secondary()->position().y <
			this->_current_piece->position().y &&
			input.get_rotation() == rotation::left; // rotation on the right boundary


		if (left_bound_left_rotation   || 
			left_bound_right_rotation  ||
			lower_bound_left_rotation  ||
			lower_bound_right_rotation ||
			right_bound_left_rotation  ||
			right_bound_right_rotation) {
			return false;
		}
		else 
		{
			bool can_rotate = true;
			for (size_t i = 0; i < this->_pieces.size() && can_rotate; ++i)
			{
				auto& piece = this->_pieces[i];

				if (piece != this->_current_piece && piece != this->_current_piece->secondary()) {


					auto left_bound_left_rotation = input.get_rotation() == rotation::left &&
													piece->position().y == this->_current_piece->position().y &&
													piece->position().x == this->_current_piece->position().x - this->_current_piece->size().width &&
													this->_current_piece->secondary()->position().y > this->_current_piece->position().y;

					auto left_bound_right_rotation = input.get_rotation() == rotation::right &&
													piece->position().y == this->_current_piece->position().y &&
													piece->position().x == this->_current_piece->position().x - this->_current_piece->size().width &&
													this->_current_piece->secondary()->position().y < this->_current_piece->position().y;

					auto lower_bound_left_rotation = input.get_rotation() == rotation::left &&
													piece->position().y == this->_current_piece->position().y - this->_current_piece->size().height &&
													piece->position().x == this->_current_piece->position().x &&
													this->_current_piece->secondary()->position().x < this->_current_piece->position().x;


					auto lower_bound_right_rotation = input.get_rotation() == rotation::right &&
													piece->position().y == this->_current_piece->position().y - this->_current_piece->size().height &&
													piece->position().x == this->_current_piece->position().x &&
													this->_current_piece->secondary()->position().x > this->_current_piece->position().x;

				
					auto right_bound_left_rotation = input.get_rotation() == rotation::left &&
													piece->position().y == this->_current_piece->position().y &&
													piece->position().x == this->_current_piece->position().x + this->_current_piece->size().width &&
													this->_current_piece->secondary()->position().y < this->_current_piece->position().y;

					auto right_bound_right_rotation = input.get_rotation() == rotation::right &&
													piece->position().y == this->_current_piece->position().y &&
													piece->position().x == this->_current_piece->position().x + this->_current_piece->size().width &&
													this->_current_piece->secondary()->position().y > this->_current_piece->position().y;

					if (left_bound_left_rotation ||
						left_bound_right_rotation ||
						lower_bound_left_rotation ||
						lower_bound_right_rotation ||
						right_bound_left_rotation ||
						right_bound_right_rotation) {
						can_rotate = false;
					}
								
				}

			}

			return can_rotate;
		}
		
	}

	bool can_move(input input) {

		auto left_bound_move = this->_current_piece->position().x -
			this->_current_piece->size().width <= 0 &&
			input.get_direction() == direction::horizontal &&
			input.normal() == -1;

		auto right_bound_move = this->_current_piece->position().x +
			this->_current_piece->size().width >= this->_game_width &&
			input.get_direction() == direction::horizontal &&
			input.normal() == 1;

		auto left_bound_secondary_move =
			this->_current_piece->secondary()->position().x -
			this->_current_piece->secondary()->size().width <= 0 &&
			input.get_direction() == direction::horizontal &&
			input.normal() == -1;

		auto right_bound_secondary_move = this->_current_piece->secondary()->position().x +
			this->_current_piece->secondary()->size().width >= this->_game_width &&
			input.get_direction() == direction::horizontal &&
			input.normal() == 1;

		auto lower_bound_move = this->_current_piece->position().y -
			this->_current_piece->size().height <= 0 &&
			input.get_direction() == direction::vertical &&
			input.normal() == -1;

		auto lower_bound_secondary_move = this->_current_piece->secondary()->position().y -
			this->_current_piece->secondary()->size().height <= 0 &&
			input.get_direction() == direction::vertical &&
			input.normal() == -1;

		if (left_bound_move || 
			left_bound_secondary_move ||
			right_bound_move || 
			right_bound_secondary_move ||			
			lower_bound_move ||
			lower_bound_secondary_move) {
			return false;
		} else {
			bool can_move = true;
			for (size_t i = 0; i < this->_pieces.size() && can_move; ++i)
			{
				auto& piece = this->_pieces[i];

				if (piece != this->_current_piece && piece != this->_current_piece->secondary()) {


					auto left_bound_move = input.get_direction() == direction::horizontal &&
											input.normal() == -1 &&
											piece->position().y == this->_current_piece->position().y &&
											piece->position().x == this->_current_piece->position().x - this->_current_piece->size().width &&
											this->_current_piece->secondary()->position().x >= this->_current_piece->position().x;
					
					/*auto left_bound_move_secondary = input.get_direction() == direction::horizontal &&
													 input.normal() == -1 &&
													 piece->position().y == this->_current_piece->secondary()->position().y &&
													 piece->position().x == this->_current_piece->secondary()->position().x - this->_current_piece->secondary()->size().width &&
													 this->_current_piece->secondary()->position().x < this->_current_piece->position().x;
					*/
					auto left_bound_move_secondary = input.get_direction() == direction::horizontal &&
													 input.normal() == -1 &&
													 piece->position().y == this->_current_piece->secondary()->position().y &&
													 piece->position().x == this->_current_piece->secondary()->position().x - this->_current_piece->secondary()->size().width;



					auto lower_bound_move = input.get_direction() == direction::vertical &&
											input.normal() == -1 &&
											piece->position().y == this->_current_piece->position().y - this->_current_piece->size().height &&
											piece->position().x == this->_current_piece->position().x &&
											this->_current_piece->secondary()->position().y < this->_current_piece->position().y;

					auto lower_bound_move_secondary = input.get_direction() == direction::vertical &&
														input.normal() == -1 &&
														piece->position().y == this->_current_piece->secondary()->position().y - this->_current_piece->secondary()->size().height &&
														piece->position().x == this->_current_piece->secondary()->position().x &&
														this->_current_piece->secondary()->position().y >= this->_current_piece->position().y;

					auto right_bound_move = input.get_direction() == direction::horizontal &&
											input.normal() == 1 &&
											piece->position().y == this->_current_piece->position().y &&
											piece->position().x == this->_current_piece->position().x + this->_current_piece->size().width &&
											this->_current_piece->secondary()->position().x < this->_current_piece->position().x;


					auto right_bound_move_secondary = input.get_direction() == direction::horizontal &&
														input.normal() == 1 &&
														piece->position().y == this->_current_piece->secondary()->position().y &&
														piece->position().x == this->_current_piece->secondary()->position().x + this->_current_piece->secondary()->size().width;
					if (left_bound_move ||
						left_bound_move_secondary ||
						lower_bound_move ||
						lower_bound_move_secondary ||
						right_bound_move || 
						right_bound_move_secondary) {
						can_move = false;
					}
								
				}

			}

			return can_move;
		}
	}

	void fall() {
		bool is_primary_locked = this->_current_piece->locked();
		bool is_secondary_locked = this->_current_piece->secondary()->locked();

		if (!is_primary_locked) {
			this->_current_piece->move(input(direction::vertical, -1, rotation::none));
		}
		
		if (!is_secondary_locked) {
			this->_current_piece->secondary()->move(input(direction::vertical, -1, rotation::none));
		}		
	}

	bool verify_fall() {

		if (this->_current_piece->position().y -
			this->_current_piece->size().height <= 0 ||
			this->_current_piece->secondary()->position().y -
			this->_current_piece->secondary()->size().height <= 0) {
			return false;
		}
		else {
			bool can_fall = true;
			for (size_t i = 0; i < this->_pieces.size() && can_fall; ++i)
			{
				auto& piece = this->_pieces[i];

				if (piece != this->_current_piece && piece != this->_current_piece->secondary()) {

					if (piece->position().x == this->_current_piece->position().x &&
						piece->position().y == this->_current_piece->position().y -
						this->_current_piece->size().height) {

						this->_current_piece->locked(true);

					}

					if (piece->position().x == this->_current_piece->secondary()->position().x &&
						piece->position().y == this->_current_piece->secondary()->position().y -
						this->_current_piece->secondary()->size().height) {

						this->_current_piece->secondary()->locked(true);
					}

					if (this->_current_piece->locked()) {
						if (this->_current_piece->secondary()->position().x == this->_current_piece->position().x &&
							this->_current_piece->secondary()->position().y == this->_current_piece->position().y +
							this->_current_piece->size().height) {

							this->_current_piece->secondary()->locked(true);

						}
					}

					if (this->_current_piece->secondary()->locked()) {
						if (this->_current_piece->secondary()->position().x == this->_current_piece->position().x &&
							this->_current_piece->secondary()->position().y == this->_current_piece->position().y -
							this->_current_piece->size().height) {

							this->_current_piece->locked(true);

						}
					}
					can_fall = !(this->_current_piece->locked() && this->_current_piece->secondary()->locked());
				}

			}

			return can_fall;
		}
	}

	void combo_fall(std::shared_ptr<piece> combo_piece) {
		bool is_primary_locked = combo_piece->locked();

		//if (!is_primary_locked) {
			combo_piece->move(input(direction::vertical, -1, rotation::none));
		//}

	}

	bool verify_combo_fall(std::shared_ptr<piece> combo_piece) {

		if (combo_piece->position().y -
			combo_piece->size().height <= 0) {
			return false;
		}
		else {
			bool can_fall = true;
			for (size_t i = 0; i < this->_pieces.size() && can_fall; ++i)
			{
				auto& piece = this->_pieces[i];

				if (piece != combo_piece) {

					if (piece->position().x == combo_piece->position().x &&
						piece->position().y == combo_piece->position().y -
						combo_piece->size().height) {

						can_fall = false;

					}
				}

			}

			return can_fall;
		}
	}

	void update() {
		//std::cout << slGetDeltaTime() << std::endl;

		this->_fall_counter += slGetDeltaTime();
		this->_move_counter += slGetDeltaTime();

		if (verify_fall()) {

			if (this->_current_piece->locked() || this->_current_piece->secondary()->locked()) {
				if (this->_fall_counter >= this->_fall_speed * 0.25) {
					this->fall();
					this->_fall_counter = 0;
				}
			} else {
				if (this->_fall_counter >= this->_fall_speed) {
					this->fall();
					this->_fall_counter = 0;
				}
			}
			

			if (this->_move_counter >= this->_move_speed) {
				this->move();
				this->_move_counter = 0;
			}
		}
		else
		{
			// new piece, player combo and game over
			if (this->_current_piece->position().x == 176 &&
				this->_current_piece->position().y == 496) {
				this->_playing = false;
			}
			else {


				if(_post_player_status == post_player_status::combo){
					perform_combo(this->_current_piece);
					perform_combo(this->_current_piece->secondary());

					_post_player_status = post_player_status::combo_falling;

				}
				else if (_post_player_status == post_player_status::combo_falling) {

					for (auto piece : this->_pieces) {
						if (verify_combo_fall(piece)) {

							if (_combo_fall_counters.find(piece) == 
								_combo_fall_counters.end()) {
								_combo_fall_counters[piece] = 0.0f;
							}

							_combo_fall_counters[piece] += slGetDeltaTime();

							if (this->_combo_fall_counters[piece] >= this->_combo_fall_speed) {
								this->combo_fall(piece);
								this->_combo_fall_counters[piece] = 0.0f;
							}
						} else {
							if (_combo_fall_counters.find(piece) !=
								_combo_fall_counters.end()) {
								_combo_fall_counters.erase(piece);
							}
						}
					}

					// no pieces falling
					if (_combo_fall_counters.size() == 0) {
						bool iterators_valid = true;
						for (uint32_t i = 0; i < this->_pieces.size() && iterators_valid; ++i) {
							auto piece = this->_pieces[i];
							perform_combo(piece);

							if (this->_combo_chain.size() >= 4) {
								iterators_valid = false;
							}
						}
						
						if (this->_combo_chain.size() < 4) {
							_post_player_status = post_player_status::post_combo_fall;
						}
						else {
							// count combo chain
						}
						
					}
				}
				else if (_post_player_status == post_player_status::post_combo_fall) {
					std::uniform_int_distribution<int> distribution(0, 3);
					int dice_roll_a = distribution(generator);
					int dice_roll_b = distribution(generator);

					this->_current_piece = std::make_shared<piece>(piece::color(dice_roll_a),
						piece::color(dice_roll_b));
					this->_pieces.push_back(this->_current_piece);
					this->_pieces.push_back(this->_current_piece->secondary());

					_post_player_status = post_player_status::combo;
				}

			}
		}

		for (auto& piece : this->_pieces) {
			piece->render();
		}

	}


	void perform_combo(std::shared_ptr<piece> combo_piece) {
		this->_combo_chain.clear();

		for (auto piece : this->_pieces) {
			piece->visited(false);
		}

		verify_combo(combo_piece);

		if (this->_combo_chain.size() >= 4) {

			for (auto itc = this->_combo_chain.begin(); itc != this->_combo_chain.end(); ++itc) {
				this->_pieces.erase(std::remove(this->_pieces.begin(),
					this->_pieces.end(),
					*itc),
					this->_pieces.end());

			}
		}
	}

	void verify_combo(std::shared_ptr<piece> combo_piece) {
		
		for (auto piece : this->_pieces) {

			bool piece_to_right = piece->position().x == combo_piece->position().x + combo_piece->size().width &&
				piece->position().y == combo_piece->position().y && 
				piece->get_color() == combo_piece->get_color();

			bool piece_to_left = piece->position().x == combo_piece->position().x - combo_piece->size().width &&
				piece->position().y == combo_piece->position().y &&
				piece->get_color() == combo_piece->get_color();

			bool piece_above = piece->position().x == combo_piece->position().x &&
				piece->position().y == combo_piece->position().y + combo_piece->size().height &&
				piece->get_color() == combo_piece->get_color();

			bool piece_below = piece->position().x == combo_piece->position().x &&
				piece->position().y == combo_piece->position().y - combo_piece->size().height &&
				piece->get_color() == combo_piece->get_color();

			if (piece_to_right || piece_to_left || piece_above || piece_below) {
				if (!piece->visited()) {
					piece->visited(true);

					_combo_chain.push_back(piece);

					verify_combo(piece);
				} 
			}
		}

	}

	std::default_random_engine generator;

	void render() {
		slSetForeColor(1.0, 1.0, 1.0, 1.0);
		slRectangleOutline(this->_x, this->_y, 512, 511);
		slRectangleOutline(this->_score_board_x, this->_score_board_y, 128, 511);
	}

private:
	uint16_t _x;
	uint16_t _y;

	uint16_t _score_board_x;
	uint16_t _score_board_y;

private:
	uint16_t _game_width; 

	std::string _board_label;
	uint64_t _score;
	
	std::vector<std::shared_ptr<piece> > _combo_chain;
	post_player_status _post_player_status;
	std::map<std::shared_ptr<piece>, double> _combo_fall_counters;
	float _combo_fall_speed;

	bool _playing;

private:
	double _fall_speed;
	double _fall_counter;

	double _move_speed;
	double _move_counter;

	std::shared_ptr<piece> _current_piece;
	std::shared_ptr<controller> _controller;
	std::vector<std::shared_ptr<piece> > _pieces;
};