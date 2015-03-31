/*
 * ReclaimTask.cpp
 *
 *  Created on: Jan 31, 2015
 *      Author: rlcevg
 */

#include "task/builder/ReclaimTask.h"
#include "task/TaskManager.h"
#include "unit/CircuitUnit.h"
#include "terrain/TerrainManager.h"
#include "CircuitAI.h"
#include "util/utils.h"

#include "AISCommands.h"
#include "Unit.h"

namespace circuit {

using namespace springai;

CBReclaimTask::CBReclaimTask(ITaskManager* mgr, Priority priority,
							 UnitDef* buildDef, const AIFloat3& position,
							 float cost, int timeout, float radius) :
		IBuilderTask(mgr, priority, buildDef, position, BuildType::RECLAIM, cost, timeout),
		radius(radius)
{
}

CBReclaimTask::~CBReclaimTask()
{
	PRINT_DEBUG("Execute: %s\n", __PRETTY_FUNCTION__);
}

void CBReclaimTask::RemoveAssignee(CCircuitUnit* unit)
{
	manager->SpecialCleanUp(unit);

	IBuilderTask::RemoveAssignee(unit);
}

void CBReclaimTask::Execute(CCircuitUnit* unit)
{
	Unit* u = unit->GetUnit();

	std::vector<float> params;
	params.push_back(static_cast<float>(priority));
	u->ExecuteCustomCommand(CMD_PRIORITY, params);

	if (target == nullptr) {
		AIFloat3 pos;
		float radius;
		if ((position == -RgtVector) || (radius == .0f)) {
			CTerrainManager* terrain = manager->GetCircuit()->GetTerrainManager();
			float width = terrain->GetTerrainWidth() / 2;
			float height = terrain->GetTerrainHeight() / 2;
			pos = AIFloat3(width, 0, height);
			radius = sqrtf(width * width + height * height);
		} else {
			pos = position;
			radius = this->radius;
		}
		unit->GetUnit()->ReclaimInArea(pos, radius, UNIT_COMMAND_OPTION_INTERNAL_ORDER, FRAMES_PER_SEC * 60);
	} else {
		unit->GetUnit()->ReclaimUnit(target->GetUnit(), UNIT_COMMAND_OPTION_INTERNAL_ORDER, FRAMES_PER_SEC * 60);
	}

	manager->SpecialProcess(unit);
}

void CBReclaimTask::Close(bool done)
{
	for (auto unit : units) {
		manager->SpecialCleanUp(unit);
	}

	IBuilderTask::Close(done);
}

void CBReclaimTask::Cancel()
{
}

} // namespace circuit
