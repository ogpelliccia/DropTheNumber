//Drop The Number
				  //by Nicholas Pelliccia
//Start Date:			    Feb. 16, 2025
//Revision Date:		    Apr. 04, 2025

//Changes since last revision:
//	complete overhaul of tile height checking system using placeTile
//  removed gridHeight vector
//  moved merging logic to new function checkColumn
//	fixed left and right merging
//	added logic for multiple merges, using recursion of checkColumn
//  partially added logic for combo merges (left and bottom have same value as placed tile)
//  TODO: implement game state structure
//	TODO: implement simple "animation" for multiple merging
//	TODO: fully implement combo merges
//	TODO: implement scoring system

#include <SFML/Graphics.hpp>  //The main SFML Graphics header
#include <iostream>			  //This is used for debug outputs to the console
#include <vector>			  //Vectors are used for the tile grid
#include <random>			  //This is to generate a random texture and value for a tile
#include <string>			  //This is also used for console debugging purposes and the score text
#include <cmath>			  //pow() is used to set the value of each tile
#include <cstdlib>			  //This library is used for enhanced randomization of tiles
#include <ctime>			  //ctime is also used for the enhanced randomization

using namespace sf;

//spriteTextures stores x-coordinates for the textures of each type of block (0 is 2, 101 is 4, 203 is 8, etc)
const int spriteTextures[18] = { 0, 101, 203, 305, 407, 509, 611, 713, 815, 917, 1019, 1121, 1223, 1325, 1427, 1529, 1631, 1733 };

//Tiles have their own structure
struct Tile 
{
	Sprite sprite; //Texture of the tile
	int value;	   //Value of the tile corresponding to its texture (this might be removed in the near future)

	//The argument list for initializing a new tile
	Tile(Sprite inputSprite, int val) 
		: sprite(inputSprite), value(val) {}
};

//Is used to calculate the y-position for the tile to drop
//New in 0.5: this function replaces the gridHeight vector
int countTiles(std::vector<std::vector<Tile>>& grid, int column)
{
	int count = 0;	//The number of non-zero blocks

	for (int i = 0; i < grid[column].size(); i++)
	{
		if (grid[column][i].value != 0)
		{
			count++;
		}
	}

	return count;
}

void checkColumn(std::vector<std::vector<Tile>>&grid, int column, int &scoreAdd)
{
	int top = 0;
	int spriteTexture = 0;

	for (int i = 5; i >= 0; i--)
	{
		if (top == 0 && grid[column][i].value != 0)
		{
			top = i;
		}
	}
	//std::cout << top << std::endl;

	//NOTE: The left and right merging is very buggy and needs improvement. For the sake of functionality, these features are disabled for now
	//Once everything works, these features will be enabled
	for (int i = 0; i < grid[column].size(); i++)
	{
		//Check for Left combo merge (left and bottom tiles are same)
		std::cout << (i > 0 && grid[column][i].value != 0 && grid[column][i].value == grid[column][i - 1].value && grid[column][i].value == grid[column - 1][i].value) << std::endl;
		if (i > 0 && grid[column][i].value != 0 && grid[column][i].value == grid[column][i - 1].value && grid[column][i].value == grid[column - 1][i].value)
		{
			spriteTexture = static_cast<int>(log2(grid[column][i - 1].value));
			grid[column][i - 1].value *= 4;
			grid[column][i - 1].sprite.setTextureRect(IntRect(spriteTextures[spriteTexture], 0, 102, 100));

			grid[column][i].value = 0;
			grid[column][i].sprite.setPosition(0.0f, 0.0f);

			grid[column - 1][i].value = 0;
			grid[column - 1][i].sprite.setPosition(0.0f, 0.0f);

			scoreAdd += grid[column][i].value;
			checkColumn(grid, column, scoreAdd);
		}

		//Check for bottom merge
		else if (i < 5 && grid[column][i].value != 0 && grid[column][i].value == grid[column][i + 1].value)
		{
			spriteTexture = static_cast<int>(log2(grid[column][i].value));
			grid[column][i].value *= 2;
			grid[column][i].sprite.setTextureRect(IntRect(spriteTextures[spriteTexture], 0, 102, 100));

			for (int j = i + 1; j < countTiles(grid, column); j++)
			{
				grid[column][j] = grid[column][j + 1];
				grid[column][j].sprite.setPosition(grid[column][j].sprite.getPosition().x, grid[column][j].sprite.getPosition().y + 102);
			}
			scoreAdd += grid[column][i].value;
			checkColumn(grid, column, scoreAdd);
		}

		//Check for tile sliding after left or right merging
		else if (grid[column][i].value == 0 && i < top)
		{
			for (int j = i + 1; j < countTiles(grid, column) + 1; j++)
			{
				grid[column][j - 1] = grid[column][j];
				grid[column][j - 1].sprite.setPosition(grid[column][j].sprite.getPosition().x, grid[column][j].sprite.getPosition().y + 102);
			}
		}

		//Check for left merge
		else if (column > 0 && grid[column][i].value != 0 && grid[column - 1][i].value == grid[column][i].value)
		{
			spriteTexture = static_cast<int>(log2(grid[column][i].value));
			grid[column - 1][i].value = 0;
			grid[column - 1][i].sprite.setPosition(0.0f, 0.0f);
			checkColumn(grid, column - 1, scoreAdd);

			grid[column][i].value *= 2;
			grid[column][i].sprite.setTextureRect(IntRect(spriteTextures[spriteTexture], 0, 102, 100));
			scoreAdd += grid[column][i].value;

			checkColumn(grid, column, scoreAdd);
		}

		//Check for right merge
		else if (column < 4 && grid[column][i].value != 0 && grid[column + 1][i].value == grid[column][i].value)
		{
			spriteTexture = static_cast<int>(log2(grid[column][i].value));
			grid[column + 1][i].sprite.setPosition(0.0f, 0.0f);
			grid[column + 1][i].value = 0;
			checkColumn(grid, column + 1, scoreAdd);

			grid[column][i].value *= 2;
			grid[column][i].sprite.setTextureRect(IntRect(spriteTextures[spriteTexture], 0, 102, 100));
			scoreAdd += grid[column][i].value;

			checkColumn(grid, column, scoreAdd);
		}
	}
}

//Spawns a new block
void resetSprite(Tile& currentTile, int& column, int& textureIndex)
{
	srand(time(0));					//The random seed is changed every second
	textureIndex = rand() % 7;		//A random number from 0 to 7 is generated (only allowing blocks from 2 to 128)

	//The texture, position, and value are set
	currentTile.sprite.setTextureRect(IntRect(spriteTextures[textureIndex], 0, 102, 100));   
	currentTile.sprite.setPosition(45 + (102 * column), 100);
	currentTile.value = pow(2.0, textureIndex + 1);
}

//Places the tile into a fixed spot on the grid and performs merging
void placeTile(std::vector<std::vector<Tile>>& grid, Tile& currentTile, int& column, int& textureIndex, int& score)
{
	int row = countTiles(grid, column);				//This is for easy navigation of the grid vector
	int scoreAdd = 0;								//This is added to the game score at the end of the function

	//The current tile has its texture and position set
	grid[column][row].value = currentTile.value;		//currentTile's value is moved to the grid vector
	grid[column][row].sprite.setTextureRect(IntRect(spriteTextures[static_cast<int>(log2((double)grid[column][row].value)) - 1], 0, 102, 100));
	grid[column][row].sprite.setPosition(currentTile.sprite.getPosition().x, (600 - (countTiles(grid, column) * 102)) + 102);

	//Console Debug Statements
	std::cout << "Row Index: " << row << std::endl;
	std::cout << "Column Index: " << column << std::endl;
	std::cout << grid[column][row].value << ", " << grid[column][row + 1].value << std::endl;

	//Checks for merging
	checkColumn(grid, column, scoreAdd);

	score += scoreAdd;										//The score is increased if merging occurs
	resetSprite(currentTile, column, textureIndex);			//After the current tile is dropped, a new one is spawned
}

//The game area is reset for a new game
void initializeGame(std::vector<std::vector<Tile>>& grid, Tile& currentTile, int& column, bool& start, Texture& texture, int& textureIndex, int& score)
{
	//The grid is populated with empty tiles
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			grid[i][j].value = 0;
			grid[i][j].sprite.setPosition(0.0f, 0.0f);
		}
	}

	column = 0;		//The column is reset
	score = 0;		//The score is reset to 0

	//A new sprite is spawned
	resetSprite(currentTile, column, textureIndex);

	//If this is the first game, the start boolean changes
	if (start == false)
	{
		start = true;
	}
}


int main()
{
	//The texture file for the tiles is loaded
	Texture texture;
	texture.loadFromFile("Texture-Map.png");

	//The texture for the background is loaded
	Texture bg;
	bg.loadFromFile("Layout.png");

	//const int horizontalPos[5] = { 45, 147, 249, 351, 453 };											//These are the x-coordinates for each column
	std::vector<std::vector<Tile>> grid = { {}, {}, {}, {}, {} };										//The grid keeps track of tiles and their values and positions
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			grid[i].push_back(Tile(sf::Sprite(texture), 0));
		}
	}
	int column = 0;				//Column counter
	int score = 0;				//The score in the game
	int textureIndex = 0;		//The index for the global spriteTextures array
	bool game = false;			//Whether the game is running or not
	bool start = true;			//Used to start a new game
	bool paused = false;		//Whether the game is paused or not

	//The clock is used for the 2-second delay after a game over is triggered
	Clock clock;

	//The main window the game is contained in
	RenderWindow window(VideoMode(600, 800), "Drop The Number!");

	//The current tile is initialized and spawned
	Tile currentTile(Sprite(texture), 0);
	resetSprite(currentTile, column, textureIndex);

	//A sprite is created for the background to be displayed
	Sprite back(bg);

	//The font, which is included with the game, is used for all text
	Font font;

	if (!font.loadFromFile("Seven Segment.ttf"))
	{
		std::cout << "Font could not be loaded" << std::endl;
	}

	//The score text is initialized
	Text scoreText(std::to_string(score), font, 45);
	scoreText.setPosition(237, 40);
	scoreText.setFillColor(Color::Red);

	//Levels and high score tracking have not been implemented yet, so the text for these are disabled

	/*Text lvlText("Lvl 1", font, 45);
	lvlText.setPosition(437, 40);
	lvlText.setFillColor(Color(216, 151, 171));*/

	/*Text highText("7654321", font, 35);
	highText.setPosition(60, 48);
	highText.setFillColor(Color(216, 151, 171));*/

	//The text for the game over/start screen
	Text newGame("Press F2 to Start a New Game", font, 35);
	newGame.setPosition(70, 300);
	newGame.setFillColor(Color::White);

	//When the game is paused, this text shows up
	Text pauseScreen("Game Paused! Press F3 to Unpause", font, 35);
	pauseScreen.setPosition(50, 300);
	pauseScreen.setFillColor(Color::White);

	window.setFramerateLimit(60);		//The game's framerate is capped at 60 FPS (a higher framerate may make the game more difficult)

	//A new game is started
	initializeGame(grid, currentTile, column, start, texture, textureIndex, score);

	while (window.isOpen())
	{
		Event e;		//This is used to evaluate keyboard input

		while (window.pollEvent(e))
		{
			//The window closes
			if (e.type == Event::Closed)
			{
				window.close();
			}
			//The keyboard input is evaluated
			if (e.type == Event::KeyPressed)
			{
				//When the Right Arrow or D keys are pressed, the current tile moves 1 column to the right
				if (currentTile.sprite.getPosition().x < 453 && currentTile.sprite.getPosition().y < 600 - (countTiles(grid, column) * 102) && !paused &&
					(e.key.code == sf::Keyboard::Right || e.key.code == sf::Keyboard::D))
				{
					currentTile.sprite.setPosition(currentTile.sprite.getPosition().x + 102, currentTile.sprite.getPosition().y);
					column += 1;
				}

				//When the Left Arrow or A keys are pressed, the current tile moves 1 column to the left
				if (currentTile.sprite.getPosition().x > 45 && currentTile.sprite.getPosition().y < 600 - (countTiles(grid, column) * 102) && !paused &&
					(e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::A))
				{
					currentTile.sprite.setPosition(currentTile.sprite.getPosition().x - 102, currentTile.sprite.getPosition().y);
					column -= 1;
				}

				//When the Down Arrow or S keys are pressed, the current tile is dropped
				if (currentTile.sprite.getPosition().y <= 600 - (countTiles(grid, column) * 102) && !paused && (e.key.code == sf::Keyboard::Down || e.key.code == sf::Keyboard::S))
				{
					placeTile(grid, currentTile, column, textureIndex, score);
				}
				
				//When the game over screen is displayed, pressing F2 starts a new game
				if (game == false && e.key.code == sf::Keyboard::F2)
				{
					initializeGame(grid, currentTile, column, start, texture, textureIndex, score);
					game = true;
				}

				//Pressing F3 pauses and unpauses the game
				if (game && e.key.code == sf::Keyboard::F3)
				{
					if (paused)
					{
						paused = false;
					}
					else
					{
						paused = true;
					}
				}
			}
		}
		if (!paused)
		{
			if (currentTile.sprite.getPosition().y < 600 - (countTiles(grid, column) * 102))
			{
				currentTile.sprite.move(0.0f, 0.5f);	//The current tile moves downward at a certain speed
			}
			else if (currentTile.sprite.getPosition().y <= 600 - (countTiles(grid, column) * 102))
			{
				//When the current tile lands on top of a stack, it's attributes are transferred to the grid vector
				placeTile(grid, currentTile, column, textureIndex, score);
			}
			else if (game)
			{
				clock.restart();	//The timer starts for the 2-second delay after a game over is triggered
				start = false;		//start is set to false to stop displaying the tiles
				game = false;		//game is set to false to stop displaying the tiles
			}
		}

		//This is debug output to the console
		if (!paused && game)
		{
			//std::cout << "Position: " << currentTile.sprite.getPosition().x << "," << currentTile.sprite.getPosition().y << std::endl;
			//std::cout << "Time Elapsed: " << clock.getElapsedTime().asSeconds() << std::endl;
			//std::cout << "Game Status: " << game << std::endl;

			std::cout << "Column Index: " << column << std::endl;
			std::cout << "Grid Height: " << 600 - (countTiles(grid, column) * 102) << std::endl;
			//std::cout << "Tile Value: " << currentTile.value << std::endl;
			//std::cout << std::to_string(static_cast<int>(log2(currentTile.value))) << std::endl;

			std::cout << "Grid Array: " << std::endl;
			for (int i = 0; i < 5; i++)
			{
				std::cout << std::endl << "[";
				for (int j = 0; j < 6; j++)
				{
					std::cout << std::to_string(grid[i][j].value) << ",";
				}
				std::cout << "]" << std::endl;
			}
			std::cout << std::endl;
		}

		window.clear(Color(200, 200, 200));		//The main window is set to blank
		window.draw(back);						//The background is drawn in the window

		//As long as the game is still going or the timer is less than 2 seconds, the current tile and all dropped tiles are printed
		if ((game == true && !paused) || (clock.getElapsedTime().asSeconds() < 2.0f && !game && !start))
		{
			window.draw(currentTile.sprite);
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					if (grid[i][j].value != 0)
					{
						window.draw(grid[i][j].sprite);
					}
				}
			}
		}
		//When the game is paused, only the pause text is printed
		else if (paused)
		{
			window.draw(pauseScreen);
		}
		else
		{
			//The game over screen is printed 
			window.draw(newGame);
		}
		scoreText.setString(std::to_string(score));			//The score integer is converted to a string so it can be outputted to text
		window.draw(scoreText);								//The score text is printed out
		//window.draw(lvlText);								//This is for the level, which is not implemented yet
		//window.draw(highText);							//This is for the high score, which is not implemented yet
		window.display();									//When everything is ready, the window is displayed in its entirety
	}

	return 0;
}