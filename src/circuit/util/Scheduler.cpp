/*
 * Scheduler.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: rlcevg
 */

#include "util/Scheduler.h"
#include "util/utils.h"

#include "CircuitAI.h"

namespace circuit {

CMultiQueue<CScheduler::WorkTask> CScheduler::workTasks;
std::thread CScheduler::workerThread;
std::atomic<bool> CScheduler::workerRunning(false);
unsigned int CScheduler::counterInstance = 0;

CScheduler::CScheduler(CCircuitAI* ai)
		: ai(ai)
		, lastFrame(-1)
		, isProcessing(false)
{
	counterInstance++;
}

CScheduler::~CScheduler()
{
	PRINT_DEBUG("Execute: %s\n", __PRETTY_FUNCTION__);
	counterInstance--;
	Release();
}

void CScheduler::ProcessRelease()
{
	for (auto& task : releaseTasks) {
		task->Run();
	}
}

void CScheduler::Release()
{
	std::weak_ptr<CScheduler>& scheduler = self;
	CMultiQueue<WorkTask>::ConditionFunction condition = [&scheduler](WorkTask& item) -> bool {
		return !scheduler.owner_before(item.scheduler) && !item.scheduler.owner_before(scheduler);
	};
	workTasks.RemoveAllIf(condition);

	if (counterInstance == 0 && workerRunning.load()) {
		workerRunning = false;
		// At this point workTasks is empty. Push empty task in case worker stuck at Pop().
		workTasks.Push({self, nullptr, nullptr});
		if (workerThread.joinable()) {
			PRINT_DEBUG("Entering join: %s\n", __PRETTY_FUNCTION__);
			workerThread.join();
			PRINT_DEBUG("Leaving join: %s\n", __PRETTY_FUNCTION__);
		}
	}
}

void CScheduler::RunTaskEvery(std::shared_ptr<CGameTask> task, int frameInterval, int frameOffset)
{
	if (frameOffset > 0) {
		RunTaskAfter(std::make_shared<CGameTask>("CScheduler_delay", [this, task, frameInterval]() {
			repeatTasks.push_back({task, frameInterval, lastFrame});
		}), frameOffset);
	} else {
		repeatTasks.push_back({task, frameInterval, lastFrame});
	}
}

void CScheduler::ProcessTasks(int frame)
{
	isProcessing = true;
	lastFrame = frame;

	// Process once tasks
	std::vector<OnceTask>::iterator ionce = onceTasks.begin();
	while (ionce != onceTasks.end()) {
		if (ionce->frame <= frame) {
			try {
				ionce->task->Run();
			} catch (const std::exception& e) {
				ai->LOG("%i | CScheduler::ProcessTasks Once EXCEPTION:\n%s", ai->GetSkirmishAIId(), e.what());
				ai->LOG("%s", ionce->task->name.c_str());
			} catch (...) {
				ai->LOG("%i | CScheduler::ProcessTasks Once ELSE EXCEPTION", ai->GetSkirmishAIId());
				ai->LOG("%s", ionce->task->name.c_str());
			}
			ionce = onceTasks.erase(ionce);  // alternatively, onceTasks.erase(iter++);
		} else {
			++ionce;
		}
	}

	// Process repeat tasks
	for (auto& container : repeatTasks) {
		if (frame - container.lastFrame >= container.frameInterval) {
			try {
				container.task->Run();
			} catch (const std::exception& e) {
				ai->LOG("%i | CScheduler::ProcessTasks Repeat EXCEPTION:\n%s", ai->GetSkirmishAIId(), e.what());
				ai->LOG("%s", container.task->name.c_str());
			} catch (...) {
				ai->LOG("%i | CScheduler::ProcessTasks Repeat ELSE EXCEPTION", ai->GetSkirmishAIId());
				ai->LOG("%s", container.task->name.c_str());
			}
			container.lastFrame = frame;
		}
	}

	try {
		// Process onComplete from parallel tasks
		CMultiQueue<FinishTask>::ProcessFunction process = [](FinishTask& item) {
			item.task->Run();
		};
		finishTasks.PopAndProcess(process);
	} catch (const std::exception& e) {
		ai->LOG("%i | CScheduler::ProcessTasks PopAndProcess EXCEPTION:\n%s", ai->GetSkirmishAIId(), e.what());
	} catch (...) {
		ai->LOG("%i | CScheduler::ProcessTasks PopAndProcess ELSE EXCEPTION", ai->GetSkirmishAIId());
	}

	// Update task queues
	if (!removeTasks.empty()) {
		ai->LOG("%i | CScheduler::ProcessTasks Remove BEGIN | %i", ai->GetSkirmishAIId(), removeTasks.size());
		for (auto& task : removeTasks) {
			auto oend = std::remove(onceTasks.begin(), onceTasks.end(), OnceTask(task, 0));
			if (oend != onceTasks.end()) {
				ai->LOG("Once: %s", task->name.c_str());
			}
			onceTasks.erase(oend, onceTasks.end());
			auto rend = std::remove(repeatTasks.begin(), repeatTasks.end(), RepeatTask(task, 0, 0));
			if (rend != repeatTasks.end()) {
				ai->LOG("Repeat: %s", task->name.c_str());
			}
			repeatTasks.erase(rend, repeatTasks.end());
		}
		removeTasks.clear();
		ai->LOG("% i | CScheduler::ProcessTasks Remove END", ai->GetSkirmishAIId());
	}

	isProcessing = false;
}

void CScheduler::RunParallelTask(std::shared_ptr<CGameTask> task, std::shared_ptr<CGameTask> onComplete)
{
	if (!workerRunning.load()) {
		workerRunning = true;
		// TODO: Find out more about std::async, std::bind, std::future.
		workerThread = std::thread(&CScheduler::WorkerThread);
	}
	workTasks.Push({self, task, onComplete});
}

void CScheduler::RemoveTask(std::shared_ptr<CGameTask>& task)
{
	if (isProcessing) {
		removeTasks.push_back(task);
	} else {
		auto oend = std::remove(onceTasks.begin(), onceTasks.end(), OnceTask(task, 0));
		onceTasks.erase(oend, onceTasks.end());
		auto rend = std::remove(repeatTasks.begin(), repeatTasks.end(), RepeatTask(task, 0, 0));
		repeatTasks.erase(rend, repeatTasks.end());
	}
}

void CScheduler::WorkerThread()
{
	WorkTask container = workTasks.Pop();
	while (workerRunning.load()) {
		container.task->Run();
		container.task = nullptr;
		if (container.onComplete != nullptr) {
			std::shared_ptr<CScheduler> scheduler = container.scheduler.lock();
			if (scheduler) {
				scheduler->finishTasks.Push(container);
			}
			container.onComplete = nullptr;
		}
		container = workTasks.Pop();
	}
	PRINT_DEBUG("Exiting: %s\n", __PRETTY_FUNCTION__);
}

} // namespace circuit
