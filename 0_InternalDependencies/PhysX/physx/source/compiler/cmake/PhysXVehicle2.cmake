## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##  * Neither the name of NVIDIA CORPORATION nor the names of its
##    contributors may be used to endorse or promote products derived
##    from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Copyright (c) 2008-2022 NVIDIA Corporation. All rights reserved.

#
# Build PhysXVehicle2 common
#

SET(PHYSX_SOURCE_DIR ${PHYSX_ROOT_DIR}/source)
SET(LL_SOURCE_DIR ${PHYSX_SOURCE_DIR}/physxvehicle2/src)

# Include here after the directories are defined so that the platform specific file can use the variables.
include(${PHYSX_ROOT_DIR}/${PROJECT_CMAKE_FILES_DIR}/${TARGET_BUILD_PLATFORM}/PhysXVehicle2.cmake)

SET(PHYSX_VEHICLE2_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleAPI.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleComponent.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleComponentSequence.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleLimits.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/PxVehicleMaths.h
)
SET(PHYSX_VEHICLE2_BRAKING_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/braking/PxVehicleBrakingFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/braking/PxVehicleBrakingParams.h
)
SET(PHYSX_VEHICLE2_COMMAND_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/commands/PxVehicleCommandHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/commands/PxVehicleCommandParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/commands/PxVehicleCommandStates.h
)
SET(PHYSX_VEHICLE2_DRIVETRAIN_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/drivetrain/PxVehicleDrivetrainComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/drivetrain/PxVehicleDrivetrainFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/drivetrain/PxVehicleDrivetrainHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/drivetrain/PxVehicleDrivetrainParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/drivetrain/PxVehicleDrivetrainStates.h
)
SET(PHYSX_VEHICLE2_PHYSXACTOR_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/physxActor/PxVehiclePhysXActorComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxActor/PxVehiclePhysXActorFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxActor/PxVehiclePhysXActorHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxActor/PxVehiclePhysXActorStates.h
)
SET(PHYSX_VEHICLE2_PHYSXCONSTRAINT_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintStates.h
)
SET(PHYSX_VEHICLE2_PHYSXROADGEOMETRY_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryParams.h
    ${PHYSX_ROOT_DIR}/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryState.h
)
SET(PHYSX_VEHICLE2_RIGIDBODY_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/rigidBody/PxVehicleRigidBodyComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/rigidBody/PxVehicleRigidBodyFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/rigidBody/PxVehicleRigidBodyParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/rigidBody/PxVehicleRigidBodyStates.h
)
SET(PHYSX_VEHICLE2_ROADGEOMETRY_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/roadGeometry/PxVehicleRoadGeometryState.h
)
SET(PHYSX_VEHICLE2_STEERING_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/steering/PxVehicleSteeringFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/steering/PxVehicleSteeringParams.h
)
SET(PHYSX_VEHICLE2_SUSPENSION_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/suspension/PxVehicleSuspensionComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/suspension/PxVehicleSuspensionFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/suspension/PxVehicleSuspensionParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/suspension/PxVehicleSuspensionStates.h
	${PHYSX_ROOT_DIR}/include/vehicle2/suspension/PxVehicleSuspensionHelpers.h
)
SET(PHYSX_VEHICLE2_TIRE_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/tire/PxVehicleTireComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/tire/PxVehicleTireFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/tire/PxVehicleTireHelpers.h
	${PHYSX_ROOT_DIR}/include/vehicle2/tire/PxVehicleTireParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/tire/PxVehicleTireStates.h
)
SET(PHYSX_VEHICLE2_WHEEL_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/wheel/PxVehicleWheelComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/wheel/PxVehicleWheelFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/wheel/PxVehicleWheelParams.h
	${PHYSX_ROOT_DIR}/include/vehicle2/wheel/PxVehicleWheelStates.h
	${PHYSX_ROOT_DIR}/include/vehicle2/wheel/PxVehicleWheelHelpers.h
)
SET(PHYSX_VEHICLE2_PVD_HEADERS
	${PHYSX_ROOT_DIR}/include/vehicle2/pvd/PxVehiclePvdComponents.h
	${PHYSX_ROOT_DIR}/include/vehicle2/pvd/PxVehiclePvdFunctions.h
	${PHYSX_ROOT_DIR}/include/vehicle2/pvd/PxVehiclePvdHelpers.h
)

SOURCE_GROUP(include FILES ${PHYSX_VEHICLE2_HEADERS})
SOURCE_GROUP(include\\braking FILES ${PHYSX_VEHICLE2_BRAKING_HEADERS})
SOURCE_GROUP(include\\commands FILES ${PHYSX_VEHICLE2_COMMAND_HEADERS})
SOURCE_GROUP(include\\drivetrain FILES ${PHYSX_VEHICLE2_DRIVETRAIN_HEADERS})
SOURCE_GROUP(include\\physxActor FILES ${PHYSX_VEHICLE2_PHYSXACTOR_HEADERS})
SOURCE_GROUP(include\\physxConstraints FILES ${PHYSX_VEHICLE2_PHYSXCONSTRAINT_HEADERS})
SOURCE_GROUP(include\\rigidBody FILES ${PHYSX_VEHICLE2_RIGIDBODY_HEADERS})
SOURCE_GROUP(include\\roadGeometry FILES ${PHYSX_VEHICLE2_ROADGEOMETRY_HEADERS})
SOURCE_GROUP(include\\physxRoadGeometry FILES ${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_HEADERS})
SOURCE_GROUP(include\\steering FILES ${PHYSX_VEHICLE2_STEERING_HEADERS})
SOURCE_GROUP(include\\suspension FILES ${PHYSX_VEHICLE2_SUSPENSION_HEADERS})
SOURCE_GROUP(include\\tire FILES ${PHYSX_VEHICLE2_TIRE_HEADERS})
SOURCE_GROUP(include\\wheel FILES ${PHYSX_VEHICLE2_WHEEL_HEADERS})
SOURCE_GROUP(include\\pvd FILES ${PHYSX_VEHICLE2_PVD_HEADERS})


SET(PHYSX_VEHICLE2_BRAKING_SOURCE
)
SET(PHYSX_VEHICLE2_COMMANDS_SOURCE
	${LL_SOURCE_DIR}/commands/VhCommandHelpers.cpp
)
SET(PHYSX_VEHICLE2_DRIVETRAIN_SOURCE
	${LL_SOURCE_DIR}/drivetrain/VhDrivetrainFunctions.cpp
	${LL_SOURCE_DIR}/drivetrain/VhDrivetrainHelpers.cpp
)
SET(PHYSX_VEHICLE2_PHYSXACTOR_SOURCE
	${LL_SOURCE_DIR}/physxActor/VhPhysXActorFunctions.cpp
	${LL_SOURCE_DIR}/physxActor/VhPhysXActorHelpers.cpp
)
SET(PHYSX_VEHICLE2_PHYSXCONSTRAINT_SOURCE
	${LL_SOURCE_DIR}/physxConstraints/VhPhysXConstraintFunctions.cpp
	${LL_SOURCE_DIR}/physxConstraints/VhPhysXConstraintHelpers.cpp
)
SET(PHYSX_VEHICLE2_PHYSXROADGEOMETRY_SOURCE
	${LL_SOURCE_DIR}/physxRoadGeometry/VhPhysXRoadGeometryFunctions.cpp
	${LL_SOURCE_DIR}/physxRoadGeometry/VhPhysXRoadGeometryHelpers.cpp
)
SET(PHYSX_VEHICLE2_RIGIDBODY_SOURCE
	${LL_SOURCE_DIR}/rigidBody/VhRigidBodyFunctions.cpp
)
SET(PHYSX_VEHICLE2_STEERING_SOURCE
	${LL_SOURCE_DIR}/steering/VhSteeringFunctions.cpp
)
SET(PHYSX_VEHICLE2_SUSPENSION_SOURCE
	${LL_SOURCE_DIR}/suspension/VhSuspensionFunctions.cpp
    ${LL_SOURCE_DIR}/suspension/VhSuspensionHelpers.cpp
)
SET(PHYSX_VEHICLE2_TIRE_SOURCE
	${LL_SOURCE_DIR}/tire/VhTireFunctions.cpp
	${LL_SOURCE_DIR}/tire/VhTireHelpers.cpp
)
SET(PHYSX_VEHICLE2_WHEEL_SOURCE
	${LL_SOURCE_DIR}/wheel/VhWheelFunctions.cpp
)
SET(PHYSX_VEHICLE2_PVD_SOURCE
	${LL_SOURCE_DIR}/pvd/VhPvdAttributeHandles.h
	${LL_SOURCE_DIR}/pvd/VhPvdObjectHandles.h
	${LL_SOURCE_DIR}/pvd/VhPvdHelpers.cpp
	${LL_SOURCE_DIR}/pvd/VhPvdFunctions.cpp
	${LL_SOURCE_DIR}/pvd/VhPvdWriter.cpp
	${LL_SOURCE_DIR}/pvd/VhPvdWriter.h
)

SOURCE_GROUP(src\\braking FILES ${PHYSX_VEHICLE2_BRAKING_SOURCE})
SOURCE_GROUP(src\\commands FILES ${PHYSX_VEHICLE2_COMMANDS_SOURCE})
SOURCE_GROUP(src\\drivetrain FILES ${PHYSX_VEHICLE2_DRIVETRAIN_SOURCE})
SOURCE_GROUP(src\\physxActor FILES ${PHYSX_VEHICLE2_PHYSXACTOR_SOURCE})
SOURCE_GROUP(src\\physxConstraints FILES ${PHYSX_VEHICLE2_PHYSXCONSTRAINT_SOURCE})
SOURCE_GROUP(src\\physxRoadGeometry FILES ${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_SOURCE})
SOURCE_GROUP(src\\rigidBody FILES ${PHYSX_VEHICLE2_RIGIDBODY_SOURCE})
SOURCE_GROUP(src\\steering FILES ${PHYSX_VEHICLE2_STEERING_SOURCE})
SOURCE_GROUP(src\\suspension FILES ${PHYSX_VEHICLE2_SUSPENSION_SOURCE})
SOURCE_GROUP(src\\tire FILES ${PHYSX_VEHICLE2_TIRE_SOURCE})
SOURCE_GROUP(src\\wheel FILES ${PHYSX_VEHICLE2_WHEEL_SOURCE})
SOURCE_GROUP(src\\pvd FILES ${PHYSX_VEHICLE2_PVD_SOURCE})

ADD_LIBRARY(PhysXVehicle2 ${PHYSXVEHICLE2_LIBTYPE}
	${PHYSX_VEHICLE2_BRAKING_SOURCE}
	${PHYSX_VEHICLE2_COMMANDS_SOURCE}
	${PHYSX_VEHICLE2_DRIVETRAIN_SOURCE}
	${PHYSX_VEHICLE2_PHYSXACTOR_SOURCE}
	${PHYSX_VEHICLE2_PHYSXCONSTRAINT_SOURCE}
	${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_SOURCE}
	${PHYSX_VEHICLE2_RIGIDBODY_SOURCE}
	${PHYSX_VEHICLE2_STEERING_SOURCE}
	${PHYSX_VEHICLE2_SUSPENSION_SOURCE}
	${PHYSX_VEHICLE2_TIRE_SOURCE}
	${PHYSX_VEHICLE2_WHEEL_SOURCE}
	${PHYSX_VEHICLE2_PVD_SOURCE}
	${PHYSX_VEHICLE2_HEADERS}
	${PHYSX_VEHICLE2_BRAKING_HEADERS}
	${PHYSX_VEHICLE2_COMMAND_HEADERS}
	${PHYSX_VEHICLE2_DRIVETRAIN_HEADERS}
	${PHYSX_VEHICLE2_PHYSXACTOR_HEADERS}
	${PHYSX_VEHICLE2_PHYSXCONSTRAINT_HEADERS}
	${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_HEADERS}
	${PHYSX_VEHICLE2_RIGIDBODY_HEADERS}
	${PHYSX_VEHICLE2_ROADGEOMETRY_HEADERS}
	${PHYSX_VEHICLE2_STEERING_HEADERS}
	${PHYSX_VEHICLE2_SUSPENSION_HEADERS}
	${PHYSX_VEHICLE2_TIRE_HEADERS}
	${PHYSX_VEHICLE2_WHEEL_HEADERS}
	${PHYSX_VEHICLE2_PVD_HEADERS}
)

INSTALL(FILES ${PHYSX_VEHICLE2_HEADERS} DESTINATION include/vehicle2)
INSTALL(FILES ${PHYSX_VEHICLE2_BRAKING_HEADERS} DESTINATION include/vehicle2/braking)
INSTALL(FILES ${PHYSX_VEHICLE2_COMMAND_HEADERS} DESTINATION include/vehicle2/commands)
INSTALL(FILES ${PHYSX_VEHICLE2_DRIVETRAIN_HEADERS} DESTINATION include/vehicle2/drivetrain)
INSTALL(FILES ${PHYSX_VEHICLE2_PHYSXACTOR_HEADERS} DESTINATION include/vehicle2/physxActor)
INSTALL(FILES ${PHYSX_VEHICLE2_PHYSXCONSTRAINT_HEADERS} DESTINATION include/vehicle2/physxConstraints)
INSTALL(FILES ${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_HEADERS} DESTINATION include/vehicle2/physxRoadGeometry)
INSTALL(FILES ${PHYSX_VEHICLE2_RIGIDBODY_HEADERS} DESTINATION include/vehicle2/rigidBody)
INSTALL(FILES ${PHYSX_VEHICLE2_ROADGEOMETRY_HEADERS} DESTINATION include/vehicle2/roadGeometry)
INSTALL(FILES ${PHYSX_VEHICLE2_STEERING_HEADERS} DESTINATION include/vehicle2/steering)
INSTALL(FILES ${PHYSX_VEHICLE2_SUSPENSION_HEADERS} DESTINATION include/vehicle2/suspension)
INSTALL(FILES ${PHYSX_VEHICLE2_TIRE_HEADERS} DESTINATION include/vehicle2/tire)
INSTALL(FILES ${PHYSX_VEHICLE2_WHEEL_HEADERS} DESTINATION include/vehicle2/wheel)
INSTALL(FILES ${PHYSX_VEHICLE2_PVD_HEADERS} DESTINATION include/vehicle2/pvd)

TARGET_INCLUDE_DIRECTORIES(PhysXVehicle2
	PRIVATE ${PHYSXVEHICLE2_PLATFORM_INCLUDES}
	PRIVATE ${PHYSX_ROOT_DIR}/include
	PRIVATE ${PHYSX_ROOT_DIR}/pvdruntime/include
)

# No linked libraries

# Use generator expressions to set config specific preprocessor definitions
TARGET_COMPILE_DEFINITIONS(PhysXVehicle2
	PRIVATE ${PHYSXVEHICLE2_COMPILE_DEFS}
)

SET_TARGET_PROPERTIES(PhysXVehicle2 PROPERTIES
	OUTPUT_NAME PhysXVehicle2
)


IF(NV_USE_GAMEWORKS_OUTPUT_DIRS AND PHYSXVEHICLE2_LIBTYPE STREQUAL "STATIC")
	SET_TARGET_PROPERTIES(PhysXVehicle2 PROPERTIES
		ARCHIVE_OUTPUT_NAME_DEBUG "PhysXVehicle2_static"
		ARCHIVE_OUTPUT_NAME_CHECKED "PhysXVehicle2_static"
		ARCHIVE_OUTPUT_NAME_PROFILE "PhysXVehicle2_static"
		ARCHIVE_OUTPUT_NAME_RELEASE "PhysXVehicle2_static"
	)
ENDIF()

IF(PHYSXVEHICLE2_COMPILE_PDB_NAME_DEBUG)
	SET_TARGET_PROPERTIES(PhysXVehicle2 PROPERTIES
		COMPILE_PDB_NAME_DEBUG ${PHYSXVEHICLE2_COMPILE_PDB_NAME_DEBUG}
		COMPILE_PDB_NAME_CHECKED ${PHYSXVEHICLE2_COMPILE_PDB_NAME_CHECKED}
		COMPILE_PDB_NAME_PROFILE ${PHYSXVEHICLE2_COMPILE_PDB_NAME_PROFILE}
		COMPILE_PDB_NAME_RELEASE ${PHYSXVEHICLE2_COMPILE_PDB_NAME_RELEASE}
	)
ENDIF()

TARGET_LINK_LIBRARIES(PhysXVehicle2
	PUBLIC ${PHYSXVEHICLE2_PLATFORM_LINKED_LIBS} PhysXFoundation PhysXCommon PhysXCooking PhysX PhysXExtensions
)

IF(PX_GENERATE_SOURCE_DISTRO)
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_BRAKING_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_COMMAND_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_DRIVETRAIN_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXACTOR_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXCONSTRAINT_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_RIGIDBODY_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_ROADGEOMETRY_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_STEERING_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_SUSPENSION_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_TIRE_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_WHEEL_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PVD_HEADERS})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_BRAKING_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_COMMANDS_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_DRIVETRAIN_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXACTOR_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXCONSTRAINT_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PHYSXROADGEOMETRY_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_RIGIDBODY_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_STEERING_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_SUSPENSION_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_TIRE_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_WHEEL_SOURCE})
	LIST(APPEND SOURCE_DISTRO_FILE_LIST ${PHYSX_VEHICLE2_PVD_SOURCE})
ENDIF()

# enable -fPIC so we can link static libs with the editor
SET_TARGET_PROPERTIES(PhysXVehicle2 PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
