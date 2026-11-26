#include "RasterisedRenderer.h"
#include "utilityFunctions.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <map>
#include<numbers>

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "ColourSpectrumRenderer.h"
#include "../libs/sdw/CanvasTriangle.h"
#include "../libs/sdw/Utils.h"
#include <Colour.h>

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/ModelTriangle.h"

//comment  for new pR 

void RasterisedRenderer::draw(DrawingWindow &window) {
	window.clearPixels(); //--> this empties the screen.
	glm::mat3 rotationMatrix = utilityFunctions::panMatrix(0.01);
	if (orbit == true) {
		//lookAt = true;
		glm::vec3 centreOfScene(0.0,0.0,0.0);
		cameraPosition = utilityFunctions::orbit(cameraPosition, centreOfScene,rotationMatrix);
		//if (lookAt == true) {
			cameraOrientation = utilityFunctions::lookAt(cameraPosition,centreOfScene,cameraOrientation);
		//}
	}

	// Core 3D rendering data structures (for when we eventually get around to working in 3D ;o)
	zDepthBuffer=utilityFunctions::clearDepthBuffer(zDepthBuffer);

	std::vector<CanvasTriangle> wireFrame;
	std::vector<Colour> triangleColours; // colour of each canvas triangle
	//std::vector<CanvasPoint> pointCloud;

	for (auto&&modelTriangle : triangles) {
		std::vector<CanvasPoint> triangleVertices;
		for (auto&&vertex : modelTriangle.vertices) {
			CanvasPoint point = projectVertexOntoCanvasPoint(cameraPosition,cameraOrientation,focalLength,vertex,window);
			triangleVertices.push_back(point);
			//pointCloud.push_back(point);
		}

		// checks if triangle is on screen.
		CanvasPoint v0 = triangleVertices[0];
		CanvasPoint v1 = triangleVertices[1];
		CanvasPoint v2 = triangleVertices[2];

		if (utilityFunctions::triangleOffScreen(v0,v1,v2)) continue;
		CanvasTriangle canvasTriangle = CanvasTriangle(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
		wireFrame.push_back(canvasTriangle);
		triangleColours.push_back(modelTriangle.colour);
	}

	// uint32_t colour = (255 << 24) + (0 << 16) + (255 << 8) + 0; // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)
	int triangleColourIndex = 0;

	for (auto&&triangle : wireFrame) {
		if (drawWireframe) { drawStrokedTriangles(triangle, window, Colour(255,255,255), zDepthBuffer); } // if 1 draws wireframe
		else { drawFilledTriangles(triangle, window, triangleColours[triangleColourIndex], zDepthBuffer);} // if 2 draws filled.
		triangleColourIndex++;
	}
}

void RasterisedRenderer::incrementTriangleInd(int increment) {
	triangleInd += increment;
}
void RasterisedRenderer::resetTriangleInd() {
	triangleInd = 0;
}

void RasterisedRenderer::setDrawWireframe(bool isItWireframe) {
	drawWireframe = isItWireframe;
}

CanvasPoint RasterisedRenderer::projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation, float focalLength, glm::vec3 vertexPosition, DrawingWindow &window) {

	CanvasPoint twoDPoint;


	 // set twoDPoint to out of bound value for just orbit because when --> got rid off because o toggle activates orbit and lookAt. 
	 // orbiting: if z is between these values  model not in frame and u and v become massive.
	  // if (!lookAt && cameraPosition.z>0.0 && cameraPosition.z<1.0) {
	  // 	return CanvasPoint(-1,-1);}
	  // if (!lookAt && cameraPosition.z<0.0) {
	  // 	return CanvasPoint(-1,-1);
	  // }

	// convert from model coordinate system to camera coordinate system. --> camera is origin.
	// model vertices relative to the centre of the room
	float imagePlaneScaling = 160.0;
	glm::vec3 posRelativeToCamera =  vertexPosition - cameraPosition;
	glm::vec3 adjustedVector = posRelativeToCamera * cameraOrientation;
	float inverseZ = adjustedVector.z;

	// points with negative coordinates are meant to be projected onto the right of the plane.
	// because 0,0 starts in middle of model when it attempts to project right side adjusted to camera it gets skewed weird?
	// they are plotted outside the image plane --> so subtract from window width
	//positive points are supposted to be on the left of image plane. negative on the right.

	float u = window.width/2 + (focalLength * (adjustedVector.x / adjustedVector.z))*imagePlaneScaling;
	float v = window.height/2 - (focalLength * (adjustedVector.y/ adjustedVector.z))*imagePlaneScaling;

	twoDPoint = CanvasPoint(round(u),round(v));
	if (inverseZ < 0.0) twoDPoint.depth = abs(1.0f/inverseZ) ;
	else {twoDPoint.depth = 1.0f/inverseZ;}


	return twoDPoint;
}

void RasterisedRenderer::lineDrawing(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine, std::vector<float> &zDepthBuffer) {

	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float depthDiff = to.depth - from.depth;

	// Colour WHITE = Colour(255,255,255);
	uint32_t colour = (255 << 24) + (colourOfLine.red << 16) + (colourOfLine.green << 8) + colourOfLine.blue; // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)

	float numberOfSteps = fmax(abs(xDiff), abs(yDiff));

	// number of steps can be massive if xdiff and ydiff are massive because when projecting the smaller ur z is the bigger ur u and v are (x and y)
	// therefore, small z , means massively farway point/step size means AHHH BUFFER
	//what would a suitable step size be??? infinite is not.

	if (numberOfSteps>0 && numberOfSteps < 5000.0f) {// stops drawing if number of steps is infinite
		// always using the bigger number of steps ensures we don't skip pixels --> how does that ensure this?
		float xStepSize = xDiff / numberOfSteps;
		// work out step size of x or y by dividing total distance by number of steps take
		// (i.e. the n.o intervals --> similar to interpolation)
		float yStepSize = yDiff / numberOfSteps;

		float depStepSize = depthDiff / numberOfSteps;

		for (int i=0; i <= numberOfSteps; i++) {
			float x = from.x + (xStepSize * i);
			float y = from.y + (yStepSize * i);

			int xPixel = round(x);
			int yPixel = round(y);

			if (xPixel>=0 && yPixel>= 0 && xPixel < window.width && yPixel < window.height) {
				// restrict the drawing space//
				float currentZDepth = abs(from.depth+(depStepSize*i));
				int z = yPixel * window.width + xPixel;
				//std::cout<<"origin:" << currentZDepth <<std::endl;

				if (currentZDepth > zDepthBuffer[z]) {
					//std::cout <<"X: "<< x <<"Y: "<< y<< "z: "<< currentZDepth <<std::endl;
					window.setPixelColour(xPixel, yPixel,  colour ); // can't have 1.5 pixel therefore need to round the x and y.
						zDepthBuffer[z] = currentZDepth; //update zdepth buffer
					}
				}

		}
	}
}

void RasterisedRenderer::drawStrokes(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine) {
	int xDiff = to.x - from.x;
	int yDiff = to.y - from.y;

	// Colour WHITE = Colour(255,255,255);
	uint32_t colour = (255 << 24) + (colourOfLine.red << 16) + (colourOfLine.green << 8) + colourOfLine.blue; // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)

	float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
	// always using the bigger number of steps ensures we don't skip pixels --> how does that ensure this?
	float xStepSize = xDiff / numberOfSteps;
	// work out step size of x or y by dividing total distance by number of steps take
	// (i.e. the n.o intervals --> similar to interpolation)
	float yStepSize = yDiff / numberOfSteps;


	if (numberOfSteps>0) {
		for (int i=0; i <= numberOfSteps; i++) {
			float x = from.x + (xStepSize * i);
			float y = from.y + (yStepSize * i);

			if (x>0 && y> 0 && x < window.width && y < window.height) { // restrict the drawing space//
				window.setPixelColour(floor(x), floor(y), colour ); // can't have 1.5 pixel therefore need to round the x and y.
			}
		}
	}
}

void RasterisedRenderer::drawStrokedTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float> &zDepthBuffer) {
	lineDrawing(triangle.vertices[0], triangle.vertices[1], window, colour, zDepthBuffer);
	lineDrawing(triangle.vertices[0], triangle.vertices[2], window, colour, zDepthBuffer);
	lineDrawing(triangle.vertices[1], triangle.vertices[2], window, colour,zDepthBuffer);
}

std::vector<CanvasPoint> RasterisedRenderer::sortCoordsByY(std::vector<CanvasPoint>  unsortedVertices) {

	int n = unsortedVertices.size();

	// Outer loop that corresponds to the number of elements to be sorted
	for (int i = 0; i < n - 1; i++) {
		// Last i elements are already in place
		for (int j = 0; j < n - i - 1; j++) {

			// Comparing adjacent elements
			if (unsortedVertices[j].y > unsortedVertices[j + 1].y)

				// Swapping if in the wrong order
					std::swap(unsortedVertices[j], unsortedVertices[j + 1]);
		}
	}
	return unsortedVertices;
}

float RasterisedRenderer::calculateGradient(CanvasPoint p1, CanvasPoint p2) {
	float yDiff = p2.y - p1.y;
	float xDiff = p2.x - p1.x;
	float gradient;

	if (xDiff == 0) gradient = INFINITY;
	else if (yDiff == 0) gradient = 0;
	else gradient = yDiff/xDiff;

	return gradient;
}

float RasterisedRenderer::calculateZGradient(CanvasPoint p1, CanvasPoint p2) {
	float yDiff = p2.y - p1.y;
	float zDiff = p2.depth - p1.depth;
	float gradient;

	if (zDiff == 0) gradient = INFINITY;
	else if (yDiff == 0) gradient = 0;
	else gradient = yDiff/zDiff;

	return gradient;
}

std::vector<CanvasPoint> RasterisedRenderer::obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, int pointCount) {

	float gradient = calculateGradient(firstPoint, secondPoint);
	float gradientInverse = 0;
	std::vector<float> xPoints =utilityFunctions::interpolateSingleFloats(firstPoint.x, secondPoint.x, pointCount);

	if (gradient != 0) {gradientInverse = 1/gradient;} // x/y to get 1/m (y=mx+c where c = 0 )

	std::vector<float> yPoints = utilityFunctions::interpolateSingleFloats(firstPoint.y, secondPoint.y, pointCount);

	std::vector<float> zDepths = utilityFunctions::interpolateSingleFloats(firstPoint.depth, secondPoint.depth, pointCount);

	std::vector<CanvasPoint> points;

	for (int i = 0; i < pointCount; i++) {
		float xPoint;
		if (gradientInverse != 0) { xPoint = firstPoint.x+((yPoints[i]-firstPoint.y) * gradientInverse);}
		else xPoint = xPoints[i]; // IT WORKS :D
		CanvasPoint newPoint = CanvasPoint(xPoint,yPoints[i], zDepths[i]);
		points.push_back(newPoint);
	}
	return points;
}

void RasterisedRenderer::drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float> &zDepthBuffer) {

	std::vector<CanvasPoint> verticiesList;

	for (int i =0; i <3; i++) {
		verticiesList.push_back(triangle.vertices[i]);
	}
	verticiesList = sortCoordsByY(verticiesList);

	float gradient = calculateGradient(verticiesList[0], verticiesList[2]);
	float gradientInverse;
	if (gradient != 0) {gradientInverse = 1/gradient;} // x/y to get 1/m (y=mx+c where c = 0 )
	else {gradientInverse = 1;}

	// if y = 0 function has slope 1/z
	float zGradient = calculateZGradient(verticiesList[0], verticiesList[2]);
	float extraPointZ;

	 // if zDiff is 0 zGradient is 0.

	//we have y dist between the middle point and top point by yCoords[1] - yCoords[0]
	//we can work out the x difference by multiplying the y difference by the gradient inverse
	// midYdiff*xdiff/ydiff= midXdiff.
	// add midXdiff to the xCoords[0] -> if midXdiff <0  xCoords[0] +- midXdiff subtracts it.
	// if its >0 then it adds as appropriate :) LORD WHY WAS THAT SO HARD???

	float extraPointX = verticiesList[0].x+((verticiesList[1].y-verticiesList[0].y) * gradientInverse); // IT WORKS :D
	std::vector<float> zDepths = utilityFunctions::interpolateSingleFloats(verticiesList[0].depth, verticiesList[1].depth, floor(abs(extraPointX - verticiesList[1].x)));
	if (zGradient != 0) {
		zGradient = 1/zGradient;
		extraPointZ = verticiesList[0].depth+((verticiesList[1].y-verticiesList[0].y) * zGradient);
	} else {
		extraPointZ = zDepths[zDepths.size()-1];
	}
	CanvasPoint extraPoint = CanvasPoint((extraPointX),verticiesList[1].y, extraPointZ);

	int numberOfRakes = verticiesList[1].y-verticiesList[0].y + 1;
	// shouldn't be the same number of points on both sides :/ --> because the lines have different gradients
	// due to rounding if you don't include an extra rake line then based on how the floats are rounded you could skip a line.
	// as a result the extra line could 'overlap' with a point that you think that has colour but it doesn't.
	// also starting point count list at 0 because then we are guaranteed to have a line at the top of the triangle.
	// ALSO for bottom rakes, the pointy[0] contains the middle line lol. top function doesn't colour that in !!!
	std::vector<CanvasPoint> firstLinePoints = obtainPoints(verticiesList[0],verticiesList[1], numberOfRakes);
	std::vector<CanvasPoint> secondLinePoints = obtainPoints(verticiesList[0],extraPoint, numberOfRakes);

	int bottomRakes =  verticiesList[2].y-verticiesList[1].y +1;
	std::vector<CanvasPoint> downLinePoints = obtainPoints(verticiesList[1],verticiesList[2], bottomRakes);
	std::vector<CanvasPoint> secondDownLinePoints = obtainPoints(extraPoint,verticiesList[2], bottomRakes);

	//top triangle
	for (int  y = verticiesList[0].y; y < verticiesList[1].y; y++) {
		for (int x = 0; x < numberOfRakes; x++) {
			lineDrawing(firstLinePoints[x], secondLinePoints[x],window,colour, zDepthBuffer);
		}
	}

	// bottom triangle
	for (int  y = verticiesList[1].y; y < verticiesList[2].y; y++) {
		for (int x = 0; x < bottomRakes; x++) {
			lineDrawing(downLinePoints[x], secondDownLinePoints[x],window,colour,zDepthBuffer);
		}
	}
	lineDrawing(triangle.vertices[0], triangle.vertices[1], window, colour, zDepthBuffer);
	lineDrawing(triangle.vertices[0], triangle.vertices[2], window, colour, zDepthBuffer);
	lineDrawing(triangle.vertices[1], triangle.vertices[2], window, colour, zDepthBuffer);
}

