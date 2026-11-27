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
      glm::vec3& cameraPosition;
      const glm::vec3& lightPosition;
      glm::mat3& cameraOrientation;
      const float& focalLength;
      const std::map<std::string, Colour>& colourPalette;
      const std::vector<ModelTriangle>& triangles;
      std::vector<float>& zDepthBuffer;
      int triangleInd;
      bool drawWireframe;
      bool orbit;
      bool lookAt;
      const double PI = 3.141592653589793;

      void draw(DrawingWindow &window) override;
      void lineDrawing(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine, std::vector<float > &zDepthBuffer);
      void drawStrokes(CanvasPoint from, CanvasPoint to, DrawingWindow &window, Colour colourOfLine);

      //debugging functions --> used to draw each model triangle one after another.
      void incrementTriangleInd(int increment);
      void setDrawWireframe(bool drawWireframe);

      void resetTriangleInd();

      // glm::vec3 orbit(glm::vec3 cameraPos);

      void drawStrokedTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float > &zDepthBuffer);
      void drawFilledTriangles(CanvasTriangle triangle, DrawingWindow &window, Colour colour, std::vector<float> &zDepthBuffer);
      std::vector<CanvasPoint> sortCoordsByY(std::vector<CanvasPoint>  unsortedVertices);
      std::vector<CanvasPoint> obtainPoints(CanvasPoint firstPoint, CanvasPoint secondPoint, int pointCount);

      float calculateGradient(CanvasPoint p1, CanvasPoint p2);
      float calculateZGradient(CanvasPoint p1, CanvasPoint p2);

      CanvasPoint projectVertexOntoCanvasPoint(glm::vec3 cameraPosition, glm::mat3 cameraOrientation ,float focalLength, glm::vec3 vertexPosition, DrawingWindow &window);

      // constructor of rasterised renderer
      RasterisedRenderer(glm::vec3 &cameraPosition_, glm::vec3 &lightPosition_,  glm::mat3 &cameraOrientation_, float &focalLength_,
                         const std::map<std::string, Colour> &colourPalette_, const std::vector<ModelTriangle> &triangles_, std::vector<float> &zDepthBuffer_) :
                         cameraPosition(cameraPosition_), lightPosition(lightPosition_), cameraOrientation(cameraOrientation_),
                         focalLength(focalLength_), colourPalette(colourPalette_),triangles(triangles_),zDepthBuffer(zDepthBuffer_), orbit(false), lookAt(false) {}
};



