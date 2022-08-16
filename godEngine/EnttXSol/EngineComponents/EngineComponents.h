#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <entt.hpp>
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_stdlib.h"
#include "../../Editor/EngineResources.h"
#include <godUtility/Internal/RapidJSONWrapper.h>

#include <tuple>
#include <string>
#include <array>

namespace god
{
	// for engine component bindings to lua
	template <typename T>
	void NewLuaType ( sol::state& luaState , std::string const& name ) {}

	// for engine component to imgui inspector
	struct ComponentInspector
	{
		template<typename T>
		void operator()( entt::entity entity , entt::registry& registry , int& imguiUniqueID , EngineResources& editorResources ) {}
	};

	// for serializing an engine component to json
	template <typename T>
	void JSONify ( EngineResources& engineResources , rapidjson::Document& document , rapidjson::Value& value , T& component ) {}

	template <typename T>
	void Assign ( T& val , rapidjson::Value& jsonObj , std::string const& name ) {};

	template<>
	inline void Assign<int> ( int& val , rapidjson::Value& jsonObj , std::string const& name )
	{
		val = jsonObj[ name.c_str () ].GetInt ();
	}

	template<>
	inline void Assign<float> ( float& val , rapidjson::Value& jsonObj , std::string const& name )
	{
		val = jsonObj[ name.c_str () ].GetFloat ();
	}

	template<>
	inline void Assign<uint32_t> ( uint32_t& val , rapidjson::Value& jsonObj , std::string const& name )
	{
		val = jsonObj[ name.c_str () ].GetUint ();
	}

	template<typename T>
	void AssignIfExist ( rapidjson::Value& jsonObj , T& val , std::string const& name )
	{
		if ( jsonObj.HasMember ( name.c_str () ) )
		{
			Assign ( val , jsonObj , name );
		}
	}

	template <typename T>
	void DeJSONify ( EngineResources& engineResources , T& component , rapidjson::Value& jsonObj ) {}

	template<typename T , typename ...Args>
	void RegisterLuaType ( sol::state& luaState , std::string const& name , Args... args )
	{
		luaState.new_usertype<T> ( name , sol::constructors<T ()> () , args... );
	}

	template<typename T , typename U>
	void RegisterInspector ( entt::entity entity , entt::registry& registry , int& imguiUniqueID , U& editorResources , void( *gui )( T& , U& ) )
	{
		if ( registry.any_of<T> ( entity ) )
		{
			ImGui::PushID ( imguiUniqueID++ );
			auto& component = registry.get<T> ( entity );
			gui ( component , editorResources );
			ImGui::PopID ();
		}
	}


}