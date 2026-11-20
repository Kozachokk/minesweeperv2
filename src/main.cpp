#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "main.hpp"

const int map_size_x = 16 * 3;
const int map_size_y = 9 * 3;
const int window_size_x = 2560 - 160 * 3;
const int window_size_y = 1440 - 90 * 3;

const int square_size = (int)(std::min(window_size_x / map_size_x, window_size_y / map_size_y));

int map[map_size_x][map_size_y];
int map_view[map_size_x][map_size_y];

sf::RenderWindow app(sf::VideoMode(sf::Vector2u(window_size_x, window_size_y)), "Minesweeper!", sf::Style::Close);

sf::Texture mine;
sf::Texture flag;
sf::Font font;

int ways[][2] = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};

void open_all_zero_around(int x, int y)
{
	for (auto i : ways)
	{
		if (x + i[0] < map_size_x && x + i[0] >= 0 && y + i[1] < map_size_y && y + i[1] >= 0)
		{
			if (map_view[x + i[0]][y + i[1]] != 0)
			{
				map_view[x + i[0]][y + i[1]] = map[x + i[0]][y + i[1]];
				if (map[x + i[0]][y + i[1]] == 0)
				{
					open_all_zero_around(x + i[0], y + i[1]);
				}
			}
		}
	}
}

void game_over()
{
	sf::Text text(font, "Game over!");
	text.setFillColor(sf::Color::Red);
	text.setCharacterSize(std::min(window_size_x, window_size_y) / 4);
	app.draw(text);
	app.display();

	while (app.isOpen())
	{
		while (const std::optional event = app.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				app.close();
			}
		}
	}
}

void set_numbers()
{
	for (int x = 0; x < map_size_x; x++)
	{
		for (int y = 0; y < map_size_y; y++)
		{
			if (map[x][y] != -1)
			{
				int n = 0;
				for (auto i : ways)
				{
					if (x + i[0] >= map_size_x || x + i[0] < 0 || y + i[1] >= map_size_y || y + i[1] < 0)
						;
					else if (map[x + i[0]][y + i[1]] == -1)
					{
						n++;
					}
				}
				map[x][y] = n;
			}
		}
	}
}

void generate_map()
{
	for (int x = 0; x < map_size_x; x++)
	{
		for (int y = 0; y < map_size_y; y++)
		{

			map_view[x][y] = 10;
			if (random(0, 5) == 1)
			{
				map[x][y] = -1;
			}
			else
			{
				map[x][y] = 0;
			}
		}
	}

	set_numbers();
}

void loop()
{
	while (app.isOpen())
	{
		while (const std::optional event = app.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				app.close();
			}

			if (event->is<sf::Event::MouseButtonPressed>())
			{
				int x = (int)(sf::Mouse::getPosition(app).x / square_size);
				int y = (int)(sf::Mouse::getPosition(app).y / square_size);

				if (x <= map_size_x && x >= 0 && y <= map_size_y && y >= 0)
				{
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
					{
						if (map_view[x][y] == 10)
						{
							map_view[x][y] = 9;
						}
						else if (map_view[x][y] == 9)
						{
							map_view[x][y] = 10;
						}
					}
					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					{
						map_view[x][y] = map[x][y] = -1;
						set_numbers();
						// if (map[x][y] == 0)
						// {
						// 	open_all_zero_around(x, y);
						// }
						// if(map[x][y] == -1){
						// 	return game_over();
						// }
					}
				}
			}
		}

		app.clear(sf::Color::Black);
		for (int x = 0; x < map_size_x; x++)
		{
			for (int y = 0; y < map_size_y; y++)
			{
				sf::RectangleShape rectangle(sf::Vector2f(x * square_size * 2, y * square_size));
				rectangle.setSize(sf::Vector2f(square_size, square_size));
				rectangle.setPosition(sf::Vector2f(x * square_size, y * square_size));
				rectangle.setFillColor(sf::Color::White);
				rectangle.setOutlineColor(sf::Color::Black);
				rectangle.setOutlineThickness(1);
				app.draw(rectangle);

				// Mine
				if (map_view[x][y] == -1)
				{
					sf::Sprite sprite(mine);
					sprite.setPosition(sf::Vector2f(square_size * x, square_size * y));
					sprite.setScale(sf::Vector2f((float)square_size / mine.getSize().x, (float)square_size / mine.getSize().y));
					sprite.setTexture(mine);
					app.draw(sprite);
				}
				else if (map_view[x][y] == 9)
				{
					sf::Sprite sprite(flag);
					sprite.setPosition(sf::Vector2f(square_size * x, square_size * y));
					sprite.setScale(sf::Vector2f((float)square_size / flag.getSize().x, (float)square_size / flag.getSize().y));
					sprite.setTexture(flag);
					app.draw(sprite);
				}
				else if(map_view[x][y] == 0);
				else if (map_view[x][y] == 10)
				{
					sf::Color colors[] = {
						sf::Color::Blue,
						sf::Color::Green,
						sf::Color::Red,
						sf::Color(0, 0, 128),
						sf::Color(128, 0, 0),
						sf::Color(0, 128, 128),
						sf::Color::Black,
						sf::Color(128, 128, 128)};

					sf::Text text(font, std::to_string(map[x][y]));
					text.setCharacterSize(square_size);
					text.setFillColor(colors[map[x][y] - 1]);
					text.setPosition(sf::Vector2f(x * square_size + square_size / 4, y * square_size - square_size / 16 * 3));
					app.draw(text);

					std::cout << std::to_string(colors[-2].r) << ":" << colors[-2].g << ":" << colors[-2].b << std::endl;
				}
			}
		}
		app.display();
	}
}

int main()
{
	flag.loadFromFile("../Assets/flag.png");
	mine.loadFromFile("../Assets/mine.png");
	font.openFromFile("../Assets/arial.ttf");

	generate_map();
	loop();
}