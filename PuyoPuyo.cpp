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

			auto player_controller = 
				std::make_shared<class player_controller>();

			this->_player_board =
				std::make_shared<board>(257, 256, player_controller);

			this->_is_playing = false;

			this->_status = status::boot;

			this->_gameloft_logo = slLoadTexture("image/gameloft_logo.png");
			this->_puyo_title_logo = slLoadTexture("image/puyo_title_logo.png");
			this->_logo_time = 0.0;
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

					slText(512, 180, "Press Enter to start!");


					auto key_enter = slGetKey(SL_KEY_ENTER);

					if (key_enter) {
						this->_status = status::playing;
						this->_is_playing = true;
					}

				} break;

				case status::playing:
					if (this->_is_playing) {
						this->_player_board->update();
						this->_player_board->render();

						/*if (!this->_player_board->is_playing()) {
							this->_status = status::game_over;
						}*/
					}
					break;

				case status::game_over:
					break;
				}

		
				

				// background glow
				//slSetForeColor(0.1, 0.9, 0.2, 0.4);
				//slSprite(tex, 200, 240, 300, 200);

				/*			// large text and fat line
				slSetForeColor(0.0, 0.8, 0.2, 1.0);
				slSetFontSize(24);
				slText(200, 250, "SIGIL:");
				slRectangleFill(200, 240, 100, 7);

				// smaller subtext
				slSetFontSize(14);
				slText(200, 220, "Sprites, text, sound, input, and more!");
				slLine(48, 210, 352, 210);*/
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

		status _status;

		std::shared_ptr<board> _player_board;
		std::shared_ptr<board> _com_board;

		bool _is_playing;

		double _logo_time;
	};

	app app;

	app.run();

	return 0;
}