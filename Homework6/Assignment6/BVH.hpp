//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include "Object.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Vector.hpp"

#include <atomic>
#include <vector>
#include <memory>
#include <ctime>

// BVHAccel Forward Declarations
struct BVHBuildNode;

// BVHAccel Declarations
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;
class BVHAccel
{
public:
	// BVHAccel Public Types
	enum class SplitMethod { NAIVE, SAH };

	// BVHAccel Public Methods
#if USE_SAH
	BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::SAH);
#else
	BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);
#endif
	Bounds3 WorldBound() const;
	~BVHAccel();

	Intersection Intersect(const Ray& ray) const;
	Intersection getIntersection(BVHBuildNode* node, const Ray& ray)const;
	bool IntersectP(const Ray& ray) const;
	BVHBuildNode* root;

	// BVHAccel Private Methods
	BVHBuildNode* recursiveBuild(std::vector<Object*>objects);

	// BVHAccel Private Data
	const int maxPrimsInNode;
	const SplitMethod splitMethod;
	std::vector<Object*> primitives;
};

struct BVHBuildNode
{
	Bounds3 bounds;
	BVHBuildNode* left;
	BVHBuildNode* right;
	Object* object;

public:
	int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
	// BVHBuildNode Public Methods
	BVHBuildNode()
	{
		bounds = Bounds3();
		left = nullptr; right = nullptr;
		object = nullptr;
	}
};

struct Bucket
{
	Bounds3 bounds;
	int count;
	std::vector<Object*> objects;
};

#endif //RAYTRACING_BVH_H
