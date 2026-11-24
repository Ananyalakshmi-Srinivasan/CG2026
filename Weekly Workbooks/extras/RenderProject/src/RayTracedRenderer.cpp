#include "RayTracedRenderer.h"
#include "utilityFunctions.h"
#include <Colour.h>
#include <map>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include "../libs/sdw/CanvasTriangle.h"

#include "../libs/sdw/DrawingWindow.h"
#include "../libs/sdw/ModelTriangle.h"
#include "../libs/sdw/RayTriangleIntersection.h"


class DrawingWindow;

void RayTracedRenderer::draw(DrawingWindow &window) {
	window.clearPixels(); //--> this empties the screen.
	glm::mat3 rotationMatrix = utilityFunctions::panMatrix(0.01);

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

				float diffuse = diffuseLighting(lightPosition, intersection);
				double specular = specularLighting(lightPosition, intersection, rayDirection);
				float brightness = fmax(diffuse,specular);
				Colour intensityAdjustedColour = Colour(intersectionColour.red*brightness,
									intersectionColour.green*brightness,
									intersectionColour.blue*brightness);

				uint32_t intersectColour = utilityFunctions::convertColourToUint32(intensityAdjustedColour);
				window.setPixelColour(x, y, intersectColour);
			}
		}
	}
}
float RayTracedRenderer::getPointBrightness(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
	float distanceFromLight = glm::length(lightPosition-intersection.intersectionPoint); // this is r.
	float maxIntensity = 7.0; //varies the strength of the light... can change how much of the room you can see with this.
    float brightness = maxIntensity/(4*PI*distanceFromLight*distanceFromLight); // 1/4pi because max intensity/brightness is 1.
	if (brightness > 1.0) { return 1.0; }
		// for brightness values which are very big --> happens when distance is small.
		// 1.0 is the maximum we use for brightness it means we want 100% of our colour!
		// clamping necessary for points close to the ceiling to prevent ripple effect(i.e. ceilings parts near point light)
	return brightness;
}

float RayTracedRenderer::getAngleOfIncidence(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
    glm::vec3 vectorToLight = glm::normalize(-(lightPosition-intersection.intersectionPoint));
	float angle = glm::dot(intersection.intersectedTriangle.normal, vectorToLight);
	// angle is 1.0 if normal and vector to light are parallel
	// 0.0 if normal and vector to light are perpendictular.
	// something in between if the angle is in between.
	// less than zero if normal points away.
	return angle;
}

float RayTracedRenderer::diffuseLighting(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
	float brightness = getPointBrightness(lightPosition, intersection);
	float angle = getAngleOfIncidence(lightPosition, intersection);
	if (angle >0 ) brightness *= angle;
	return brightness;
}

double RayTracedRenderer::specularLighting(glm::vec3 lightPosition, RayTriangleIntersection intersection ,glm::vec3 viewVector) {
	glm::vec3 vectorFromLight = glm::normalize(lightPosition-intersection.intersectionPoint);
	glm::vec3 reflectedRay = glm::reflect(vectorFromLight, intersection.intersectedTriangle.normal);
    // float dot = glm::dot(vectorFromLight, intersection.intersectedTriangle.normal)*2;

	// glm::vec3 reflectedRay = vectorFromLight - glm::vec3(dot,dot,dot)*intersection.intersectedTriangle.normal;
	double dotViewandReflection = glm::dot(viewVector, reflectedRay);
	dotViewandReflection = pow(dotViewandReflection,128);
	return dotViewandReflection;
}



// float RayTracedRenderer::getPointBrightness(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
// 	float distanceFromLight = glm::length(lightPosition-intersection.intersectionPoint); // this is r.
// 	float intensity = 1/(4*PI*distanceFromLight*distanceFromLight); // 1/4pi because max intensity/brightness is 1.
// 	std::cout << intensity << std::endl;
// 	return intensity;
// } // dividing this brightness produced a different interesting light effect.

// float RayTracedRenderer::getPointBrightness(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
// 	float distanceFromLight = glm::length(intersection.intersectionPoint-lightPosition); // this is r.
// 	float intensity = 0.25*PI*distanceFromLight*distanceFromLight); // 1/4pi because max intensity/brightness is 1.
// } does circular prettiness :)




glm::vec3 RayTracedRenderer::transposePlaneCoordsToRay(glm::vec3 planePoint, float scaleFactor) {
	float x=planePoint.x -WIDTH/2;
	float y= HEIGHT/2-planePoint.y; //otherwise box is upside down.
	glm::vec3 rayDirection = glm::vec3(x, y, planePoint.z*scaleFactor); // inverting  z coordinate to work with forward orientation.
	glm::vec3 rayNormalised = glm::normalize(rayDirection);

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

// doesn't intersect with white triangle.
Colour RayTracedRenderer::castHardShadows(glm::vec3 lightPosition, RayTriangleIntersection intersection) {
	// hard shadows //
	glm::vec3 lightRayDirection = (intersection.intersectionPoint-lightPosition);
	glm::vec3 lightRayNormalised = glm::normalize(lightRayDirection);
	RayTriangleIntersection lightIntersection = getClosestIntersection(lightRayNormalised, lightPosition);

	if (lightIntersection.distanceFromCamera != -1.0) {
		if ((lightIntersection.distanceFromCamera < glm::length(lightRayDirection))) {
			if (intersection.triangleIndex != lightIntersection.triangleIndex) {
				// this removes shadow acne around light box // --> ask if theres any other way...this is way too pedantic.
				uint32_t cyan = utilityFunctions::convertColourToUint32(colourPalette.find("Cyan")->second);
				if (utilityFunctions::convertColourToUint32(intersection.intersectedTriangle.colour) == cyan) {
					return intersection.intersectedTriangle.colour;
				}
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
