/*
* File:   main.cpp
* Author: Re4derTheGlorious
*
* RainboX The Game is a fascinating story about a great hero which embarks on a travel to discover the only right colours.
* Using mechanics from classic 2048 game your goal is to get up to the solid red block.
* Use keyboard arrows for controling your game board and enter to restart the game at any time.
*
*/

#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>

using namespace sf;

//some global settings
int gapSize = 3;
int startingBlocks = 4;
int bigBlockChance = 10; //per 100 blocks
const int boardSize = 4;

class Block {
private:
	int value;
	bool newSpawned;
public:
	Block() {
		value = 0;
		newSpawned = true;
	}
	void setValue(int value) {
		this->value = value;
	}
	void clearValue() {
		this->value = 0;
	}
	int getValue() {
		return value;
	}
	bool isNew() {
		return newSpawned;
	}
	void setNew(bool value) {
		this->newSpawned = value;
	}

	void render(RenderWindow *frame, int offX, int offY) {
		int size = (frame->getSize().x-5*gapSize) / 4;
		RectangleShape shape(Vector2f(size, size));
		shape.setPosition(gapSize+offX*size+gapSize*offX, gapSize+offY*size+gapSize*offY);

		//if is new
		if (isNew() && value!=0) {
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
		default:
			color = Color::Red;
			break;
		}
		shape.setFillColor(color);
		frame->draw(shape);
	}
};

class Board {
private:
	Block **board;
	int size;
public:
	Board(int size) {
		this->size = size;
		board = new Block*[size];
		for (int i = 0; i < size; i++) {
			board[i] = new Block[size];
		}
		fillBoard();
	}
	~Board() {
		for (int i = 0; i < size; i++) {
			delete[] board[i];
		}
		delete[] board;
	}
	Block *getBlock(int x, int y) {
		return &board[x][y];
	}

	bool isActionEligible(int direction) {
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				if (getBlock(x, y)->getValue() != 0) { //check whether is not empty
					if (direction == Keyboard::Left && x>0) {
						for (int i = x - 1; i >= 0; i--) {
							if (getBlock(i, y)->getValue() == 0 || getBlock(i, y)->getValue() == getBlock(x, y)->getValue()) {
								return true;
							}
							else if (getBlock(i, y)->getValue() != 0) {
								break;
							}
						}
					}
					else if (direction == Keyboard::Right && x<size - 1) {
						for (int i = x + 1; i < size; i++) {
							if (getBlock(i, y)->getValue() == 0 || getBlock(i, y)->getValue() == getBlock(x, y)->getValue()) {
								return true;
							}
							else if (getBlock(i, y)->getValue() != 0) {
								break;
							}
						}
					}
					else if (direction == Keyboard::Up && y>0) {
						for (int i = y - 1; i >= 0; i--) {
							if (getBlock(x, i)->getValue() == 0 || getBlock(x, i)->getValue() == getBlock(x, y)->getValue()) {
								return true;
							}
							else if (getBlock(x, i)->getValue() != 0) {
								break;
							}
						}
					}
					else if (direction == Keyboard::Down && y<size-1) {
						for (int i = y + 1; i < size; i++) {
							if (getBlock(x, i)->getValue() == 0 || getBlock(x, i)->getValue() == getBlock(x, y)->getValue()) {
								return true;
							}
							else if (getBlock(x, i)->getValue() != 0) {
								break;
							}
						}
					}
				}
			}
		}

		return false;
	}

	void makeAction(int direction) {
		for (int stage = 0; stage < 3; stage++) { //stages: align, merge, align again
			if (stage == 1) { //merge
				//mergin in order
				for (int x = 0; x < boardSize; x++) {
					for (int y = 0; y < boardSize; y++) {
						if (getBlock(x, y)->getValue() != 0) { //check whether the field is not empty 
							if (direction == Keyboard::Left && x < boardSize - 1 && getBlock(x+1, y)->getValue() == getBlock(x, y)->getValue()) { //check for neighboring mergable block on horizontal axis
								getBlock(x, y)->setValue(getBlock(x, y)->getValue()*2);
								getBlock(x+1, y)->clearValue();
							}
							else if (direction == Keyboard::Up && y < boardSize - 1 && getBlock(x, y+1)->getValue() == getBlock(x, y)->getValue()) { //check for neighboring mergable block on vertical axis
								getBlock(x, y)->setValue(getBlock(x, y)->getValue() * 2);
								getBlock(x, y+1)->clearValue();
							}
						}
					}
				}
				//merging not in order
				for (int x = boardSize - 1; x >= 0; x--) {
					for (int y = boardSize - 1; y >= 0; y--) {
						if (getBlock(x, y)->getValue() != 0) { //check whether the field is not empty 
							if (direction == Keyboard::Right && x > 0 && getBlock(x-1, y)->getValue() == getBlock(x, y)->getValue()) { //check for neighboring mergable block on horizontal axis
								getBlock(x, y)->setValue(getBlock(x, y)->getValue() * 2);
								getBlock(x-1, y)->clearValue();
							}
							else if (direction == Keyboard::Down && y > 0 && getBlock(x, y-1)->getValue() == getBlock(x, y)->getValue()) { //check for neighboring mergable block on vertical axis
								getBlock(x, y)->setValue(getBlock(x, y)->getValue() * 2);
								getBlock(x, y-1)->clearValue();
							}
						}
					}
				}
			}
			else { //align
				for (int i = 0; i < boardSize - 1; i++) {
					for (int x = 0; x < boardSize; x++) {
						for (int y = 0; y < boardSize; y++) {
							if (getBlock(x, y)->getValue() != 0) { //check whether the field is not empty 
								if (direction == Keyboard::Left && x > 0 && getBlock(x-1, y)->getValue() == 0) { //check for empty space on the left
									getBlock(x-1, y)->setValue(getBlock(x, y)->getValue());
									getBlock(x, y)->clearValue();
								}
								else if (direction == Keyboard::Right && x < boardSize - 1 && getBlock(x+1, y)->getValue() == 0) { //check for empty space on the right
									getBlock(x+1, y)->setValue(getBlock(x, y)->getValue());
									getBlock(x, y)->clearValue();
								}
								else if (direction == Keyboard::Up && y > 0 && getBlock(x, y-1)->getValue() == 0) { //check for empty space on top
									getBlock(x, y-1)->setValue(getBlock(x, y)->getValue());
									getBlock(x, y)->clearValue();
								}
								else if (direction == Keyboard::Down && y < boardSize - 1 && getBlock(x, y+1)->getValue() == 0) { //check for empty space on the bottom
									getBlock(x, y+1)->setValue(getBlock(x, y)->getValue());
									getBlock(x, y)->clearValue();
								}
							}
						}
					}
				}
			}
		}
	}
	void fillBoard() {
		//fill board
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				board[x][y] = Block();
			}
		}

		//set starting blocks
		for (int i = 0; i < startingBlocks; i++) {
			int posX = 0;
			int posY = 0;
			do {
				posX = rand() % size;
				posY = rand() % size;
			} while (getBlock(posX, posY)->getValue() != 0);
			getBlock(posX, posY)->setValue(1);
		}
	}
	void render(RenderWindow *frame) {
		frame->clear();
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				getBlock(x, y)->render(frame, x, y);
			}
		}
		frame->display();
	}
};

void init(RenderWindow *frame) {
	//create window
	frame->create(VideoMode(800, 800), "RainboX The Game", sf::Style::Titlebar | sf::Style::Close);

	//init rng
	srand(time(NULL));
}

bool handleEvents(RenderWindow *frame, Event *externalEvent) {
	bool newEvent = false;
	Event event;
	while (frame->pollEvent(event));
	{
		//window actions
		if (event.type == Event::Closed)
			frame->close();

		//keyboard handling
		else if (event.type == Event::KeyReleased) {
			*externalEvent = event;
			newEvent = true;
		}
	}
	return newEvent;
}

void logic(Board *board, Event event) {
	//set action direction
	int direction = event.key.code;
	if (event.key.code == Keyboard::Enter) { //or restart
		board->fillBoard();
		return;
	}
	else if (!(direction >= Keyboard::Left && direction <= Keyboard::Down)) { //if random key pressed, just return at this point
		return;
	}
	
	//check win/lose condition
	bool boardFull = true;
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			if (board->getBlock(x, y)->getValue() == 0) {
				boardFull = false;
				break;
			}
		}
	}
	if (boardFull) {
		if (!board->isActionEligible(Keyboard::Left) && !board->isActionEligible(Keyboard::Right) && !board->isActionEligible(Keyboard::Up) && !board->isActionEligible(Keyboard::Down)) {
			std::cout << "No possibile moves" << std::endl;
			return;
		}
	}
	
	//check eligibility
	if (!board->isActionEligible(direction)) {
		std::cout << "Action ineligible"<<std::endl;
		return;
	}

	//move blocks
	board->makeAction(direction);

	//unset as new
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			board->getBlock(x, y)->setNew(false);
		}
	}
	
	//spawn new block
	int posX = 0;
	int posY = 0;
	bool bigBlock = false;
	do {
		posX = rand() % (boardSize);
		posY = rand() % (boardSize);
	} while (board->getBlock(posX, posY)->getValue() != 0);

	if (rand() % 100 < bigBlockChance) {
		bigBlock = true;
	}

	if (!bigBlock) {
		board->getBlock(posX, posY)->setValue(1);
	}
	else {
		board->getBlock(posX, posY)->setValue(2);
	}
	board->getBlock(posX, posY)->setNew(true);
}

int main(){
	//variables
	RenderWindow frame;
	Board board(boardSize);

	//init
	init(&frame);

	//game loop
	board.render(&frame);
	while (frame.isOpen())
	{
		//events
		Event event;
		bool needsRefresh = handleEvents(&frame, &event);

		//loop body
		if (needsRefresh) {
			logic(&board, event);
			board.render(&frame);
		}
	}

	return 0;
}
