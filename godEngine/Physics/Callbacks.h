#pragma once

#include "PxPhysicsAPI.h"
#include "PhysicUtils.h"
#include "DiceCallBack.h"
#include "SnippetUtils.h"



#include <vector>
#include <unordered_map>
namespace god
{
	using namespace physx;




	class ContactReportCallback : public PxSimulationEventCallback
	{
	public:
		ContactReportCallback() 
		{
			mContacts.resize(64);

		}
		~ContactReportCallback() {}




	

	private:


		std::vector< PxContactPairPoint> mContacts;



		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
		void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onTrigger(PxTriggerPair* pairs, PxU32 count); 
		void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	};




	class CallbackFinishTask : public PxLightCpuTask
	{
		SnippetUtils::Sync* mSync;
	public:
		CallbackFinishTask() { mSync = SnippetUtils::syncCreate(); }
		~CallbackFinishTask() {

		}

		void free()
		{
			SnippetUtils::syncRelease(mSync);
		}
		virtual void release()
		{
			PxLightCpuTask::release();
			SnippetUtils::syncSet(mSync);
		}

		void reset() { SnippetUtils::syncReset(mSync); }

		void wait() { SnippetUtils::syncWait(mSync); }

		virtual void run() { /*Do nothing - release the sync in the release method for thread-safety*/ }

		virtual const char* getName() const { return "CallbackFinishTask"; }
	};




}
