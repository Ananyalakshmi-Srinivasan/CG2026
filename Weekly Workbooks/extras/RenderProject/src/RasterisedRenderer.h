#pragma once

#include "Renderer.h"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include <Colour.h>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


class RasterisedRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      void lineDrawing(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine, std::vector<float > &zDepthBuffer);
      void drawStrokedTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float > &zDepthBuffer);
      void drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float> &zDepthBuffer);
      std::vector<CanvasPoint> sortCoordsByY(std::vector<CanvasPoint>  unsortedVeritcies);
      std::vector<CanvasPoint> obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, int pointCount);
      std::vector<float> obtainZDepths(CanvasPoint firstPoint, CanvasPoint secondPoint, int pointCount);
      float calculateGradient(CanvasPoint p1, CanvasPoint p2);
      CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, float focalLength, glm::vec3 vertexPosition, DrawingWindow &window);
};


