// PuyoPuyo.cpp : Defines the entry point for the console application.
//

/*
class piece {

public:

	enum rotation {
		left = 0,
		right
	};

	void rotate(const rotation& rotation) {

		switch (rotation) {
		case rotation::left:
			break;

		case rotation::right:
			break;
		}

	}

	void update() {

	}



	void render(){

	}

};

class board {



	std::vector<std::shared_ptr<piece> > pieces;
};*/

#include <iostream>
#include "board.hpp"

int main(int args, char *argv[])
{
	class app {
	public:
		app() {

			slWindow(1024, 512, "Puyo Puyo Game", false);
			slSetFont(slLoadFont("ttf/white_rabbit.ttf"), 16);
			slSetTextAlign(SL_ALIGN_LEFT);

			

			this->_is_playing = false;

			this->_status = status::boot;

			this->_gameloft_logo = slLoadTexture("image/gameloft_logo.png");
			this->_puyo_title_logo = slLoadTexture("image/puyo_title_logo.png");
			this->_player_one_wins_logo = slLoadTexture("image/player_one_wins_logo.png");
			this->_player_two_wins_logo = slLoadTexture("image/player_two_wins_logo.png");
			this->_logo_time = 0.0;

			this->_vs_com = false;

			this->_title_input_time = 0.0;
			this->_game_over_time = 0.0;
		}

		void run() {
			while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE))
			{

				switch (this->_status) {
				case status::boot:
					this->_logo_time += slGetDeltaTime();

					if (this->_logo_time >= 4.0) {
						this->_status = status::main_menu;
					}
					else 
					{
						slSprite(this->_gameloft_logo, 512, 256, 400, 232);
					}
					
					break;

				case status::main_menu: {
					slSprite(this->_puyo_title_logo, 512, 256, 400, 232);

					slSetFontSize(16);
					slText(512, 180, "Press Enter to start!");

					slText(512, 150, "1 player");
					slText(512, 135, "vs COM");

					if (this->_vs_com) {
						slText(500, 135, ">");
					} else {
						slText(500, 150, ">");
					}
					

					auto key_enter = slGetKey(SL_KEY_ENTER);
					auto key_down  = slGetKey(SL_KEY_DOWN);
					auto key_up    = slGetKey(SL_KEY_UP);

					if (key_enter) {
						this->_status = status::playing;
					}

					this->_title_input_time += slGetDeltaTime();

					if (this->_title_input_time >= 0.15) {
						this->_title_input_time = 0.0;

						if (key_down) {
							if (this->_vs_com) {
								this->_vs_com = false;
							}
							else {
								this->_vs_com = true;
							}
						}

						if (key_up) {
							if (this->_vs_com) {
								this->_vs_com = false;
							}
							else {
								this->_vs_com = true;
							}
						}
					}
				

				} break;

				case status::playing:
					if (this->_is_playing) {
						bool is_game_over = false;

						if(!this->_player_board->is_playing()) {

							is_game_over = true;
													
						} else {

							if (this->_vs_com) {
								if (!this->_com_board->is_playing()) {
									is_game_over = true;
								}
								else {
									this->_player_board->update();
									this->_com_board->update();
								}
							}
							else {
								this->_player_board->update();
							}
						}

						this->_player_board->render();

						if (this->_vs_com) {
							this->_com_board->render();
						}

						if (is_game_over) {
							this->_game_over_time += slGetDeltaTime();

							if (this->_vs_com) {
								if (this->_player_board->is_playing()) {
									slSprite(this->_player_one_wins_logo, 200, 256, 384, 193);
								}
								else {
									slSprite(this->_player_two_wins_logo, 720, 256, 384, 193);
								}
							}
							else {

							}

							if (this->_game_over_time >= 4.0) {
								this->_status = status::main_menu;
								this->_is_playing = false;
							}
							
						}
					}
					else {

						auto player_controller =
							std::make_shared<class player_controller>();

						this->_player_board =
							std::make_shared<board>(257, 256, player_controller);

						if (this->_vs_com) {
							auto ai_controller =
								std::make_shared<class ai_controller>();
							
							this->_com_board =
								std::make_shared<board>(767, 256, ai_controller);

						}

						this->_is_playing = true;
					}
					break;

				case status::game_over:
					break;
				}

				slRender();	// draw everything
			}
			slClose(); //close the window and shut down SIGIL
		}

	private:
		enum status {
			boot, 
			main_menu, 
			playing, 
			game_over
		};


		int _gameloft_logo;
		int _puyo_title_logo;

		int _player_one_wins_logo;
		int _player_two_wins_logo;

		status _status;

		std::shared_ptr<board> _player_board;
		std::shared_ptr<board> _com_board;

		bool _is_playing;
		bool _vs_com;
		
		double _logo_time;
		double _title_input_time;
		double _game_over_time;
		double _rematch_time;
	};

	app app;

	app.run();

	return 0;
}