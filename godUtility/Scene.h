#pragma once
#include "godUtility.h"
#include "Internal/RecycleVector.h"

#include <glm/glm/glm.hpp>

#include <vector>
#include <queue>
#include <unordered_map>

namespace god
{
	using SceneObjectID = uint32_t;

	struct Scene
	{
		struct InstancedRenderData
		{
			size_t		operator()( InstancedRenderData const& data ) const;
			bool		operator==( InstancedRenderData const& rhs ) const;
			uint32_t	m_model_id { 0 };
			uint32_t	m_diffuse_id { 0 };
			uint32_t	m_specular_id { 0 };
			float		m_shininess { 0.0f };
			float		m_emissive{ 1.0f };
		};

		struct PointLightData
		{
			glm::vec3 m_position;
			glm::vec3 m_colour;
			glm::vec3 m_ambient;
			glm::vec3 m_diffuse;
			glm::vec3 m_specular;

			GODUTILITY_API PointLightData& operator=( PointLightData const& rhs );
		};

		struct DirectionalLightData
		{
			glm::vec3 m_position;
			glm::vec3 m_colour;
			glm::vec3 m_ambient;
			glm::vec3 m_diffuse;
			glm::vec3 m_specular;

			GODUTILITY_API DirectionalLightData& operator=( DirectionalLightData const& rhs );
		};

		std::unordered_map<InstancedRenderData , std::vector<glm::mat4> , InstancedRenderData> m_instanced_render_data;
		std::unordered_map<InstancedRenderData , std::vector<glm::mat4> , InstancedRenderData> m_2D_instanced_render_data;
		std::unordered_map<InstancedRenderData , std::vector<glm::mat4> , InstancedRenderData> m_characters;
		std::unordered_map<InstancedRenderData , std::vector<glm::mat4> , InstancedRenderData> m_billboard_sprites;
		std::vector<std::tuple<InstancedRenderData , glm::mat4>> m_animation_data;

		std::vector<PointLightData> m_point_light_data;
		std::vector<DirectionalLightData> m_directional_light_data;


		GODUTILITY_API		Scene ();
		GODUTILITY_API void	AddInstancedObject ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void	Add2DInstancedObject ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void AddCharacter ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void AddBillboard ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void AddAnimation ( InstancedRenderData const& data , glm::mat4 const& transform );
		GODUTILITY_API void	ClearInstancedScene ();

		GODUTILITY_API void AddPointLight ( PointLightData const& pld );
		GODUTILITY_API void AddDirectionalLight ( DirectionalLightData  const& dld );
	};
}