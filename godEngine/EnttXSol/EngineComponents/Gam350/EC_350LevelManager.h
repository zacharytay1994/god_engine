#pragma once

#include "../EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct _350LevelManager
	{
		std::string m_level_names_serialize;
		std::vector<std::string> m_level_names;
		std::vector<uint32_t> m_level_ids;

		glm::vec3 m_offset_direction { 20.0f,-10.0f,0.0f };
		glm::vec3 m_offset { 0.0f };

		uint32_t m_currently_initializing { 0 };
		uint32_t m_currently_playing { 0 };
		bool m_initializing { false };
		bool m_done { false };

		bool operator==( _350LevelManager const& rhs )
		{
			( rhs );
			return true;
		}
	};
	template <>
	inline void NewLuaType<_350LevelManager> ( sol::state& luaState , std::string const& name )
	{
		RegisterLuaType<_350LevelManager> ( luaState , name ,
			"done" , &_350LevelManager::m_done );
	}
	template<>
	inline void ComponentInspector::operator() < _350LevelManager > ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources )
	{
		RegisterInspector<_350LevelManager , EngineResources> ( entity , registry , imguiUniqueID , editorResources ,
			[]( _350LevelManager& component , EngineResources& resources )
			{
				( resources );
				ImGui::Separator ();
				ImGui::Text ( "_350 Level Manager" );
				ImGui::Separator ();

				ImGui::InputText ( ": Levels" , &component.m_level_names_serialize );
			} );
	}

	template<>
	inline void JSONify<_350LevelManager> ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , _350LevelManager& component )
	{
		( engineResources );
		// serialize
		RapidJSON::JSONifyToValue ( value , document , "levels" , component.m_level_names_serialize );
	}

	template<>
	inline void DeJSONify<_350LevelManager> ( EngineResources& engineResources , _350LevelManager& component , rapidjson::Value& jsonObj )
	{
		( engineResources );
		// deserialize
		AssignIfExist ( jsonObj , component.m_level_names_serialize , "levels" );
	}
}