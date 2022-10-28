#pragma once

#include "PxPhysicsAPI.h"
#include "PhysicUtils.h"
#include "../PhysX/physx/snippets/snippetutils/SnippetUtils.h"

namespace god
{
	using namespace physx;

	class ContactReportCallback : public PxSimulationEventCallback
	{
		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
		void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
		void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
		void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
		{
			PX_UNUSED((pairHeader));
			//Maximum of 64 vertices can be produced by contact gen
			const PxU32 bufferSize = 64;
			PxContactPairPoint contacts[bufferSize];
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				const PxContactPair& cp = pairs[i];

				PxU32 nbContacts = pairs[i].extractContacts(contacts, bufferSize);
				for (PxU32 j = 0; j < nbContacts; j++)
				{
					PxVec3 point = contacts[j].position;
					PxVec3 impulse = contacts[j].impulse;
					PxU32 internalFaceIndex0 = contacts[j].internalFaceIndex0;
					PxU32 internalFaceIndex1 = contacts[j].internalFaceIndex1;
					//...
					//std::cout << "ContactReportCallback -> Point: " << point << std::endl;
				}
			}
		}
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
