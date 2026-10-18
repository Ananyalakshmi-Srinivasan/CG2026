#include "RasterisedRenderer.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <vector>

#include "../libs/sdw/CanvasTriangle.h"

class DrawingWindow;

void RasterisedRenderer::draw(DrawingWindow &window) {
	// window.clearPixels(); --> this empies the screen.

	CanvasPoint canvasPointFromOne= CanvasPoint(0, 0);
	CanvasPoint canvasPointToOne= CanvasPoint(window.width/2, window.height/2);

	lineDrawing(canvasPointFromOne,canvasPointToOne, window, Colour(255,255,255));


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

void RasterisedRenderer::drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour) {
	CanvasPoint v1 = triangle.vertices[0];
	CanvasPoint v2 = triangle.vertices[1];
	CanvasPoint v3 = triangle.vertices[2];

	// make lists to store x and y coordinates
	std::vector<float> yCoords;
	std::vector<float> xCoords;

	yCoords.push_back(v1.y);
	yCoords.push_back(v2.y);
	yCoords.push_back(v3.y);
	std::cout << "yb  "<< v1.y << " "  << "x b "<< v2.y << " " <<  v3.y << " " <<  std::endl;

	std::cout << "xb  "<< v1.x << " "  << "x b "<< v2.x << " " <<  v3.x << " " <<  std::endl;


    std::sort(yCoords.begin(), yCoords.end());

	if (v1.y == yCoords[0] && v2.y == yCoords[1] && v3.y == yCoords[2]) {
		xCoords.push_back(v1.x);
		xCoords.push_back(v2.x);
		xCoords.push_back(v3.x);
	} else if (v1.y == yCoords[0] && v2.y == yCoords[2] && v3.y == yCoords[1]) {
		xCoords.push_back(v1.x);
		xCoords.push_back( v3.x);
		xCoords.push_back( v2.x);

	} else if (v1.y == yCoords[1] && v2.y == yCoords[0] && v3.y == yCoords[2]) {
		xCoords.push_back( v2.x);
		xCoords.push_back( v1.x);
		xCoords.push_back( v3.x);
	} else if (v1.y == yCoords[1] && v2.y == yCoords[2] && v3.y == yCoords[0]) {
		xCoords.push_back( v3.x);
		xCoords.push_back( v1.x);
		xCoords.push_back(v2.x);
	} else if (v1.y == yCoords[2] && v2.y == yCoords[0] && v3.y == yCoords[1]) {
		xCoords.push_back(  v2.x);
		xCoords.push_back( v3.x);
		xCoords.push_back(v1.x);
	} else  {
		xCoords.push_back( v3.x);
		xCoords.push_back(v2.x);
		xCoords.push_back( v1.x);
	}


	float bottomTopDiffY = yCoords[2] - yCoords[0];
	float bottomTopDiffX = xCoords[2] - xCoords[0];
	float gradientInverse = bottomTopDiffX/bottomTopDiffY; // x/y to get 1/m (y=mx+c where c = 0 )
		//we have y dist between the middle point and top point by yCoords[1] - yCoords[0]
		//we can work out the x difference by multiplying the y difference by the gradient inverse
		// midYdiff*xdiff/ydiff= midXdiff.
		// add midXdiff to the xCoords[0] -> if midXdiff <0  xCoords[0] +- midXdiff subtracts it.
		// if its >0 then it adds as appropriate :) LORD WHY WAS THAT SO HARD???
	float extraPointX = xCoords[0]+((yCoords[1]-yCoords[0]) * gradientInverse); // IT WORKS :D



	/// split triangles
	/// add to array and sort ascending y, to find 'highest point, middle and lowest point'
	/// draw line from middle point --> work out x using triangles and ratio we have from y diff. :)

}
