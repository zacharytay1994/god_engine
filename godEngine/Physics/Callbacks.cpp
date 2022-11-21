#include "../pch.h"
#include "Callbacks.h"
#include "../EnttXSol/EngineComponents/Physics/EC_Dynamic.h"

#define CONTACTPOINTS 0

namespace god
{
	using namespace physx;


	void ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		for (int i = 0; i < count; i++)
		{
			for (auto const& [ptypeid, fp] : TriggerCallBack)
			{
				int type1 = reinterpret_cast<RigidDynamic*>(pairs[i].triggerShape->getActor()->userData)->PhysicsTypeid;

				if (type1 == ptypeid)
				{
					fp(ConvertToGlmVector( pairs[i].triggerShape->getActor()->getGlobalPose().p ) );
				}

			}


			for (auto const& [ptypeid0, ptypeid1,fp] : TriggerCallBackPair)
			{
				int type0 = reinterpret_cast<RigidDynamic*>(pairs[i].triggerShape->getActor()->userData)->PhysicsTypeid;
				int type1 = reinterpret_cast<RigidDynamic*>(pairs[i].otherShape->getActor()->userData)->PhysicsTypeid;
				if (type0 == ptypeid0 && type1== ptypeid1)
				{
					fp( ConvertToGlmVector(pairs[i].triggerShape->getActor()->getGlobalPose().p) 
						, ConvertToGlmVector(pairs[i].otherShape->getActor()->getGlobalPose().p) );
				}


			}
		}
	}

	void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		
		for (auto const& [ptypeid,fp] : ContactCallBack)
		{
			int type0 = reinterpret_cast<RigidDynamic*>(pairHeader.actors[0]->userData)->PhysicsTypeid;
			int type1 = reinterpret_cast<RigidDynamic*>(pairHeader.actors[1]->userData)->PhysicsTypeid;
			if (type0 == ptypeid)
			{
				fp( ConvertToGlmVector( ( pairHeader.actors[0]->getGlobalPose().p ) ) );
			}
			if (type1 == ptypeid)
			{
				fp(ConvertToGlmVector((pairHeader.actors[1]->getGlobalPose().p)));
			}

		}

		for (auto const& [ptypeid0, ptypeid1, fp] : ContactCallBackPair)
		{
			int type0 = reinterpret_cast<RigidDynamic*>(pairHeader.actors[0]->userData)->PhysicsTypeid;
			int type1 = reinterpret_cast<RigidDynamic*>(pairHeader.actors[1]->userData)->PhysicsTypeid;
			if (type0 == ptypeid0 && type1== ptypeid1)
			{
				fp( ConvertToGlmVector( pairHeader.actors[0]->getGlobalPose().p ), ConvertToGlmVector(pairHeader.actors[1]->getGlobalPose().p ) );
			}
			if (type0 == ptypeid1 && type1 == ptypeid0)
			{
				fp(ConvertToGlmVector(pairHeader.actors[1]->getGlobalPose().p), ConvertToGlmVector(pairHeader.actors[0]->getGlobalPose().p));
			}

		}



#if CONTACTPOINTS 
		//Maximum of 64 vertices can be produced by contact gen
		const PxU32 bufferSize = 64;
		PxVec3 point(0.f, 0.f, 0.f);
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];

			if (cp.events & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND)
			{
				for (auto const& [actor, fp] : ContactTrack)
				{
					if ((pairHeader.actors[0] == actor) ||
						(pairHeader.actors[1] == actor))

					{
						PxU32 nbContacts = pairs[i].extractContacts(&mContacts.front(), bufferSize);
						for (PxU32 j = 0; j < nbContacts; j++)
						{
							point += mContacts[j].position;
							PxVec3 impulse = mContacts[j].impulse;
							PxU32 internalFaceIndex0 = mContacts[j].internalFaceIndex0;
							PxU32 internalFaceIndex1 = mContacts[j].internalFaceIndex1;

							//std::cout << "ContactReportCallback -> Point: " << point << std::endl;
						}
					}
				}
			}
			
		}
#endif




	}







}//god