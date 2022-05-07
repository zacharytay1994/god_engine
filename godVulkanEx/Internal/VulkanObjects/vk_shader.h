#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VK_OBJECT
{
	enum class SHADER_TYPE
	{
		VERTEX = 0 ,
		FRAGMENT
	};

	struct Shader
	{
		Shader ( VkDevice vkLogicalDevice , SHADER_TYPE type , char const* spvFile );
		~Shader ();

		VkShaderModule					GetShaderModule ();
		VkPipelineShaderStageCreateInfo GetStageCreateInfo ();

	private:
		VkDevice			m_logical_device;
		std::vector<char>	m_shader_code;
		VkShaderModule		m_shader_module;
		SHADER_TYPE			m_type;

		void ReadFile ( char const* spvFile );
		void CreateShaderModule ( VkDevice vkLogicalDevice );
	};
}