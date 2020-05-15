#include <SFML/Graphics.hpp>
#include <time.h>  // для генерации случайных чисел 

using namespace sf;

// Массив фигурок-тетрамино
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

const int M = 20; // высота игрового поля в кубиках
const int N = 10; // ширина игрового поля в кубиках
int field[M][N] = { 0 }; // игровое поле
// Проверка на выход за границы игрового поля
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
	srand(time(0)); // аналог randomize, привязанный ко времени

	// Объект, который, собственно, является главным окном приложения
	RenderWindow window(VideoMode(320, 480), "Tetris");

	// Создание и загрузка текстур - рисунок кубиков, рисунок рамки и общий фон
	Texture texture, texture_background, texture_frame;
	texture.loadFromFile("images//tiles.png");
	texture_background.loadFromFile("images//background.png");
	texture_frame.loadFromFile("images//frame.png");

	// Создание спрайта
	Sprite sprite(texture), sprite_background(texture_background), sprite_frame(texture_frame);
	const int pixels_in_cube = 18;
	// Вырезаем из спрайта отдельный квадратик размером 18х18 пикселей
	sprite.setTextureRect(IntRect(0, 0, pixels_in_cube, pixels_in_cube));

	// ПЕРЕМЕННЫЕ
	bool begin_game = true;
	int dx = 0; // Переменная для горизонтального перемещения тетрамино
	bool rotate = false; // Переменная для определения, вращать или нет
	int colorNum = 1; // цвет
	Point coordinates_each_cube[4] = { 0 };
	unsigned short figure_number = rand() % 7;

	// Переменные для таймера и задержки
	const float initial_speed = 0.9;
	float timer = 0, delay = initial_speed;
	// Часы (таймер)
	Clock clock;

	// Главный цикл приложения. Выполняется, пока открыто окно
	while (window.isOpen())
	{
		// Получаем время, прошедшее с начала отсчёта, и конвертируем его в секунды
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		// Обрабатываем очередь событий в цикле
		Event event;
		while (window.pollEvent(event))
		{
			// Пользователь нажал на «крестик» и хочет закрыть окно?
			if (event.type == Event::Closed)
				// Тогда закрываем его
				window.close();
			// Была ли нажата клавиша на клавиатуре?
			if (event.type == Event::KeyPressed)
				// Этой клавишей была стрелка вверх?
				if (event.key.code == Keyboard::Up) rotate = true;
			// Или может стрелка влево?
				else if (event.key.code == Keyboard::Left) dx = -1;
			// Или стрелка вправо?
				else if (event.key.code == Keyboard::Right) dx = 1;
		}
		// Нажали на кнопку "Вниз"? Ускоряем падение тетрамино
		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.05;

		// ГОРИЗОНТАЛЬНОЕ СМЕЩЕНИЕ
		Point temp_coordinates[4] = { 0 }; // вспомогательный массив для временных координат
		for (int i = 0; i < 4; i++) 
		{ 
			temp_coordinates[i] = coordinates_each_cube[i];
			coordinates_each_cube[i].x += dx*pixels_in_cube;
		}
		// Если вышли за пределы поля после перемещения, то возвращаем старые координаты
		if (!check(pixels_in_cube, coordinates_each_cube))
		{
			for (int i = 0; i < 4; i++)
				coordinates_each_cube[i] = temp_coordinates[i];
		}
		dx = 0; // обнуляем смещение фигуры

		// ВРАЩЕНИЕ
		if (rotate)
		{
			Point centre_of_rotation = coordinates_each_cube[1]; // указываем центр вращения
			for (int i = 0; i < 4; i++)
			{
				int x = coordinates_each_cube[i].y - centre_of_rotation.y; // y - y0
				int y = coordinates_each_cube[i].x - centre_of_rotation.x; // x - x0
				coordinates_each_cube[i].x = centre_of_rotation.x - x;
				coordinates_each_cube[i].y = centre_of_rotation.y + y;
			}
			// Если вышли за пределы поля после перемещения, то возвращаем старые координаты
			if (!check(pixels_in_cube, coordinates_each_cube))
			{
				for (int i = 0; i < 4; i++)
					coordinates_each_cube[i] = temp_coordinates[i];
			}
		}
		rotate = false;

		// ДВИЖЕНИЕ ТЕТРАМИНО ВНИЗ («тик» таймера)
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++)
			{ 
				temp_coordinates[i] = coordinates_each_cube[i];
				coordinates_each_cube[i].y += pixels_in_cube;
			}

			// Если достигли дна:
			if (!check(pixels_in_cube, coordinates_each_cube))
			{
				for (int i = 0; i < 4; i++)
				{
					// возвращаем старые координаты
					coordinates_each_cube[i] = temp_coordinates[i];
					// обозначаем координаты фигуры цветом для проверки, занято ли поле
					field[temp_coordinates[i].y / pixels_in_cube][temp_coordinates[i].x / pixels_in_cube] = colorNum;
				}
				// для следующей фигуры генерируем случайный цвет и случайную фигуру:
				colorNum = 1 + rand() % 7;
				figure_number = rand() % 7;
				// генерируем следующую фигуру:
				for (int i = 0; i < 4; i++)
					{
						coordinates_each_cube[i].x = (figures[figure_number][i] % 2)*pixels_in_cube;
						coordinates_each_cube[i].y = (figures[figure_number][i] / 2)*pixels_in_cube;
					}
			}
			timer = 0;
		}

		// ПРОВЕРКА ПОЛНОЙ ЛИНИИ
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

		// Первое появление тетрамино на поле?
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

		//----ОТРИСОВКА----//
		window.clear(Color::White); // Установка цвета фона - белый
		window.draw(sprite_background);
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
			{
				if (field[i][j] == 0) continue;
				sprite.setTextureRect(IntRect(field[i][j] * pixels_in_cube, 0, pixels_in_cube, pixels_in_cube));
				sprite.setPosition(j * pixels_in_cube, i * pixels_in_cube);
				sprite.move(28, 31); // смещение
				window.draw(sprite);
			}
		for (unsigned short i=0; i<4; i++)
			{
				// Разукрашиваем тетрамино
				sprite.setTextureRect(IntRect(colorNum * pixels_in_cube, 0, pixels_in_cube, pixels_in_cube));
				// Устанавливаем позицию каждого кусочка тетрамино
				sprite.setPosition(coordinates_each_cube[i].x, coordinates_each_cube[i].y);
				sprite.move(28, 31); // смещение
				// Отрисовка спрайта
				window.draw(sprite);
			}
		// Отрисовка фрейма
		window.draw(sprite_frame);
		// Отрисовка окна	
		window.display();
	}

	return 0;
}