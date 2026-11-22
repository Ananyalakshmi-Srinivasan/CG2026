#pragma once

#include "Renderer.h"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include <Colour.h>
#include <map>
#include "../libs/sdw/RayTriangleIntersection.h"


#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


class RayTracedRenderer: public Renderer {
   public:
      glm::vec3& cameraPosition;
      const glm::vec3& lightPosition;
      glm::mat3& cameraOrientation;
      const float& focalLength;
      const std::map<std::string, Colour>& colourPalette;
      const std::vector<ModelTriangle>& triangles;

      bool orbit;
      bool lookAt;

      bool hardShadows;
      // const double PI = 3.141592653589793;

      void draw(DrawingWindow &window) override;
      RayTriangleIntersection getClosestIntersection(glm::vec3 rayDirection, glm::vec3 cameraPosition);
      Colour castHardShadows(glm::vec3 lightPosition, RayTriangleIntersection intersection);
      std::vector<RayTriangleIntersection> sortPointsByT(std::vector<RayTriangleIntersection>  unsortedIntersections);
      glm::vec3 transposePlaneCoordsToRay(glm::vec3 planePoint, float scaleFactor );
      //glm::vec3 RayTracedRenderer::getRayToLight(glm::vec3 modelPoint, float scaleFactor);

    // constructor of raytraced renderer
    RayTracedRenderer(glm::vec3 &cameraPosition_, glm::vec3 &lightPosition_, glm::mat3 &cameraOrientation_, float &focalLength_,
                       const std::map<std::string, Colour> &colourPalette_, const std::vector<ModelTriangle> &triangles_) :
                       cameraPosition(cameraPosition_), lightPosition(lightPosition_), cameraOrientation(cameraOrientation_),
                       focalLength(focalLength_), colourPalette(colourPalette_),triangles(triangles_), orbit(false), lookAt(false) {}

};


