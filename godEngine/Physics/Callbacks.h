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
			for (auto& [id1,id2,fp] : ContactCallBackPair)
			{
				ContactPairTrack.push_back({ id1,id2,fp ,nullptr,nullptr });
			}
		}
		~ContactReportCallback() {}


		void AddToContactTrack(const PxRigidBody* const rb, void (*function)(glm::vec3 const& pos));
		
		std::vector < std::tuple <uint32_t, uint32_t, void (*)(glm::vec3 const& pos, glm::vec3 const& pos1),
			PxRigidActor*, PxRigidActor*  > >& getContactPairTrack();

	

	private:


		std::vector< PxContactPairPoint> mContacts;
		std::vector < std::pair< const PxRigidActor* const, void (*)(glm::vec3 const& pos) > > ContactTrack;
		std::vector < std::tuple <uint32_t, uint32_t, void (*)(glm::vec3 const& pos, glm::vec3 const& pos1),
			 PxRigidActor* ,	 PxRigidActor*  > > 
			ContactPairTrack;


		void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
		void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
		void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
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
