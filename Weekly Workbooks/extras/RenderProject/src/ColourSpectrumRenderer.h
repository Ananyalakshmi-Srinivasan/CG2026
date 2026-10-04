#pragma once

#include "Renderer.h"
#include <vector>
#include <glm/glm.hpp>

class ColourSpectrumRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
      std::vector<float> interpolateSingleFloats(float from, float to, float numberOfValues);
      std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues);
};
