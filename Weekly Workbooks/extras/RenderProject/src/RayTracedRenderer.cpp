#include "RayTracedRenderer.h"
#include "Parser.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <map>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "ColourSpectrumRenderer.h"
#include "../libs/sdw/CanvasTriangle.h"
#include "../libs/sdw/Utils.h"
#include <Colour.h>

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/ModelTriangle.h"
// #include "../libs/sdw/RayTriangleIntersection.cpp"
// #include "../libs/sdw/RayTriangleIntersection.h"


glm::vec3 camera(0.0, 0.0, 4.0);
glm::vec3 rayDirection(0.0, 0.0, 1.0);


class DrawingWindow;

void RayTracedRenderer::draw(DrawingWindow &window) {
	//window.clearPixels(); //--> this empies the screen.

}

RayTriangleIntersection RayTracedRenderer::getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 directionOfRay,std::vector<ModelTriangle> &triangles, int index) {

	float absDistance = 0.0;
	std::vector<RayTriangleIntersection> intersections;
	RayTriangleIntersection closestIntersection;

	for (int triangle = 0 ; triangle < triangles.size(); triangle++) {

		glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0]; // p1-p0
		glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0]; //p2 -p0
		glm::vec3 SPVector = cameraPosition - triangle.vertices[0];
		glm::mat3 DEMatrix(-directionOfRay, e0, e1); // -ray direction because negative z is away from us.
		// and screen is in direction opposite us :)
		glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;
		glm::vec3 r = triangle.vertices[0] + possibleSolution.y*e0 + possibleSolution.z*e1;
		glm::vec3 positionVector = cameraPosition + (possibleSolution.x*directionOfRay);

		if (positionVector == r) {
			RayTriangleIntersection intersection = RayTriangleIntersection(r, possibleSolution.t, triangles[triangle], index);
			// collect all intersections and get one with smallest absolute dist.
			intersections.push_back(intersection);
		}
	}

	for (auto&& intersect : intersections) {
		if (intersect.distanceFromCamera < absDistance) {
			absDistance = intersect.distanceFromCamera;
			closestIntersection = intersect;
		}
	}
	return closestIntersection;
}

