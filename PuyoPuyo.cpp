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

			this->_board = 
				std::make_shared<board>(player_controller);
		}

		void run() {
			while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE))
			{
				this->_board->update();
				this->_board->render();

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
		std::shared_ptr<board> _board;
	};

	app app;

	app.run();

	return 0;
}