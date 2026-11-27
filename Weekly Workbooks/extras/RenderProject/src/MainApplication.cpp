#include "Renderer.h"
#include "RasterisedRenderer.h"
#include "utilityFunctions.h"
#include "RayTracedRenderer.h"
#include <Colour.h>

#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp> // allows you to output vec3 in standard output.
#include <filesystem>

#include "../libs/sdw/CanvasTriangle.h"

struct Colour;
// Define globals for WIDTH and HEIGHT of the window (can be accessed from any renderer)
extern const int WIDTH = 320;
extern const int HEIGHT = 240;

extern const float PI = 3.141592653589793;

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT);
// RedNoiseRenderer redNoise = RedNoiseRenderer();
// ColourSpectrumRenderer colourSpectrumRenderer = ColourSpectrumRenderer();

bool savingFrames = false;
int frameCounter = 0;

// // Core 3D rendering data structures (for when we eventually get around to working in 3D ;o)
std::vector<ModelTriangle> triangles; // week 4
std::map<std::string, Colour> colourPalette;
glm::vec3 cameraPosition(0.0, 0.0, 4.0); // camera centered in the x and  y dimension but stepped a 'back a bit' in the z
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 right(1.0f, 0.0f, 0.0f);
glm::vec3 forward(0.0f, 0.0f, -1.0f);
glm::mat3 cameraOrientation(right, up, forward);

glm::vec3 lightPosition(0.0, 0.8, 0.0);
float focalLength = 2.0; // setting the focal length at 2.0 positions the image plane halfway between the camera and the centre of the model.
std::vector <float> zDepthBuffer(WIDTH*HEIGHT, 0.0);
int triangleInd = 0;

//Renderers
RasterisedRenderer rasterisedRenderer = RasterisedRenderer(cameraPosition, lightPosition, cameraOrientation, focalLength, colourPalette, triangles, zDepthBuffer);
RayTracedRenderer rayTracedRenderer = RayTracedRenderer(cameraPosition, lightPosition, cameraOrientation, focalLength, colourPalette, triangles);
Renderer* currentRenderer = &rayTracedRenderer; // pointer to current renderer


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {
			glm::vec3 translationVector(-1.0, 0.0, 0.0);
			cameraPosition = utilityFunctions::translateCameraPosition(cameraPosition, translationVector);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			glm::vec3 translationVector(1.0, 0.0, 0.0);
			cameraPosition = utilityFunctions::translateCameraPosition(cameraPosition, translationVector);
		}
		else if (event.key.keysym.sym == SDLK_UP) {
			glm::vec3 translationVector(0.0, 0.0, 1.0);
			cameraPosition = utilityFunctions::translateCameraPosition(cameraPosition, translationVector);
		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			glm::vec3 translationVector(0.0, 0.0, -1.0);
		    cameraPosition = utilityFunctions::translateCameraPosition(cameraPosition, translationVector);
		}
		else if (event.key.keysym.sym == SDLK_o) {
			rasterisedRenderer.orbit = !rasterisedRenderer.orbit;
			rayTracedRenderer.orbit = !rayTracedRenderer.orbit;
		}
		else if (event.key.keysym.sym == SDLK_r) {
			rayTracedRenderer.lookAt = !rayTracedRenderer.lookAt;
		}
		else if (event.key.keysym.sym == SDLK_f) { // allows you to draw one triangle at a time at key press.
			// if (triangleInd < triangles.size()) { // wireframe size not necessarily size of obj. --> address this.
			// 	rasterisedRenderer.incrementTriangleInd(1);
			// } else { // resets triangle counter to 0.
			// 	triangleInd = 0;
			// 	rasterisedRenderer.resetTriangleInd();
			// }
		} else if (event.key.keysym.sym == SDLK_h) {
			rayTracedRenderer.hardShadows = !rayTracedRenderer.hardShadows;
		}
		else if (event.key.keysym.sym == SDLK_w) {
			lightPosition += glm::vec3(0.0, 0.1, 0.0);
		} else if (event.key.keysym.sym == SDLK_a) {
			lightPosition += glm::vec3(-0.1, 0.0, 0.0);
		} else if (event.key.keysym.sym == SDLK_s) {
			lightPosition += glm::vec3(0.0, -0.1, 0.0);
		} else if (event.key.keysym.sym == SDLK_d) {
			lightPosition += glm::vec3(0.1, 0.0, 0.0);
		} else if (event.key.keysym.sym == SDLK_b) {
			lightPosition += glm::vec3(0.0, 0.0, -0.1);
		}else if (event.key.keysym.sym == SDLK_z) {
			lightPosition += glm::vec3(0.0, 0.0, 0.1);
		}

		else if (event.key.keysym.sym == SDLK_1) {
			currentRenderer = &rasterisedRenderer; // switches renderer to rasterised.
			rasterisedRenderer.setDrawWireframe(true); // sets draw wureframe boolean to true.
		}

		else if (event.key.keysym.sym == SDLK_2) {
			currentRenderer = &rasterisedRenderer;
			rasterisedRenderer.setDrawWireframe(false);
		}
		else if (event.key.keysym.sym == SDLK_3) {
			currentRenderer = &rayTracedRenderer;
		}
		else if (event.key.keysym.sym == SDLK_RETURN) window.toggleFullscreen();

		else if (event.key.keysym.sym == SDLK_TAB) {
			savingFrames = !savingFrames;
			std::cout << savingFrames << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_RETURN) window.toggleFullscreen();
	}
}

int main(int argc, char *argv[]) {
	SDL_Event event;
	// if i run with make file (i.e. terminal) relative path for these  won't lead to a render?
	std::string matFile = "../src/models/cornellBox/cornell-box.mtl";
	std::string objFile = "../src/models/cornellBox/cornell-box.obj";
	std::string sphereObj = "../src/models/sphere.obj";
	std::string sphereMat = "../src/models/sphere.mtl";


	float scaleFactor = 0.35;

	//utilityFunctions::loadModel(objFile, matFile, triangles, colourPalette, scaleFactor);
	utilityFunctions::loadModel(sphereObj, sphereMat, triangles, colourPalette, 0.5);

	// colourPalette = utilityFunctions::readMatFile(matFile, colourPalette);
	// triangles = utilityFunctions::readObjFile(objFile, triangles, scaleFactor, colourPalette);

	while (true) { // forever loop
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		currentRenderer->draw(window);

		/// polling for incoming mouse and keyboard events
		// In C++ we can't use "." notation for dynamic dispatch, so we have to use -> instead :o(
		//currentRenderer->draw(window);/// draws screen with current renderer --> if something already drawn by event handline the window.clearPixels() cleans it.
		// Need to ask the SDL window to render the frame, otherwise nothing actually gets shown on the screen !
		window.renderFrame(); // renders the memory buffers

		if(savingFrames) { /// check if file open fails at draw window. -- fresh build before saving frames.
			std::string paddedCounter = std::to_string(frameCounter);
			while(paddedCounter.length() < 6) paddedCounter = "0" + paddedCounter;
			// Use whichever image file format works best on your platform (comment out the one you don't need)
			window.savePPM("../saved-frames", paddedCounter + ".ppm");
			std::cout << std::__fs::filesystem::current_path() << std::endl;
			// window.saveBMP("../saved-frames", paddedCounter + ".bmp");
			frameCounter++;
		}
	}
}
