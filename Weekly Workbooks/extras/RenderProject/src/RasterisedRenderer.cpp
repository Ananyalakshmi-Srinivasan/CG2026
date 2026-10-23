#include "RasterisedRenderer.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "ColourSpectrumRenderer.h"
#include "../libs/sdw/CanvasTriangle.h"

class DrawingWindow;

void RasterisedRenderer::draw(DrawingWindow &window) {
	// window.clearPixels(); --> this empies the screen.

	// CanvasPoint canvasPointFromOne= CanvasPoint(0, 0);
	// CanvasPoint canvasPointToOne= CanvasPoint(window.width/2, window.height/2);
	//
	// Colour WHITE = Colour(255,255,255);
	// lineDrawing(canvasPointFromOne,canvasPointToOne, window, WHITE);

}

void RasterisedRenderer::lineDrawing(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine) {

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

	std::cout << numberOfSteps << std::endl;
	for (int i=0; i <= numberOfSteps; i++) {
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);

		if (x>0 && y> 0 && x < window.width && y < window.height) { // restrict the drawing space//
			// std::cout << round(x) << " " << round(y) <<  " " << std::endl;
			window.setPixelColour(round(x), round(y), colour );
			// can't have 1.5 pixel therefore need to round the x and y.
		}
	}
}

void RasterisedRenderer::drawStrokedTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour) {
	lineDrawing(triangle.vertices[0], triangle.vertices[1], window, colour);
	//printf("%f",triangle.vertices[0].x);
	lineDrawing(triangle.vertices[0], triangle.vertices[2], window, colour);
	lineDrawing(triangle.vertices[1], triangle.vertices[2], window, colour);
}

std::vector<CanvasPoint> RasterisedRenderer::sortCoordsByY(std::vector<CanvasPoint>  unsortedVeritcies) {

	int n = unsortedVeritcies.size();

	// Outer loop that corresponds to the number of elements to be sorted
	for (int i = 0; i < n - 1; i++) {
		// Last i elements are already in place
		for (int j = 0; j < n - i - 1; j++) {

			// Comparing adjacent elements
			if (unsortedVeritcies[j].y > unsortedVeritcies[j + 1].y)

				// Swapping if in the wrong order
					std::swap(unsortedVeritcies[j], unsortedVeritcies[j + 1]);
		}
	}
	return unsortedVeritcies;
}

float RasterisedRenderer::calculateGradient(CanvasPoint p1, CanvasPoint p2) {
	float yDiff = p2.y - p1.y;
	float xDiff = p2.x - p1.x;
	float gradient = yDiff/xDiff;

	return gradient;
}

std::vector<CanvasPoint> RasterisedRenderer::obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, int pointCount) {


	float gradientInverse = 1/calculateGradient(firstPoint, secondPoint);

	ColourSpectrumRenderer interpolationFunctions;
	std::vector<float> yPoints = interpolationFunctions.interpolateSingleFloats(firstPoint.y, secondPoint.y, float(pointCount));

	std::vector<CanvasPoint> points;

	for (int i = 0; i < pointCount; i++) {
		float xPoint = firstPoint.x+((yPoints[i]-firstPoint.y) * gradientInverse); // IT WORKS :D
		points.push_back(CanvasPoint(xPoint,yPoints[i]));
	}
	return points;

}

void RasterisedRenderer::drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour) {
	std::cout << triangle.vertices[0] << " " << std::endl;
	std::cout << triangle.vertices[1] << " " << std::endl;
	std::cout << triangle.vertices[2] << " " << std::endl;

	lineDrawing(triangle.vertices[0], triangle.vertices[1], window, colour);
	lineDrawing(triangle.vertices[0], triangle.vertices[2], window, colour);
	lineDrawing(triangle.vertices[1], triangle.vertices[2], window, colour);

	std::vector<CanvasPoint> verticiesList;

	for (int i =0; i <3; i++) {
		verticiesList.push_back(triangle.vertices[i]);
	}
	verticiesList = sortCoordsByY(verticiesList);

	float gradientInverse = 1/calculateGradient(verticiesList[0], verticiesList[2]); // x/y to get 1/m (y=mx+c where c = 0 )
	//we have y dist between the middle point and top point by yCoords[1] - yCoords[0]
	//we can work out the x difference by multiplying the y difference by the gradient inverse
	// midYdiff*xdiff/ydiff= midXdiff.
	// add midXdiff to the xCoords[0] -> if midXdiff <0  xCoords[0] +- midXdiff subtracts it.
	// if its >0 then it adds as appropriate :) LORD WHY WAS THAT SO HARD???

	float extraPointX = verticiesList[0].x+((verticiesList[1].y-verticiesList[0].y) * gradientInverse); // IT WORKS :D
	CanvasPoint extraPoint = CanvasPoint(round(extraPointX),verticiesList[1].y);

	int numberOfRakes =  verticiesList[1].y-verticiesList[0].y + 1;
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
			std::cout << firstLinePoints[x] << secondLinePoints[x] << " " << std::endl;
			lineDrawing(firstLinePoints[x], secondLinePoints[x],window,colour);
		}
	}

	// bottom triangle
	for (int  y = verticiesList[1].y; y < verticiesList[2].y; y++) {
		for (int x = 0; x < bottomRakes; x++) {
			std::cout << downLinePoints[x] << secondDownLinePoints[x] << " bottom " << std::endl;
			lineDrawing(downLinePoints[x], secondDownLinePoints[x],window,colour);
		}
	}
}

	// texture map later. --> just need theory for midterm!!!!!! -_- doing easier way after reading week.
