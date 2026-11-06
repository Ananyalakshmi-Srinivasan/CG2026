#include "RasterisedProjector.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <map>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "ColourSpectrumRenderer.h"
#include "../libs/sdw/CanvasTriangle.h"
#include "../libs/sdw/Utils.h"
#include <Colour.h>

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/ModelTriangle.h"


class DrawingWindow;

void RasterisedProjector::draw(DrawingWindow &window) {
	//window.clearPixels(); //--> this empies the screen.

	// CanvasPoint canvasPointFromOne= CanvasPoint(0, 0);
	// CanvasPoint canvasPointToOne= CanvasPoint(window.width/2, window.height/2);
	//
	// Colour WHITE = Colour(255,255,255);
	// lineDrawing(canvasPointFromOne,canvasPointToOne, window, WHITE);

}

CanvasPoint RasterisedProjector::projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition, DrawingWindow &window) {

	// convert from model coordinate system to camera coordinate system. --> camera is origin.
	// model. verticies  relative to the centre of the room


	float imagePlaneScaling = 160.0;

	glm::vec3 posRelativeToCamera = vertexPosition - cameraPosition; 

	float u = (focalLength * (posRelativeToCamera.x / posRelativeToCamera.z))*imagePlaneScaling +window.width/2;
	float v = (focalLength * (posRelativeToCamera.y/ posRelativeToCamera.z))*imagePlaneScaling +window.height/2;

	CanvasPoint twoDPoint = CanvasPoint(round(u),round(v));

	return twoDPoint;
}



