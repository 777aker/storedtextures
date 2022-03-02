/*
* HW07: Stored Textures
* 
* Key bindings:
* arrows - change view angle
* PgDn/PgUp - zoom in and out
* 0 - reset view angle
* -/+ - light height
* 1 - change mode
* ESC - Exit
*/
#include "x239lib/CSCIx239.h"
#include <time.h>
int zh = 0; // light position
float lh = 0; // light height
int th = 0, ph = 0; // view angles
int fov = 57; // field of view
int tex; // texture
float asp = 1; // aspect ratio
float dim = 3; // size of world
int shader; // shader
int mode = 3; // mode

// noise based on 
// http://www.science-and-fiction.org/rendering/noise.html
float rand2D(int xcor, int ycor) {
	float rand = xcor * 12.9898 + ycor * 78.233;
	rand = Sin(rand) * 43758.5453;
	rand = rand - floor(rand);
	return rand;
}

// create 2D noise texture (based on noise.c)
// gonna do voronoi, tutorial here
// https://www.ronja-tutorials.com/post/028-voronoi-noise/#3d-voronoi-1
int myNoiseTex(int unit) {
	// size of the texture
	const int size = 128;
	// texture arrawy
	float pix[size * size * 4];
	// how big we want a voronoi cell to be
	// should divide size with no remainder
	float cellsize = 16;
	// declaring some variables outside the for loop for efficiency
	int point;
	int valx;
	int valy;
	float distance;
	float tempdis;
	int vorx = 0;
	int vory = 0;
	// make voronoi different across runs very simply
	// by just adding a simple random number
	srand(time(NULL));
	int r = rand();
	// voronoi points
	float voronoi[size / (int) cellsize][size / (int) cellsize][2];
	// fill in our voronoi points
	for (int i = 0; i < size / cellsize; i++) {
		for (int j = 0; j < size / cellsize; j++) {
			voronoi[i][j][0] = rand2D(j + r, i);
			voronoi[i][j][1] = rand2D(i + r, j);
		}
	}
	// voronoi colors? sections? idk, categorization?
	// stores 10 different types
	float vorsections[size / (int) cellsize][size / (int) cellsize][3];
	for (int i = 0; i < size / cellsize; i++) {
		for (int j = 0; j < size / cellsize; j++) {
			switch ((int)floor(rand2D(i + r, j + r) * 10)) {
			case 0:
				vorsections[i][j][0] = 1;
				vorsections[i][j][1] = 0;
				vorsections[i][j][2] = 0;
				break;
			case 1:
				vorsections[i][j][0] = .5;
				vorsections[i][j][1] = .5;
				vorsections[i][j][2] = .5;
				break;
			case 2:
				vorsections[i][j][0] = .2;
				vorsections[i][j][1] = 0;
				vorsections[i][j][2] = .5;
				break;
			case 3:
				vorsections[i][j][0] = .6;
				vorsections[i][j][1] = .4;
				vorsections[i][j][2] = .2;
				break;
			case 4:
				vorsections[i][j][0] = 1;
				vorsections[i][j][1] = 1;
				vorsections[i][j][2] = 1;
				break;
			case 5:
				vorsections[i][j][0] = 1;
				vorsections[i][j][1] = 0;
				vorsections[i][j][2] = 1;
				break;
			case 6:
				vorsections[i][j][0] = 0;
				vorsections[i][j][1] = 1;
				vorsections[i][j][2] = 0;
				break;
			case 7:
				vorsections[i][j][0] = 0;
				vorsections[i][j][1] = 0;
				vorsections[i][j][2] = 1;
				break;
			case 8:
				vorsections[i][j][0] = 0;
				vorsections[i][j][1] = .4;
				vorsections[i][j][2] = .8;
				break;
			case 9:
				vorsections[i][j][0] = .4;
				vorsections[i][j][1] = .8;
				vorsections[i][j][2] = .4;
				break;
			default:
				vorsections[i][j][0] = 0.2;
				vorsections[i][j][1] = 0.2;
				vorsections[i][j][2] = 0.2;
				break;
			}
		}
	}

	// loop through all the pixels of our texture
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			// sooo, math means pixel point =
			point = y * size * 4 + x * 4;
		    // this is which voronoi grid point / cell we are in
			float xval = x / cellsize;
			float yval = y / cellsize;
			// floor it so we can access voronoi array for this point
			valx = floor(xval);
			valy = floor(yval);
			// make distance really big since we need min distance
			distance = 50;
			// check to see if neighboring voronoi points are closer
			// if they are then that is our distance
			for (int i = -1; i <= 1 && i < size / (int)cellsize; i++) {
				for (int j = -1; j <= 1 && size / (int)cellsize; j++) {
					// but our voronoi points aren't really points in space
					// but a point away from bottom left of their cell
					// so we have to do some weird math 
					float xvor;
					float yvor;
					if (i == -1)
						xvor = 1 - voronoi[valx + i][valy + j][0] + (xval - floor(xval));
					else if (i == 0)
						xvor = voronoi[valx + i][valy + j][0] - (xval - floor(xval));
					else
						xvor = 1 - (xval - floor(xval)) + voronoi[valx + i][valy + j][0];
					if (j == -1)
						yvor = 1 - voronoi[valx + i][valy + j][1] + (yval - floor(yval));
					else if (j == 0)
						yvor = voronoi[valx + i][valy + j][1] - (yval - floor(yval));
					else
						yvor = 1 - (yval - floor(yval)) + voronoi[valx + i][valy + j][1];
						
					tempdis = sqrt(pow(xvor, 2) +
						pow(yvor, 2));
					if (tempdis < distance) {
						distance = tempdis;
						vorx = valx + i;
						vory = valy + j;
					}
				}
			}
			// different display modes just for fun
			switch (mode) {
			case 0:
				pix[point] = 0;
				pix[point + 1] = 0;
				pix[point + 2] = (1 - distance) * vorsections[vorx][vory][2];
				pix[point + 3] = 1;
				break;
			case 1:
				pix[point] = 0;
				pix[point + 1] = 0;
				pix[point + 2] = (1 - distance);
				pix[point + 3] = 1;
				break;
			case 2:
				pix[point] = 0;
				pix[point + 1] = 0;
				pix[point + 2] = vorsections[vorx][vory][2];
				pix[point + 3] = 1;
				break;
			case 3:
				pix[point] = 0;
				pix[point + 1] = 0;
				pix[point + 2] = (distance);
				pix[point + 3] = 1;
				break;
			}
			
		}
	}

	cellsize *= 2;
	// not sure if I did stored textures assignment requirements
	// so a second voronoi smaller
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			// sooo, math means pixel point =
			point = y * size * 4 + x * 4;
			// this is which voronoi grid point / cell we are in
			float xval = x / cellsize;
			float yval = y / cellsize;
			// floor it so we can access voronoi array for this point
			valx = floor(xval);
			valy = floor(yval);
			// make distance really big since we need min distance
			distance = 50;
			// check to see if neighboring voronoi points are closer
			// if they are then that is our distance
			for (int i = -1; i <= 1 && i < size / (int)cellsize; i++) {
				for (int j = -1; j <= 1 && size / (int)cellsize; j++) {
					// but our voronoi points aren't really points in space
					// but a point away from bottom left of their cell
					// so we have to do some weird math 
					float xvor;
					float yvor;
					if (i == -1)
						xvor = 1 - voronoi[valx + i][valy + j][0] + (xval - floor(xval));
					else if (i == 0)
						xvor = voronoi[valx + i][valy + j][0] - (xval - floor(xval));
					else
						xvor = 1 - (xval - floor(xval)) + voronoi[valx + i][valy + j][0];
					if (j == -1)
						yvor = 1 - voronoi[valx + i][valy + j][1] + (yval - floor(yval));
					else if (j == 0)
						yvor = voronoi[valx + i][valy + j][1] - (yval - floor(yval));
					else
						yvor = 1 - (yval - floor(yval)) + voronoi[valx + i][valy + j][1];

					tempdis = sqrt(pow(xvor, 2) +
						pow(yvor, 2));
					if (tempdis < distance) {
						distance = tempdis;
						vorx = valx + i;
						vory = valy + j;
					}
				}
			}
			// different display modes just for fun
			switch (mode) {
			case 0:
				pix[point + 1] = (1 - distance) * vorsections[vorx][vory][2];
				break;
			case 1:
				pix[point + 1] = (1 - distance);
				break;
			case 2:
				pix[point + 1] = vorsections[vorx][vory][2];
				break;
			case 3:
				pix[point + 1] = (distance);
				break;
			}

		}
	}

	//  Select texture unit
	glActiveTexture(unit);
	//  Generate 2D texture and make current
	unsigned int tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	//  Copy noise to texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, pix);

	//  Set texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	

	//  Select default texture unit
	glActiveTexture(GL_TEXTURE0);
	return tex;
}

// refresh display
void display(GLFWwindow* window) {
	// erase the window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z-buffering
	glEnable(GL_DEPTH_TEST);
	// set view
	View(th, ph, fov, dim);
	// enable lighting
	Lighting(3 * Cos(zh), 1.5 + lh, 3 * Sin(zh), 0.3, 0.5, 0.8);

	// select shader
	glUseProgram(shader);
	// pass texture
	int id = glGetUniformLocation(shader, "Noise3D");
	glUniform1i(id, 1);
	// draw scene
	SolidCube();
	// revert to fixed pipeline
	glUseProgram(0);
	glDisable(GL_LIGHTING);

	// display parameters
	SetColor(1, 1, 1);
	glWindowPos2i(5, 5);
	Print("mode = %d", mode);
	// render scene
	ErrCheck("display");
	glFlush();
	glfwSwapBuffers(window);
}

// key pressed
void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// discard key releases
	if (action == GLFW_RELEASE) return;

	// exit on ESC
	switch (key) {
		// quit
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		break;
		// reset view
	case GLFW_KEY_0:
		th = ph = 0;
		break;
		// increase/descrease asimuth
	case GLFW_KEY_RIGHT:
		th += 5;
		break;
	case GLFW_KEY_LEFT:
		th -= 5;
		break;
	case GLFW_KEY_UP:
		ph += 5;
		break;
	case GLFW_KEY_DOWN:
		ph -= 5;
		break;
	case GLFW_KEY_PAGE_DOWN:
		dim += 0.1;
		break;
	case GLFW_KEY_PAGE_UP:
		dim = dim > 1 ? dim - 0.1 : dim;
		break;
	case GLFW_KEY_KP_ADD:
	case GLFW_KEY_EQUAL:
		lh += 0.1;
		break;
	case GLFW_KEY_KP_SUBTRACT:
	case GLFW_KEY_MINUS:
		lh -= 0.1;
		break;
	case GLFW_KEY_1:
		mode += 1;
		mode %= 4;
		myNoiseTex(GL_TEXTURE1);
		break;
	}

	// wrap angles
	th %= 360;
	ph %= 360;
	// update projection
	Projection(fov, asp, dim);
}

// window resize callback
void reshape(GLFWwindow* window, int width, int height) {
	// get framebuffer dimensions
	glfwGetFramebufferSize(window, &width, &height);
	// aspect ratio
	asp = (height > 0) ? (double)width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, width, height);
	// set projection
	Projection(fov, asp, dim);
}

// main program
int main(int argc, char* argv[]) {
	// initialize GLFW
	GLFWwindow* window = InitWindow("Kelley Kelley HW07: Stored Textures", 1, 600, 600, &reshape, &key);
	//  Load random texture
	myNoiseTex(GL_TEXTURE1);
	shader = CreateShaderProg("hw07.vert", "hw07.frag");
	// event loop
	ErrCheck("init");
	while (!glfwWindowShouldClose(window)) {
		// light animation
		zh = fmod(90 * glfwGetTime(), 360);
		// display
		display(window);
		// process any events
		glfwPollEvents();
	}
	// shut down GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}