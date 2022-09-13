#pragma once

namespace god
{
	struct PhysicSettings
	{
		float		m_fixedDT = 1.f / 60.f;
		glm::vec3	m_gravity = { 0.0f, -9.81f, 0.0f };
		glm::vec3	m_worldMin = glm::vec3(-100.0f);
		glm::vec3	m_worldMax = glm::vec3(100.0f);
		uint32_t	m_worldSubdivisions = 2;

		bool		DebugModeOn = true;

		//settings set to this below
		//BroadphaseAlgorithm = BroadphaseType::AutomaticBoxPrune;
		//FrictionModel = FrictionType::Patch;
		//DebugType = DebugType::LiveDebug
		
	};
}