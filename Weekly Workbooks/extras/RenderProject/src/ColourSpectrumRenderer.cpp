#include "ColourSpectrumRenderer.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/Utils.h"


class DrawingWindow;

void ColourSpectrumRenderer::draw(DrawingWindow &window) {
   window.clearPixels();
   // Write some drawing code in here !


   glm::vec3 topLeft(255, 0, 0);        // red
   glm::vec3 topRight(0, 0, 255);       // blue
   glm::vec3 bottomRight(0, 255, 0);    // green
   glm::vec3 bottomLeft(255, 255, 0);   // yellow

   // works out first and last column //
   // std::vector<glm::vec3> firstColumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
   // std::vector<glm::vec3> lastColumn = interpolateThreeElementValues(topRight, bottomRight, window.height);

   // area = 0.5*(window.width) *window.height

   glm::vec2 top(window.width/2, 0); // top corner
   glm::vec2 left(0, window.height); // left corner
   glm::vec2 right(window.width, window.height); // right corner.


   for (size_t y = 0; y < window.height; y++) {

         for (size_t x = 0; x < window.width; x++) {
            glm::vec2 middle(x,y); //  OFC u have to give the middle coordinate here
            glm::vec3 barycentric = convertToBarycentricCoordinates(top, left,right,middle);

            if ((barycentric.x > 0) && (barycentric.y > 0)  && (barycentric.z >0) ) { // if a barycentric coordinate for a pixel is negative then that pixel exists outside triangle
               // selects each row's vec3 and obtains the appropriate coordinate.
               float red =255.0 *barycentric.x;
               float green = 255.0* barycentric.z;
               float blue = 255.0*barycentric.y;

               uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue); // UNSIGNED INTEGER (THEREFORE, NEGATIVE INTEGER)
               window.setPixelColour(x, y, colour);
            }
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
   glm::vec3 intervals((numberOfValues-1.0),(numberOfValues-1.0),(numberOfValues-1.0));
   glm::vec3 cDifference = difference/intervals;

   std::vector <glm::vec3> result;

   for (float i = 0; i < numberOfValues; i++) {
      glm::vec3 p = glm::vec3(i,i,i); // can only multiply by a vec3, hence make a vector p
      result.push_back(from + cDifference*p);
   }

   return result;
}



