/*
 * FightAction.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: rlcevg
 */

#include "unit/action/FightAction.h"
#include "unit/CircuitUnit.h"
#include "CircuitAI.h"
#include "util/Utils.h"

#include "AISCommands.h"

namespace circuit {

using namespace springai;

CFightAction::CFightAction(CCircuitUnit* owner, int squareSize, float speed)
		: ITravelAction(owner, Type::FIGHT, squareSize, speed)
{
}

CFightAction::CFightAction(CCircuitUnit* owner, const std::shared_ptr<PathInfo>& pPath,
		int squareSize, float speed)
		: ITravelAction(owner, Type::FIGHT, pPath, squareSize, speed)
{
}

CFightAction::~CFightAction()
{
}

void CFightAction::Update(CCircuitAI* circuit)
{
	if (lastFrame + FRAMES_PER_SEC * 2 > circuit->GetLastFrame()) {
		return;
	}
	lastFrame = circuit->GetLastFrame();
	CCircuitUnit* unit = static_cast<CCircuitUnit*>(ownerList);

	float stepSpeed;
	int pathMaxIndex = CalcSpeedStep(stepSpeed);
	if (pathMaxIndex < 0) {
		return;
	}
	int step = pathIterator;

	TRY_UNIT(circuit, unit,
		const AIFloat3& pos = pPath->posPath[step];
		unit->GetUnit()->Fight(pos, UNIT_COMMAND_OPTION_RIGHT_MOUSE_KEY, lastFrame + FRAMES_PER_SEC * 60);
		unit->CmdWantedSpeed(stepSpeed);

		constexpr short options = UNIT_COMMAND_OPTION_RIGHT_MOUSE_KEY | UNIT_COMMAND_OPTION_SHIFT_KEY;
		for (int i = 2; (step < pathMaxIndex) && (i < 3); ++i) {
			step = std::min(step + increment, pathMaxIndex);
			const AIFloat3& pos = pPath->posPath[step];
			unit->GetUnit()->Fight(pos, options, lastFrame + FRAMES_PER_SEC * 60 * i);
		}
	)
}

} // namespace circuit
