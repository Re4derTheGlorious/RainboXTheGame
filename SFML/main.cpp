#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace sf;

//some global settings
int gapSize = 3;
int startingBlocks = 4;
int bigBlockChance = 10; //per 100 blocks
const int boardSize = 4;

class Block {
	public:
		int value;
		bool isNew;
		Block() {
			value = 0;
			isNew = true;
		}
		void render(RenderWindow *frame, int offX, int offY) {
			int size = (frame->getSize().x-5*gapSize) / 4;
			RectangleShape shape(Vector2f(size, size));
			shape.setPosition(gapSize+offX*size+gapSize*offX, gapSize+offY*size+gapSize*offY);

			//if is new
			if (isNew && value!=0) {
				shape.setOutlineColor(Color::Red);
				shape.setOutlineThickness(gapSize);
			}

			//set color
			Color color;
			switch (value) {
			case 0:
				color = Color::White;
				break;
			case 1:
				color = Color(216, 191, 216);
				break;
			case 2:
				color = Color(221, 160, 221);
				break;
			case 4:
				color = Color(218, 112, 214);
				break;
			case 8:
				color = Color(186, 85, 211);
				break;
			case 16:
				color = Color(148, 0, 211);
				break;
			case 32:
				color = Color(75, 0, 130);
				break;
			case 64:
				color = Color(139, 0, 139);
				break;
			case 128:
				color = Color(0, 0, 139);
				break;
			case 256:
				color = Color(0, 0, 205);
				break;
			case 512:
				color = Color(30, 144, 255);
				break;
			case 1024:
				color = Color(0, 191, 255);
				break;
			}
			shape.setFillColor(color);
			frame->draw(shape);
		}
};

void init(RenderWindow *frame, Block *board) {
	//create window
	frame->create(VideoMode(800, 800), "2048");

	//init rng
	srand(time(NULL));

	//fill board
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			*(board+x+y*boardSize) = Block();
		}
	}
	
	//set starting blocks
	for (int i = 0; i < startingBlocks; i++) {
		int pos = 0;
		do {
			pos = rand() % (boardSize*boardSize);
		} while ((board + pos)->value != 0);
		(board + pos)->value = 1;
	}
}

bool handleEvents(RenderWindow *frame, Event *externalEvent) {
	bool newEvent = false;
	Event event;
	Event keyboardEvent;
	while (frame->pollEvent(event));
	{
		//window actions
		if (event.type == Event::Closed)
			frame->close();

		//keyboard handling
		else if (event.type == Event::KeyPressed) {
			*externalEvent = event;
			newEvent = true;
		}
	}
	return newEvent;
}

void logic(Block *board, Event event) {
	//check win/lose condition
	bool boardFull = true;
	for (int i = 0; i < boardSize*boardSize; i++) {
		if (board[i].value == 0) {
			boardFull = false;
			break;
		}
	}
	if (boardFull) {
		return;
	}

	//unset as new
	for (int i = 0; i < boardSize*boardSize; i++) {
		board[i].isNew = false;
	}

	//set movement direction
	int direction = event.key.code; //71=left, 72=right, 73=up, 74=down

	//move blocks
	for (int stage = 0; stage < 3; stage++) { //stages: align, merge, align again
		if (stage == 1) { //merge
			//mergin in order
			for (int x = 0; x < boardSize; x++) {
				for (int y = 0; y < boardSize; y++) {
					if ((board + x + y * boardSize)->value != 0) { //check whether the field is not empty 
						if (direction == 71 && x < boardSize - 1 && (board + (x + 1) + y * boardSize)->value == (board + x + y * boardSize)->value) { //check for neighboring mergable block on horizontal axis
							(board + x + y * boardSize)->value *= 2;
							(board + (x + 1) + y * boardSize)->value = 0;
						}
						else if (direction == 73 && y < boardSize - 1 && (board + x + (y + 1) * boardSize)->value == (board + x + y * boardSize)->value) { //check for neighboring mergable block on vertical axis
							(board + x + y * boardSize)->value *= 2;
							(board + x + (y + 1) * boardSize)->value = 0;
						}
					}
				}
			}
			//merging in reversed order
			for (int x = boardSize-1; x >= 0; x--) {
				for (int y = boardSize-1; y >= 0; y--) {
					if ((board + x + y * boardSize)->value != 0) { //check whether the field is not empty 
						if (direction == 72 && x > 0 && (board + (x - 1) + y * boardSize)->value == (board + x + y * boardSize)->value) { //check for neighboring mergable block on horizontal axis
							(board + x + y * boardSize)->value *= 2;
							(board + (x - 1) + y * boardSize)->value = 0;
						}
						else if (direction == 74 && y > 0 && (board + x + (y - 1) * boardSize)->value == (board + x + y * boardSize)->value) { //check for neighboring mergable block on vertical axis
							(board + x + y * boardSize)->value *= 2;
							(board + x + (y - 1) * boardSize)->value = 0;
						}
					}
				}
			}
		}
		else { //align blocks
			for (int i = 0; i < boardSize - 1; i++) {
				for (int x = 0; x < boardSize; x++) {
					for (int y = 0; y < boardSize; y++) {
						if ((board + x + y * boardSize)->value != 0) { //check whether the field is not empty 
							if (direction == 71 && x > 0 && (board + (x - 1) + y * boardSize)->value == 0) { //check for empty space on the left
								(board + (x - 1) + y * boardSize)->value = (board + x + y * boardSize)->value;
								(board + x + y * boardSize)->value = 0;
							}
							else if (direction == 72 && x < boardSize - 1 && (board + (x + 1) + y * boardSize)->value == 0) { //check for empty space on the right
								(board + (x + 1) + y * boardSize)->value = (board + x + y * boardSize)->value;
								(board + x + y * boardSize)->value = 0;
							}
							else if (direction == 73 && y > 0 && (board + x + (y - 1) * boardSize)->value == 0) { //check for empty space on top
								(board + x + (y - 1) * boardSize)->value = (board + x + y * boardSize)->value;
								(board + x + y * boardSize)->value = 0;
							}
							else if (direction == 74 && y < boardSize - 1 && (board + x + (y + 1) * boardSize)->value == 0) { //check for empty space on the bottom
								(board + x + (y + 1) * boardSize)->value = (board + x + y * boardSize)->value;
								(board + x + y * boardSize)->value = 0;
							}
						}
					}
				}
			}
		}
	}

	//spawn new block
	int pos = 0;
	bool bigBlock = false;
	do {
		pos = rand() % (boardSize*boardSize);
	} while ((board + pos)->value != 0);

	if (rand() % 100 < bigBlockChance) {
		bigBlock = true;
	}

	if (!bigBlock) {
		(board + pos)->value = 1;
	}
	else {
		(board + pos)->value = 2;
	}
	(board + pos)->isNew = true;
}

void render(RenderWindow *frame, Block *board) {
	frame->clear();
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			(board+x + y * boardSize)->render(frame, x, y);
		}
	}
	frame->display();
}

int main(){
	//variables
	RenderWindow frame;
	Block board[boardSize*boardSize];

	//init
	init(&frame, board);

	//game loop
	render(&frame, board);
	while (frame.isOpen())
	{
		//events
		Event event;
		bool needsRefresh = handleEvents(&frame, &event);

		//loop body
		if (needsRefresh) {
			logic(board, event);
			render(&frame, board);
		}
	}

	return 0;
}
