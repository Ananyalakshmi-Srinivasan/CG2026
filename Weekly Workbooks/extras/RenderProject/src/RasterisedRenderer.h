#pragma once

#include "Renderer.h"

class RasterisedRenderer: public Renderer {
   public:
      void draw(DrawingWindow &window) override;
};


