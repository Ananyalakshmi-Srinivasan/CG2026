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

	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;

	// Colour WHITE = Colour(255,255,255);
	uint32_t colour = (255 << 24) + (colourOfLine.red << 16) + (colourOfLine.green << 8) + colourOfLine.blue; // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)

	float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
	// always using the bigger number of steps ensures we don't skip pixels --> how does that ensure this?
	float xStepSize = xDiff / numberOfSteps;
	// work out step size of x or y by dividing total distance by number of steps take
	// (i.e. the n.o intervals --> similar to interpolation)
	float yStepSize = yDiff / numberOfSteps;

	for (float i=0.0; i <= numberOfSteps; i++) {
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);

		if (x>0 && y> 0 && x < window.width && y < window.height) { // restrict the drawing space//
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

std::vector<CanvasPoint> RasterisedRenderer::obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, float pointCount) {

	float bottomTopDiffY = secondPoint.y - firstPoint.y;
	float bottomTopDiffX = secondPoint.x - firstPoint.x;
	float gradientInverse = bottomTopDiffX/bottomTopDiffY;

	ColourSpectrumRenderer interpolationFunctions;
	std::vector<float> yPoints = interpolationFunctions.interpolateSingleFloats(firstPoint.y, secondPoint.y, pointCount);

	std::vector<CanvasPoint> points;

	for (float i = 1; i < pointCount; i++) {

		float xPoint = firstPoint.x+((yPoints[i]-firstPoint.y) * gradientInverse); // IT WORKS :D
		points.push_back(CanvasPoint(xPoint,yPoints[i]));
	}
	return points;

}

void RasterisedRenderer::drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour) {
	lineDrawing(triangle.vertices[0], triangle.vertices[1], window, colour);
	//printf("%f",triangle.vertices[0].x);
	lineDrawing(triangle.vertices[0], triangle.vertices[2], window, colour);
	lineDrawing(triangle.vertices[1], triangle.vertices[2], window, colour);

	std::vector<CanvasPoint> verticiesList;

	for (int i =0; i <3; i++) {
		verticiesList.push_back(triangle.vertices[i]);
	}
	verticiesList = sortCoordsByY(verticiesList);

	float bottomTopDiffY = verticiesList[2].y - verticiesList[0].y;
	float bottomTopDiffX = verticiesList[2].x - verticiesList[0].x;
	float gradientInverse = bottomTopDiffX/bottomTopDiffY; // x/y to get 1/m (y=mx+c where c = 0 )
		//we have y dist between the middle point and top point by yCoords[1] - yCoords[0]
		//we can work out the x difference by multiplying the y difference by the gradient inverse
		// midYdiff*xdiff/ydiff= midXdiff.
		// add midXdiff to the xCoords[0] -> if midXdiff <0  xCoords[0] +- midXdiff subtracts it.
		// if its >0 then it adds as appropriate :) LORD WHY WAS THAT SO HARD???

	float extraPointX = verticiesList[0].x+((verticiesList[1].y-verticiesList[0].y) * gradientInverse); // IT WORKS :D
	CanvasPoint extraPoint = CanvasPoint(extraPointX,verticiesList[1].y);

	int numberOfRakes =  verticiesList[1].y-verticiesList[0].y;
	std::vector<CanvasPoint> firstLinePoints = obtainPoints(verticiesList[0],verticiesList[1], numberOfRakes);
	std::vector<CanvasPoint> secondLinePoints = obtainPoints(verticiesList[0],extraPoint, numberOfRakes);

	int bottomRakes =  verticiesList[2].y-verticiesList[1].y;
	std::vector<CanvasPoint> downLinePoints = obtainPoints(verticiesList[1],verticiesList[2], bottomRakes);
	std::vector<CanvasPoint> secondDownLinePoints = obtainPoints(extraPoint,verticiesList[2], bottomRakes);

	std::cout << numberOfRakes << " " << bottomRakes  << " " << numberOfRakes+bottomRakes << " " << verticiesList[2].y-verticiesList[0].y << " ";
	//top triangle
	for (int  y = verticiesList[0].y; y < verticiesList[1].y; y++) {
		for (int x = 0; x < round(numberOfRakes); x++) {
			lineDrawing(firstLinePoints[x], secondLinePoints[x],window,colour);
		}
	}
	// // bottom triangle
	// for (int  y = verticiesList[1].y; y < verticiesList[2].y; y++) {
	// 	for (int x = 0; x < bottomRakes; x++) {
	// 		lineDrawing(downLinePoints[x], secondDownLinePoints[x],window,colour);
	// 	}
	// }
}
