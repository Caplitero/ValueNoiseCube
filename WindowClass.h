#pragma once

#include <SFML/Graphics.hpp>
#include "ValueClass.h"
#include "Cube.h"

class Window {

	
	sf::RenderWindow* window     = nullptr;  // Render window created for sfml to interact with the user
	float*            drawVector = nullptr;  
	Cube*             cube       = nullptr;  // Pointer to the cuber created on start
        ValueNoise*       Map        = nullptr;  // Value Noise Map , with a pointer to the object
	
	sf::VertexArray   Map2D;  // Vertex array used to save the newly created faces
	sf::RenderTexture faces;  // Image used to save the map of the value noise ,and convert it to texture    
	sf::VertexArray   Cube3D; // Vertex array used to draw the cube on the screen 
	
	sf::Vector2i mouseStart; // Start position of the mouse , used to calculate angle for cube rotation
	sf::Vector2i mouseEnd;   // End position of the mouse , used to calculate angle for cube rotation
	sf::Mouse    mouse;      // Mouse of the user , with the position on screen
	
	int  tile_width  = 129;   
	int  tile_height = 129;   // width , height and depth of the value noise map
	int  tile_depth  = 129;
        bool changes     = 0;     // Check if changes are done to the value map
	
        // This vector will be used to store all faces corners positions (x,y) in the texture image
	std::vector<std::vector<std::vector<int> > >positions =
	{
	{   {0             , 0 }, {tile_width    , 0 },{tile_width     ,tile_width }, {            0,tile_width} }, // front
	{   {tile_width * 1, 0 }, {tile_width * 2, 0 },{tile_width  *2 ,tile_width }, {tile_width    ,tile_width}}, // back
	{   {tile_width * 2, 0 }, {tile_width * 3, 0 },{tile_width * 3 ,tile_width }, {tile_width * 2,tile_width}}, // up
	{   {tile_width * 3, 0 }, {tile_width * 4, 0 },{tile_width * 4 ,tile_width }, {tile_width * 3,tile_width}}, // down
	{   {tile_width * 4, 0 }, {tile_width * 5, 0 },{tile_width * 5 ,tile_width }, {tile_width * 4,tile_width}}, // left
	{   {tile_width * 5, 0 }, {tile_width * 6, 0 },{tile_width * 6 ,tile_width }, {tile_width * 5,tile_width}}, //right
	}; 

	void updateDraw()
	{   
     for (int face = 0; face < 6; face++)
      for (int y = 0; y < tile_height; y++)
	for (int x = 0; x < tile_width; x++)
	       { 
		int       indexTexture = x + y * tile_width ;                         // get position on the face
	        int       indexWindow  = (x + tile_width* face) + (y * tile_width*6); // get position inside texture image
		float     value        = Map->output[face][indexTexture] * 15;        // get value of the point from the face
		sf::Color color        = sf::Color(0,0,255*value);                    // Color can be set to anything
			
		// The most imporant step is to update the position in space of the point and its color
		Map2D[indexWindow].position = sf::Vector2f(x + positions[face][0][0], y);
	        Map2D[indexWindow].color = color;  
		}
			
	faces.clear();      // Clear face , such the new draw won't be affected
	faces.draw(Map2D);  // Draw the newly created faces

	for (int i = 0; i < cube->show.size(); i++) {
		std::vector<int> faceid = cube->show[i];    // Go trough the faces 
		sf::Vertex*      face   = &Cube3D[i * 4];   // Set the new quad (face of the cube)
		for (int corner = 0; corner < 4; corner++)
			{
			int id                 = faceid[4]; // Get the face in which the point takes part
			face[corner].position  = sf::Vector2f(cube->C3D[faceid[corner]].x, cube->C3D[faceid[corner]].y);
			face[corner].texCoords = sf::Vector2f(positions[id][corner][0], positions[id][corner][1]);
			}
		}
			
	}
	
        // Function used to update the screen 
	void update()
	{
		sf::RenderStates states;  // The state will save all the custom states , including a texture image
		faces.display();          // Everything saved in the faces memory , will be drawn on it
		states.texture = &faces.getTexture(); // Load the texture
		window->clear();                      // Clear the screen
		window->draw(Cube3D,states);          // Draw the cube to the window , with the custom states (texture)
		window->display();                    // Everything saved in the memory of the window , will be drawn on it
	}
        
	void close() {delete window;}
	
	bool ok = 0;  // Bool to save if the mouse is still pressed or not
	void loop()
	{
		sf::Event event;      // Class used to save all events meet by the application
		while (window->isOpen())
		{
			changes = 0; // This variable will save if any changes are made to the value noise map
                                     // By that , the map does not need to be transformed every frame , and reduce lag
			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window->close();    // If the X button is pressed, the app will stop

				if (event.type == sf::Event::KeyPressed) // Check if any button is pressed
				{
					if (event.key.code == sf::Keyboard::Q){
				         Map->gen_NewSeed(); // Generate new map 
					 changes = 1;}
					if (event.key.code == sf::Keyboard::A){Map->_octaves++;  changes = 1;}
					if (event.key.code == sf::Keyboard::D){Map->_octaves--;  changes = 1;}
					if (event.key.code == sf::Keyboard::R){Map->_noiseCancelling -= 0.05;changes = 1;}
					if (event.key.code == sf::Keyboard::T){Map->_noiseCancelling += 0.05;changes = 1;}

				}

				if (event.type == sf::Event::MouseButtonPressed) // Check if mouse is pressed (for cube rotation)
				{
					mouseStart = mouse.getPosition(*window); // Save the position of the mouse ,
					                                         // inside the window
					ok = 1;                                  // Mouse in use
				}
				if (event.type == sf::Event::MouseButtonReleased) 
				{

					mouseEnd = mouse.getPosition(*window);  // Get the new Mouse Position
					float dx = mouseEnd.x - mouseStart.x;   // Calculate x distance between mouse positions
					float dy = mouseEnd.y - mouseStart.y;   // Calculate y distance between mouse positions
					
                                        float disP1 = cube->distanceTo(mouseStart.x,  // Calculate distance from 
								       mouseStart.y,  // the center of the cube
								       posz);         // Used to convert the distance to angle     
					                                                                    
					float alfaX = atan2f(dx, disP1);  // Calculate the X angle between the mouse positions
					float alfaY = atan2f(dy, disP1);  // Calculate the Y angle between the mouse positions
                                        
					cube->rotate(alfaY, alfaX, 0);        // Rotate the cube by the angles found
					cube->draw(window->getSize().x / 2, window->getSize().y / 2, posz); // Draw the updated cube
					Cube3D.resize(cube->show.size() * 4); // Update the Vertex Array to the new number of faces shown


					ok = 0; // The Mouse is no longer used

				}
				if (ok == 1)
				{       // If the mouse is still used , the rotation is on and the angles can be computed once again
					mouseEnd    = mouse.getPosition(*window);
					float dx    = mouseEnd.x - mouseStart.x;
					float dy    = mouseEnd.y - mouseStart.y;
					float disP1 = cube->distanceTo(mouseStart.x, mouseStart.y, posz);
					float alfaX = atan2f(dx, disP1);
					float alfaY = atan2f(dy, disP1);

					cube->rotate(-alfaY, alfaX, 0);
					cube->draw(window->getSize().x / 2, window->getSize().y / 2, posz);
					Cube3D.resize(cube->show.size() * 4);
					
					mouseStart = mouseEnd;  // This is the only difference , because the next position of 
					                        // the mouse will use this as point of reference

				}



			}
			if (changes)  
			Map->TransformCube3D(); // If changes are done to the map , it will be transformed 
			                        // with the new settings
			
			updateDraw();           // Update the cube
			update();               // Update the window
		}
		close(); // Close game
	}
public:

	int posz = 0;

	void start(unsigned int Width, unsigned int Height, const char* Title)
	{
		window = new sf::RenderWindow(sf::VideoMode(Width, Height), Title); // Init the window
		Map    = new ValueNoise(tile_width, tile_height, tile_depth);       // Init the Value Map
		cube   = new Cube(300);                                             // Init the Cube
		
	        faces.create(tile_width * 6, tile_height); // Init the texture image
		Cube3D.setPrimitiveType(sf::Quads);        // Init the Cube Vertex Array
		
		cube->move(Width / 2, Height / 2, -100); // Move the Cube to the center of the screen , for a better representation
		cube->draw(Width / 2, Height / 2, posz); // Compute the cube , with the coresponding view coordinates
		Cube3D.resize(cube->show.size() * 4);    // Resize the vertex array to the number of shown faces
		
		Map2D.setPrimitiveType(sf::Points);         // Init the texture vertex array
		Map2D.resize(tile_width * tile_height * 6); // Resize the vertex because it has 6 faces

		Map->gen_NewSeed();     // Generate a new map
		Map->TransformCube3D(); // Transform the map , to be used by the cube

		loop();

	}

};
