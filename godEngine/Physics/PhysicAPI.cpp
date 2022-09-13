#include "../pch.h"
#include "PhysicAPI.h"

namespace god
{
	static PhysicData* p_physicData;

	void PhysicErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		const char* errorMessage = NULL;

		switch (code)
		{
		case physx::PxErrorCode::eNO_ERROR:				errorMessage = "No Error"; break;
		case physx::PxErrorCode::eDEBUG_INFO:			errorMessage = "Info"; break;
		case physx::PxErrorCode::eDEBUG_WARNING:		errorMessage = "Warning"; break;
		case physx::PxErrorCode::eINVALID_PARAMETER:	errorMessage = "Invalid Parameter"; break;
		case physx::PxErrorCode::eINVALID_OPERATION:	errorMessage = "Invalid Operation"; break;
		case physx::PxErrorCode::eOUT_OF_MEMORY:		errorMessage = "Out Of Memory"; break;
		case physx::PxErrorCode::eINTERNAL_ERROR:		errorMessage = "Internal Error"; break;
		case physx::PxErrorCode::eABORT:				errorMessage = "Abort"; break;
		case physx::PxErrorCode::ePERF_WARNING:			errorMessage = "Performance Warning"; break;
		case physx::PxErrorCode::eMASK_ALL:				errorMessage = "Unknown Error"; break;
		}

		switch (code)
		{
		case physx::PxErrorCode::eNO_ERROR:
		case physx::PxErrorCode::eDEBUG_INFO:
			//LOG("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
			break;
		case physx::PxErrorCode::eDEBUG_WARNING:
		case physx::PxErrorCode::ePERF_WARNING:
			//LOG("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
			break;
		case physx::PxErrorCode::eINVALID_PARAMETER:
		case physx::PxErrorCode::eINVALID_OPERATION:
		case physx::PxErrorCode::eOUT_OF_MEMORY:
		case physx::PxErrorCode::eINTERNAL_ERROR:
			//LOG("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);
			break;
		case physx::PxErrorCode::eABORT:
		case physx::PxErrorCode::eMASK_ALL:
			//LOG("[PhysX]: {0}: {1} at {2} ({3})", errorMessage, message, file, line);

			break;
		}
	}

	void PhysicAssertHandler::operator()(const char* exp, const char* file, int line, bool& ignore)
	{
		//LOG("[PhysX Error]: {0}:{1} - {2}", file, line, exp);
	}

	void PhysicAPI::Init()
	{
		assert(!p_physicData, "Attempt to initialize the PhysX numerous times!");
		p_physicData = new PhysicData();

		p_physicData->m_PhysXFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, p_physicData->Allocator, p_physicData->ErrorCallback);
		assert(!p_physicData->m_PhysXFoundation, "Creation of PhysXFoundation FAILED!");

		physx::PxTolerancesScale physic_scale = physx::PxTolerancesScale();
		physic_scale.length = 1.f;
		physic_scale.speed = 100.f;

		//p_physicData->m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *p_physicData->m_PhysXFoundation, physic_scale, true, (physx::PxPvd)*);
		//assert(s_PhysXData->PhysXSDK, "PxCreatePhysics failed.");

		//s_PhysXData->PhysXCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	}
	void PhysicAPI::Shutdown()
	{
	}



}