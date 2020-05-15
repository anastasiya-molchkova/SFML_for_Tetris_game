#include <SFML/Graphics.hpp>
#include <time.h>  // ��� ��������� ��������� ����� 

using namespace sf;

// ������ �������-���������
const int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // S
	3,5,4,6, // Z
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

struct Point
{
	int x;
	int y;
};

const int M = 20; // ������ �������� ���� � �������
const int N = 10; // ������ �������� ���� � �������
int field[M][N] = { 0 }; // ������� ����
// �������� �� ����� �� ������� �������� ����
bool check(const int pixels_in_cube, Point coordinates_each_cube[4])
{
	for (int i = 0; i < 4; i++)
		if ((coordinates_each_cube[i].x < 0) || (coordinates_each_cube[i].x >= (N * pixels_in_cube)) || (coordinates_each_cube[i].y >= (M * pixels_in_cube)))
			return 0;
		else if (field[coordinates_each_cube[i].y / pixels_in_cube][coordinates_each_cube[i].x / pixels_in_cube])
			return 0;
	return 1;
}

int main()
{
	srand(time(0)); // ������ randomize, ����������� �� �������

	// ������, �������, ����������, �������� ������� ����� ����������
	RenderWindow window(VideoMode(320, 480), "Tetris");

	// �������� � �������� ������� - ������� �������, ������� ����� � ����� ���
	Texture texture, texture_background, texture_frame;
	texture.loadFromFile("images//tiles.png");
	texture_background.loadFromFile("images//background.png");
	texture_frame.loadFromFile("images//frame.png");

	// �������� �������
	Sprite sprite(texture), sprite_background(texture_background), sprite_frame(texture_frame);
	const int pixels_in_cube = 18;
	// �������� �� ������� ��������� ��������� �������� 18�18 ��������
	sprite.setTextureRect(IntRect(0, 0, pixels_in_cube, pixels_in_cube));

	// ����������
	bool begin_game = true;
	int dx = 0; // ���������� ��� ��������������� ����������� ���������
	bool rotate = false; // ���������� ��� �����������, ������� ��� ���
	int colorNum = 1; // ����
	Point coordinates_each_cube[4] = { 0 };
	unsigned short figure_number = rand() % 7;

	// ���������� ��� ������� � ��������
	const float initial_speed = 0.9;
	float timer = 0, delay = initial_speed;
	// ���� (������)
	Clock clock;

	// ������� ���� ����������. �����������, ���� ������� ����
	while (window.isOpen())
	{
		// �������� �����, ��������� � ������ �������, � ������������ ��� � �������
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		// ������������ ������� ������� � �����
		Event event;
		while (window.pollEvent(event))
		{
			// ������������ ����� �� �������� � ����� ������� ����?
			if (event.type == Event::Closed)
				// ����� ��������� ���
				window.close();
			// ���� �� ������ ������� �� ����������?
			if (event.type == Event::KeyPressed)
				// ���� �������� ���� ������� �����?
				if (event.key.code == Keyboard::Up) rotate = true;
			// ��� ����� ������� �����?
				else if (event.key.code == Keyboard::Left) dx = -1;
			// ��� ������� ������?
				else if (event.key.code == Keyboard::Right) dx = 1;
		}
		// ������ �� ������ "����"? �������� ������� ���������
		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

		// �������������� ��������
		Point temp_coordinates[4] = { 0 }; // ��������������� ������ ��� ��������� ���������
		for (int i = 0; i < 4; i++) 
		{ 
			temp_coordinates[i] = coordinates_each_cube[i];
			coordinates_each_cube[i].x += dx*pixels_in_cube;
		}
		// ���� ����� �� ������� ���� ����� �����������, �� ���������� ������ ����������
		if (!check(pixels_in_cube, coordinates_each_cube))
		{
			for (int i = 0; i < 4; i++)
				coordinates_each_cube[i] = temp_coordinates[i];
		}
		dx = 0; // �������� �������� ������

		// ��������
		if (rotate)
		{
			Point centre_of_rotation = coordinates_each_cube[1]; // ��������� ����� ��������
			for (int i = 0; i < 4; i++)
			{
				int x = coordinates_each_cube[i].y - centre_of_rotation.y; // y - y0
				int y = coordinates_each_cube[i].x - centre_of_rotation.x; // x - x0
				coordinates_each_cube[i].x = centre_of_rotation.x - x;
				coordinates_each_cube[i].y = centre_of_rotation.y + y;
			}
			// ���� ����� �� ������� ���� ����� �����������, �� ���������� ������ ����������
			if (!check(pixels_in_cube, coordinates_each_cube))
			{
				for (int i = 0; i < 4; i++)
					coordinates_each_cube[i] = temp_coordinates[i];
			}
		}
		rotate = false;

		// �������� ��������� ���� (���� �������)
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++)
			{ 
				temp_coordinates[i] = coordinates_each_cube[i];
				coordinates_each_cube[i].y += pixels_in_cube;
			}

			// ���� �������� ���:
			if (!check(pixels_in_cube, coordinates_each_cube))
			{
				for (int i = 0; i < 4; i++)
				{
					// ���������� ������ ����������
					coordinates_each_cube[i] = temp_coordinates[i];
					// ���������� ���������� ������ ������ ��� ��������, ������ �� ����
					field[temp_coordinates[i].y / pixels_in_cube][temp_coordinates[i].x / pixels_in_cube] = colorNum;
				}
				// ��� ��������� ������ ���������� ��������� ���� � ��������� ������:
				colorNum = 1 + rand() % 7;
				figure_number = rand() % 7;
				// ���������� ��������� ������:
				for (int i = 0; i < 4; i++)
					{
						coordinates_each_cube[i].x = (figures[figure_number][i] % 2)*pixels_in_cube;
						coordinates_each_cube[i].y = (figures[figure_number][i] / 2)*pixels_in_cube;
					}
			}
			timer = 0;
		}

		// �������� ������ �����
		int k = M - 1; // 
		for (int i = M - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < N; j++)
			{
				if (field[i][j]) count++;
				field[k][j] = field[i][j];
			}
			if (count < N) k--;
		}

		// ������ ��������� ��������� �� ����?
		if (begin_game)
		{
			begin_game = false;
			figure_number = rand() % 7;
			for (unsigned short i = 0; i < 4; i++)
				{
					coordinates_each_cube[i].x = (figures[figure_number][i] % 2) * pixels_in_cube;
					coordinates_each_cube[i].y = (figures[figure_number][i] / 2) * pixels_in_cube;
				}
		}	
		delay = initial_speed;

		//----���������----//
		window.clear(Color::White); // ��������� ����� ���� - �����
		window.draw(sprite_background);
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
			{
				if (field[i][j] == 0) continue;
				sprite.setTextureRect(IntRect(field[i][j] * pixels_in_cube, 0, pixels_in_cube, pixels_in_cube));
				sprite.setPosition(j * pixels_in_cube, i * pixels_in_cube);
				sprite.move(28, 31); // ��������
				window.draw(sprite);
			}
		for (unsigned short i=0; i<4; i++)
			{
				// ������������� ���������
				sprite.setTextureRect(IntRect(colorNum * pixels_in_cube, 0, pixels_in_cube, pixels_in_cube));
				// ������������� ������� ������� ������� ���������
				sprite.setPosition(coordinates_each_cube[i].x, coordinates_each_cube[i].y);
				sprite.move(28, 31); // ��������
				// ��������� �������
				window.draw(sprite);
			}
		// ��������� ������
		window.draw(sprite_frame);
		// ��������� ����	
		window.display();
	}

	return 0;
}