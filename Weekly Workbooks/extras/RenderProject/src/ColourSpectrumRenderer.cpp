#include "ColourSpectrumRenderer.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <glm/glm.hpp>


void ColourSpectrumRenderer::draw(DrawingWindow &window) {
   window.clearPixels();
   // Write some drawing code in here !

   std::vector <float> colourPixels = interpolateSingleFloats(255.0, 0, window.width);


   for (size_t y = 0; y < window.height; y++) { // vertical coordinate
      for (size_t x = 0; x < window.width; x++) { // horizontal coordinate

         float red = colourPixels[x];
         float green = colourPixels[x];
         float blue = colourPixels[x];

         uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
         window.setPixelColour(x, y, colour);
      }
   }
}

// the gradient is horizontal --> so as x changes colour should become more gray.from and to represent the colours, number ofvalues is the window.wdith

// ColourSpectrumRenderer::function_name --> states its a member of the colour spectrum renderer
std::vector<float> ColourSpectrumRenderer::interpolateSingleFloats(float from, float to, float numberOfValues) {

   float difference = to-from; // find the difference between start and end/
   float intervals = numberOfValues -1.0;  // find the number of intervals
   float cDifference = difference/intervals; // find common difference between vector values.

   std::vector <float> result;
   for (float i = 0; i < numberOfValues; i++) {
      result.push_back(from + cDifference*i); // start + c.difference gives colour at different pixel
   }

   return result;
}

std::vector<glm::vec3> ColourSpectrumRenderer::interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, float numberOfValues) {

   glm::vec3 difference = to-from;
   float intervals = numberOfValues -1.0;
   glm::vec3 cDifferences = difference/intervals;




   std::vector <glm::vec3> result;




   return result;
}



