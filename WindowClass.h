#pragma once

#include <SFML/Graphics.hpp>
#include "ValueClass.h"
#include "Cube.h"





class Window {

	



	sf::RenderWindow* window = nullptr;
	float* drawVector = nullptr;
    sf::RenderTexture faces;
	Cube* cube = nullptr;
	sf::VertexArray Cube3D;
	sf::Vector2i mouseStart;
	sf::Vector2i mouseEnd;
	sf::Mouse mouse;


	ValueNoise* Map;
	int tile_width = 129;
	int tile_height = 129;
	int tile_depth = 129;


	std::vector<std::vector<std::vector<int> > >positions = {
		{   {0,0 }, {tile_width ,0 },{tile_width ,tile_width }, {0,tile_width} }, //  front
		{ {tile_width*1,0 }, {tile_width*2 ,0 },{tile_width*2 ,tile_width }, {tile_width,tile_width}}, // back
		{ {tile_width * 2,0 }, {tile_width * 3 ,0 },{tile_width * 3 ,tile_width }, {tile_width*2,tile_width}}, // up
		{ {tile_width * 3,0 }, {tile_width * 4 ,0 },{tile_width * 4 ,tile_width }, {tile_width * 3,tile_width}}, // down
		{ {tile_width * 4 , 0 }, {tile_width * 5 ,0 },{tile_width * 5 ,tile_width }, {tile_width * 4,tile_width}},// left
		{ {tile_width * 5,0 }, {tile_width * 6 ,0 },{tile_width * 6 ,tile_width }, {tile_width * 5,tile_width}}, //right

	};

	sf::VertexArray Map2D;
	void updateDraw()
	{   
	
		
		for (int j = 0; j < 6; j++)
		{
			
			for (int y = 0; y < tile_height; y++)
				for (int x = 0; x < tile_width; x++)
				{ 
					int indexTexture = y * tile_width + x;
					int indexWindow  = x+tile_width*j + y * tile_width*6;
					float con = Map->output[j][indexTexture] * 15;
					sf::Color color = sf::Color(0,0,255*con);
					Map2D[indexWindow].color = color;
					Map2D[indexWindow].position = sf::Vector2f(x + positions[j][0][0], y);

				}
					
		}
		faces.clear();
		faces.draw(Map2D);

		for (int i = 0; i < cube->show.size(); i++) {
			std::vector<int> faceid = cube->show[i];
			sf::Vertex* face = &Cube3D[i * 4];
			for (int j = 0; j < 4; j++)
			{
				int id = faceid[4];
				face[j].position = sf::Vector2f(cube->C3D[faceid[j]].x, cube->C3D[faceid[j]].y);
				face[j].texCoords = sf::Vector2f(positions[id][j][0], positions[id][j][1]);
			}


	

		}
			



		
	}


	void update()
	{
		sf::RenderStates states;
		faces.display();
		states.texture = &faces.getTexture();
		window->clear();
		window->draw(Cube3D,states);
		window->display();
	}

	void close() {

		delete window;
	}


	bool ok = 0;
	void loop()
	{
		sf::Event event;
		while (window->isOpen())
		{
			changes = 0;

			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window->close();

				}
				if (event.type == sf::Event::KeyPressed)
				{


					if (event.key.code == sf::Keyboard::Q)
					{
						Map->gen_NewSeed();
						changes = 1;
					}

					if (event.key.code == sf::Keyboard::A)
					{
						Map->_octaves++;
						changes = 1;
					}
					if (event.key.code == sf::Keyboard::D)
					{
						Map->_octaves--;
						changes = 1;
					}
					if (event.key.code == sf::Keyboard::Space && posz < tile_depth - 1)
					{
						posz++;
					}
					if (event.key.code == sf::Keyboard::LShift && posz > 0)
					{
						posz--;
					}

					if (event.key.code == sf::Keyboard::R)
					{
						Map->_noiseCancelling -= 0.05;
						changes = 1;
					}
					if (event.key.code == sf::Keyboard::T)
					{
						Map->_noiseCancelling += 0.05;
						changes = 1;
					}

					




				}

				if (event.type == sf::Event::MouseButtonPressed)
				{
					mouseStart = mouse.getPosition(*window);

					ok = 1;
				}
				if (event.type == sf::Event::MouseButtonReleased)
				{

					mouseEnd = mouse.getPosition(*window);
					float dx = mouseEnd.x - mouseStart.x;
					float dy = mouseEnd.y - mouseStart.y;
					float disP1 = cube->distanceTo(mouseStart.x, mouseStart.y, posz);

					float alfaX = atan2f(dx, disP1);
					float alfaY = atan2f(dy, disP1);

					cube->rotate(alfaY, alfaX, 0);
					cube->draw(window->getSize().x / 2, window->getSize().y / 2, posz);
					Cube3D.resize(cube->show.size() * 4);


					ok = 0;

				}
				if (ok == 1)
				{
					mouseEnd = mouse.getPosition(*window);
					float dx = mouseEnd.x - mouseStart.x;
					float dy = mouseEnd.y - mouseStart.y;
					float disP1 = cube->distanceTo(mouseStart.x, mouseStart.y, posz);

					float alfaX = atan2f(dx, disP1);
					float alfaY = atan2f(dy, disP1);

					cube->rotate(-alfaY, alfaX, 0);
					cube->draw(window->getSize().x / 2, window->getSize().y / 2, posz);
					Cube3D.resize(cube->show.size() * 4);
					mouseStart = mouseEnd;

				}



			}
			if (changes)
			{
				Map->TransformCube3D();
				
			}
			updateDraw();
			update();


		}
		close();


	}


public:

	
	int posz = 0;
	bool changes = 0;
	

	

	void start(unsigned int Width, unsigned int Height, const char* Title)
	{

		window = new sf::RenderWindow(sf::VideoMode(Width, Height), Title);
		Map = new ValueNoise(tile_width, tile_height, tile_depth);
		cube = new Cube(300);
		Cube3D.setPrimitiveType(sf::Quads);
		cube->move(Width / 2, Height / 2, -100);
		cube->draw(Width / 2, Height / 2, posz);
		Cube3D.resize(cube->show.size() * 4);
		faces.create(tile_width * 6, tile_height);
		Map2D.setPrimitiveType(sf::Points);
		Map2D.resize(tile_width * tile_height * 6);

		Map->gen_NewSeed();
		 Map->TransformCube3D();

		loop();

	}

};