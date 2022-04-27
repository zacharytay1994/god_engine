#include <iostream>

#include <godEngine/godEngine.h>

int main ()
{
	std::cout << "Game entry point!" << std::endl;

	god::godEngine engine;

	engine.Update ();
}