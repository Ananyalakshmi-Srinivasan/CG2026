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


	/// split triangles
	/// add to array and sort ascending y, to find 'highest point, middle and lowest point'
	/// draw line from middle point --> work out x using triangles and ratio we have from y diff. :)

}
