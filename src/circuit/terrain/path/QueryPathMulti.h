/*
 * QueryPathMulti.h
 *
 *  Created on: Apr 26, 2020
 *      Author: rlcevg
 */

#ifndef SRC_CIRCUIT_TERRAIN_PATH_QUERYPATHMULTI_H_
#define SRC_CIRCUIT_TERRAIN_PATH_QUERYPATHMULTI_H_

#include "terrain/path/PathQuery.h"

namespace circuit {

class CQueryPathMulti: public IPathQuery {
public:
	CQueryPathMulti(const CPathFinder& pathfinder, int id);
	virtual ~CQueryPathMulti();

	void InitQuery(const springai::AIFloat3& startPos, float maxRange,
			const F3Vec& targets, NSMicroPather::TestFunc&& hitTest,
			float maxThreat, bool endPosOnly);

	void Prepare();

	// Process Data
	springai::AIFloat3& GetStartPosRef() { return startPos; }
	F3Vec& GetTargetsRef() { return targets; }
	PathInfo& GetPathInfoRef() { return *pPath; }
	float& GetPathCostRef() { return pathCost; }

	// Input Data
	const springai::AIFloat3& GetStartPos() const { return startPos; }
	const F3Vec& GetTargets() const { return targets; }
	const float GetMaxRange() const { return maxRange; }
	const NSMicroPather::TestFunc& GetHitTest() const { return hitTest; }
	const float GetMaxThreat() const { return maxThreat; }

	// Result
	const std::shared_ptr<PathInfo>& GetPathInfo() const { return pPath; }
	const float GetPathCost() const { return pathCost; }

private:
	std::shared_ptr<PathInfo> pPath;
	float pathCost = 0.f;

	springai::AIFloat3 startPos;
	F3Vec targets;
	float maxRange = 0.f;
	NSMicroPather::TestFunc hitTest;
	float maxThreat = 0.f;
	bool endPosOnly = false;
};

} // namespace circuit

#endif // SRC_CIRCUIT_TERRAIN_PATH_QUERYPATHMULTI_H_
