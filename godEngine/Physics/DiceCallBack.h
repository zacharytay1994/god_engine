#pragma once
#include "PhysicUtils.h"
#include "Types.h"
#include "../Audio/AudioAPI.h"

#include <vector>
namespace god
{

	void DiceCallBack(glm::vec3 const& pos);

	void ExamplePairCallBack(glm::vec3 const& pos, glm::vec3 const& pos1);

	void DiceTriggerCallBack(glm::vec3 const& pos);

	void ExampleTriggerPairCallBack(glm::vec3 const& pos, glm::vec3 const& pos1);


	// Will be called every frame as long as the pair of Entities has Contact
	// Requires 2 Simulation Shape
	// PhysicsTypes id ,  Callback function( position of the entt) 
	static std::vector< std::pair<PhysicsTypes::physicstype, void (*)(glm::vec3 const& pos)> > ContactCallBack
	{
		{PhysicsTypes::Dice , DiceCallBack}
	};


	// Will be called every frame as long as the pair of Entities has Contact
	// Requires 2 Simulation Shape
	// PhysicsTypes id 0 , PhysicsTypes id 1,  Callback function( position of entt 0 ,position of entt 1 ) 
	static std::vector< std::tuple<PhysicsTypes::physicstype, PhysicsTypes::physicstype, void (*)(glm::vec3 const& pos, glm::vec3 const& pos1)> > ContactCallBackPair
	{
		{PhysicsTypes::Bullet, PhysicsTypes::Player, ExamplePairCallBack}
	};

	// Will be called once upon contact and once again upon no contact
	// Requires 1 Trigger Shape and 1 Simulation Shape
	// PhysicsTypes id ,  Callback function( position of the entt) 
	static std::vector< std::pair<PhysicsTypes::physicstype, void (*)(glm::vec3 const& pos)> > TriggerCallBack
	{
		{PhysicsTypes::Dice , DiceTriggerCallBack}
	};


	// Will be called once upon contact and once again upon no contact
	// Requires 1 Trigger Shape and 1 Simulation Shape
	// (Trigger Shape)PhysicsTypes id 0 , (Simulation Shape)PhysicsTypes id 1,  Callback function( position of entt 0 ,position of entt 1 ) 
	static std::vector< std::tuple<PhysicsTypes::physicstype, PhysicsTypes::physicstype, void (*)(glm::vec3 const& pos, glm::vec3 const& pos1)> >TriggerCallBackPair
	{

		{PhysicsTypes::Bullet, PhysicsTypes::Player, ExampleTriggerPairCallBack}
	};

}
