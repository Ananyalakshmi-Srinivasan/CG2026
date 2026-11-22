#include "RayTracedRenderer.h"
#include "utilityFunctions.h"
#include <CanvasPoint.h>
#include <Colour.h>
#include <map>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "../libs/sdw/CanvasTriangle.h"
#include "../libs/sdw/Utils.h"
#include <Colour.h>

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/ModelTriangle.h"
#include "../libs/sdw/RayTriangleIntersection.h"


class DrawingWindow;

void RayTracedRenderer::draw(DrawingWindow &window) {
	window.clearPixels(); //--> this empties the screen.
	glm::mat3 rotationMatrix = utilityFunctions::panMatrix(0.01);
	glm::vec3 lightPosInModelCoord = transposePlaneCoordsToRay(lightPosition, 160.0);
	// check if this light intersections with each triangle.

	if (orbit == true) {
		glm::vec3 centreOfScene(0.0,0.0,0.0);
		cameraPosition = utilityFunctions::orbit(cameraPosition, centreOfScene,rotationMatrix);
		if (lookAt == true) {
			cameraOrientation = utilityFunctions::lookAt(cameraPosition, centreOfScene, cameraOrientation);
		}
	}

	for (int x = 0; x < window.width; x++) {
		for (int y = 0; y < window.height; y++) {
			glm::vec3 rayDirection = transposePlaneCoordsToRay(glm::vec3(x, y, focalLength),160.0);

			// intersection point --> 3d point in world where intersection took place. check if this intersects with light
			RayTriangleIntersection intersection = getClosestIntersection(rayDirection, cameraPosition);
			if (intersection.distanceFromCamera != -1.0) {
				Colour intersectionColour;
				if (hardShadows) intersectionColour = castHardShadows(lightPosition, intersection);
				else intersectionColour = intersection.intersectedTriangle.colour;

				uint32_t intersectColour = utilityFunctions::convertColourToUint32(intersectionColour);
				window.setPixelColour(x, y, intersectColour);
				// //std::cout << faintersection.intersectedTriangle.colour << "intersection" << std::endl;
				// uint32_t colour = utilityFunctions::convertColourToUint32(triangleColour);
				// window.setPixelColour(x, y, colour);
				// //std::cout << "bleh" << std::endl;
			}
		}
	}
}


glm::vec3 RayTracedRenderer::transposePlaneCoordsToRay(glm::vec3 planePoint, float scaleFactor) {
	float x=planePoint.x -WIDTH/2;
	float y= HEIGHT/2-planePoint.y; //otherwise box is upside down.
	glm::vec3 rayDirection = glm::vec3(x, y, planePoint.z*scaleFactor); // inverting  z coordinate to work with forward orientation.
	glm::vec3 rayNormalised = glm::normalize(rayDirection);
	//std::cout << rayNormalised.x << " " << rayNormalised.y << std::endl;

	return cameraOrientation*rayNormalised;
}

std::vector<RayTriangleIntersection> RayTracedRenderer::sortPointsByT(std::vector<RayTriangleIntersection>  unsortedIntersections) {

	int n = unsortedIntersections.size();

	// Outer loop that corresponds to the number of elements to be sorted
	for (int i = 0; i < n - 1; i++) {
		// Last i elements are already in place
		for (int j = 0; j < n - i - 1; j++) {

			// Comparing adjacent elements
			if (unsortedIntersections[j].distanceFromCamera > unsortedIntersections[j + 1].distanceFromCamera)

				// Swapping if in the wrong order
				std::swap(unsortedIntersections[j], unsortedIntersections[j + 1]);
		}
	}
	return unsortedIntersections;
}

//raydirection --> get coordinates of surface point. subtract light source to get ray direction and normalise .
// pass to get closest intersection //
// if the colour of the closest intersection is white --> we see light
// else draw black pixel

// doesn't intersect with white triangle.
Colour RayTracedRenderer::castHardShadows(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
	// hard shadows //
	glm::vec3 lightRayDirection = (intersection.intersectionPoint-lightPosition);
	glm::vec3 lightRayNormalised = glm::normalize(lightRayDirection);
	RayTriangleIntersection lightIntersection = getClosestIntersection(lightRayNormalised, lightPosition);

	if (lightIntersection.distanceFromCamera != -1.0) {
		if ((lightIntersection.distanceFromCamera <= glm::length(lightRayDirection))) {
			if (intersection.triangleIndex != lightIntersection.triangleIndex) {
				// if the triangle its intersecting is not itself THEN theres a box or alternate surface in the way
				// then u colour it black.
				return {0,0,0};
			}
		}
	}
	return intersection.intersectedTriangle.colour; // colour by pixel colour if light can see.
}

// what happens if no intersection?? --> that's why validation.
RayTriangleIntersection RayTracedRenderer::getClosestIntersection(glm::vec3 rayDirection, glm::vec3 rayOrigin) { //ray Origin is camera or light
	std::vector<RayTriangleIntersection> intersections;
	RayTriangleIntersection closestIntersection = RayTriangleIntersection(rayOrigin, -1.0, triangles[0],-1);

	size_t triangleInd = 0;
	for (auto&&triangle : triangles) {

		//std::cout << triangle.vertices[0] << " " << triangle.vertices[1] << " " << triangle.vertices[2] << std::endl;

		glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
		glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
		glm::vec3 SPVector = (rayOrigin - triangle.vertices[0]) ;
		glm::mat3 DEMatrix(-rayDirection, e0, e1);
		glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

		// and screen is in direction opposite us :)
		// t, u, v vector
		float t = possibleSolution.x;
		float u = possibleSolution.y;
		float v = possibleSolution.z;
		// std::cout << t << ", " << u << ", " << v << std::endl;

		// adds intersection to intersection list if u and v are within the triangle
		// and if t is positive --> if t < 0 then the triangle renders behind camera so don't need the intersection.
		if (((u >= 0.0)&& (u <= 1.0)) ) {
			if ((v >= 0.0)&& (v <= 1.0)) {
				if (((u+v) <= 1.0) && (t >= 0.0)){
					glm::vec3 positionVector = (rayOrigin + (t*rayDirection));

					RayTriangleIntersection intersection = RayTriangleIntersection(positionVector, possibleSolution.x, triangle, triangleInd);
					// collect all intersections and get one with smallest absolute dist.
					intersections.push_back(intersection);
				}
			}
		}
		triangleInd++;
	}

	if (intersections.empty()) return closestIntersection;
	// do after u check for all possible intersects.

    intersections = sortPointsByT(intersections);
	closestIntersection = intersections[0];

	return closestIntersection;
}
