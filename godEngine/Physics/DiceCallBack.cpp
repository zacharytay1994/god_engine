#include "../pch.h"
#include "DiceCallBack.h"


namespace god
{
	


	void DiceCallBack(glm::vec3 const& pos)
	{
		std::cout << "Dice contacts: " << pos << std::endl;
	}


}