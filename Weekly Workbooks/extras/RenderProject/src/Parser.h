#pragma once

#include "Renderer.h"
#include "CanvasPoint.h"
#include "CanvasTriangle.h"
#include <Colour.h>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


class Parser: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      std::vector<ModelTriangle> readObjFile(std::string filename, std::vector<ModelTriangle>, float scaleFactor,std::map<std::string,Colour> colourMap);
      std::map<std::string, Colour> readMatFile(std::string filename, std::map<std::string,Colour> colourTable);
};


