#pragma once

#include "Renderer.h"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include <Colour.h>


class RasterisedRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      void lineDrawing(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine);
      void drawStrokedTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour);
      void drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour);
      std::vector<CanvasPoint> sortCoordsByY(std::vector<CanvasPoint>  unsortedVeritcies);
      std::vector<CanvasPoint> obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, float pointCount);

};


