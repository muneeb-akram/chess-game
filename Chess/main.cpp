#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int boxSize = 56;
Vector2f offset(28, 28);

Sprite pieces[32]; //pieces
string position = "";

int board[8][8] = //store codes for piece type
{ -1,-2,-3,-4,-5,-3,-2,-1,
 -6,-6,-6,-6,-6,-6,-6,-6,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 6, 6, 6, 6,
  1, 2, 3, 4, 5, 3, 2, 1 };


int getCode(Vector2f p)
{
	int row, col;
	row = 7 - p.y / boxSize;
	col = p.x / boxSize;
	return board[row][col];
}

string toChessNote(Vector2f p)
{
	string s = "";
	s += char(p.x / boxSize + 97);
	s += char(7 - p.y / boxSize + 49);
	return s;
}

Vector2f toCoord(char a, char b)
{
	int x = int(a) - 97;
	int y = 7 - int(b) + 49;
	return Vector2f(x * boxSize, y * boxSize);
}

void makeMove(string str)
{
	Vector2f oldPos = toCoord(str[0], str[1]);
	Vector2f newPos = toCoord(str[2], str[3]);

	for (int i = 0; i < 32; i++)
		if (pieces[i].getPosition() == newPos) pieces[i].setPosition(-100, -100);

	for (int i = 0; i < 32; i++)
		if (pieces[i].getPosition() == oldPos) pieces[i].setPosition(newPos);

	//castling       //if the king didn't move
	if (str == "e1g1") if (position.find("e1") == -1) makeMove("h1f1");
	if (str == "e8g8") if (position.find("e8") == -1) makeMove("h8f8");
	if (str == "e1c1") if (position.find("e1") == -1) makeMove("a1d1");
	if (str == "e8c8") if (position.find("e8") == -1) makeMove("a8d8");
}

void loadPosition()
{
	int index = 0;
	for (int row = 0; row < 8; row++)
		for (int col = 0; col < 8; col++)
		{
			int code = board[row][col];
			if (code == 0) continue;
			int x = abs(code) - 1;
			int y = 0;
			if (code > 0) y = 1;
			pieces[index].setTextureRect(IntRect(boxSize * x, boxSize * y, boxSize, boxSize));
			pieces[index].setPosition(boxSize * col, boxSize * row);
			index++;
		}

	for (int i = 0; i < position.length(); i += 5)
		makeMove(position.substr(i, 4));
}

int main() {

	RenderWindow window(VideoMode(504, 504), "The Chess! (press SPACE)"); //504x504 window size
	Texture piecesTexture, boardTexture;
	piecesTexture.loadFromFile("figures.png");
	boardTexture.loadFromFile("board.png");
	for (int i = 0; i < 32; i++) pieces[i].setTexture(piecesTexture);
	Sprite boardSprite(boardTexture);
	loadPosition();

	bool isMove = false;
	float dx = 0, dy = 0;
	Vector2f oldPos, newPos;
	string str;
	int n = 0;

	while (window.isOpen())
	{
		Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();

			////undo move//////
			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::BackSpace)
				{
					if (position.length() > 6) position.erase(position.length() - 6, 5); loadPosition();
				}

			/////drag and drop///////
			if (e.type == Event::MouseButtonPressed)
				if (e.key.code == Mouse::Left)
					for (int i = 0; i < 32; i++)
						if (pieces[i].getGlobalBounds().contains(pos.x, pos.y))
						{
							isMove = true; n = i;
							dx = pos.x - pieces[i].getPosition().x;
							dy = pos.y - pieces[i].getPosition().y;
							oldPos = pieces[i].getPosition();
						}

			if (e.type == Event::MouseButtonReleased)
				if (e.key.code == Mouse::Left)
				{

					isMove = false;
					Vector2f p = pieces[n].getPosition() + Vector2f(boxSize / 2, boxSize / 2);
					newPos = Vector2f(boxSize * int(p.x / boxSize), boxSize * int(p.y / boxSize));
					cout << "-moved piece type " << getCode(oldPos) << endl;
					// invalid move logic
					str = toChessNote(oldPos) + toChessNote(newPos);
					makeMove(str);
					if (oldPos != newPos) position += str + " ";
					pieces[n].setPosition(newPos);
				}
		}

		if (isMove) pieces[n].setPosition(pos.x - dx, pos.y - dy);

		////// draw  ///////
		window.clear();
		window.draw(boardSprite);
		for (int i = 0; i < 32; i++) pieces[i].move(offset);
		for (int i = 0; i < 32; i++) window.draw(pieces[i]); window.draw(pieces[n]);
		for (int i = 0; i < 32; i++) pieces[i].move(-offset);
		window.display();
	}


	return 0;
}
