#pragma once

#include "Renderer.h"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include <Colour.h>
#include <map>
// #include "../libs/sdw/RayTriangleIntersection.cpp"
// #include "../libs/sdw/RayTriangleIntersection.h"


#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


class RayTracedRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      void getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 directionOfRay);
};


