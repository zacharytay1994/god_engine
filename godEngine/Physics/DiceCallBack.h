#pragma once
#include "PhysicUtils.h"
#include <vector>
namespace god
{

	void DiceCallBack(glm::vec3 const& pos);


	void ExamplePairCallBack(glm::vec3 const& pos, glm::vec3 const& pos1);




	// Will be called every time the Entity has Contact
	// EntityData.m_id ,  Callback function( position of the entt) 
	static std::vector< std::pair<uint32_t, void (*)(glm::vec3 const& pos)> > ContactCallBack
	{
		{41,DiceCallBack},{48,DiceCallBack},{104,DiceCallBack}
	};


	// Will be called every time the pair of Entities has Contact
	// EntityData.m_id 0 , EntityData.m_id 1,  Callback function( position of entt 0 ,position of entt 1 ) 
	static std::vector< std::tuple<uint32_t, uint32_t, void (*)(glm::vec3 const& pos, glm::vec3 const& pos1)> > ContactCallBackPair
	{
		
		{0, 1, ExamplePairCallBack}
	};

}
