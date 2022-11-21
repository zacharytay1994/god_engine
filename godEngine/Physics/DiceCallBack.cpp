#include "../pch.h"
#include "DiceCallBack.h"


namespace god
{
	Sound diceSFX;
	glm::vec3 prevPos;

	void DiceCallBack(glm::vec3 const& pos)
	{
		AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\Dice Hit.wav", diceSFX);
		std::cout << "Dice contacts: " << pos << std::endl;
		
		if (prevPos != pos)
		{
			AudioAPI::PlaySound(diceSFX);
			prevPos = pos;
		}
		else
		{
			AudioAPI::StopSound(diceSFX.m_channel);
		}

	}

	void ExamplePairCallBack(glm::vec3 const& pos, glm::vec3 const& pos1)
	{
		//std::cout << "Pair contacts: " << pos <<", " <<pos1<< std::endl;
	}

	void DiceTriggerCallBack(glm::vec3 const& pos)
	{
		std::cout << "Dice triggered: " << pos << std::endl;
	}

	void ExampleTriggerPairCallBack(glm::vec3 const& pos, glm::vec3 const& pos1)
	{
		std::cout << "Pair triggered: " << pos << ", " << pos1 << std::endl;
	}
}