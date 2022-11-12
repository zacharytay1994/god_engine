#include "../pch.h"
#include "Callbacks.h"

namespace god
{
	using namespace physx;





	void ContactReportCallback::AddPosCb(const PxRigidBody* const rb, void (*function)(glm::vec3 const& pos))
	{
		for (auto const& x : trackedrb)
		{
			if (x.first == rb)
				return;
		}
		trackedrb.push_back({ rb, function });
		//std::cout << "added to cb\n";
	}


	void ContactReportCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		

		for (auto const& x : trackedrb)
		{
			const PxRigidActor* const localrb = x.first;
			if ((pairHeader.actors[0] == localrb) ||
				(pairHeader.actors[1] == localrb))
			{
				x.second(ConvertToGlmVector(localrb->getGlobalPose().p));
			}

		}
#define CONTACTPOINTS 0
#if CONTACTPOINTS
		//Maximum of 64 vertices can be produced by contact gen
		const PxU32 bufferSize = 64;

		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& cp = pairs[i];

			PxU32 nbContacts = pairs[i].extractContacts(&mContacts.front(), bufferSize);
			for (PxU32 j = 0; j < nbContacts; j++)
			{
				PxVec3 point = mContacts[j].position;
				PxVec3 impulse = mContacts[j].impulse;
				PxU32 internalFaceIndex0 = mContacts[j].internalFaceIndex0;
				PxU32 internalFaceIndex1 = mContacts[j].internalFaceIndex1;
				
				std::cout << "ContactReportCallback -> Point: " << point << std::endl;
			}
			
		}
#endif
	}







}//god