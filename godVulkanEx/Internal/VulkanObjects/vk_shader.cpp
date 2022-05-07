#include "vk_shader.h"

#include <fstream>
#include <iostream>

namespace VK_OBJECT
{
	Shader::Shader ( VkDevice vkLogicalDevice , SHADER_TYPE type , char const* spvFile )
		:
		m_logical_device ( vkLogicalDevice ) ,
		m_type ( type )
	{
		ReadFile ( spvFile );
		CreateShaderModule ( vkLogicalDevice );
	}

	Shader::~Shader ()
	{
		vkDestroyShaderModule ( m_logical_device , m_shader_module , nullptr );
	}

	VkShaderModule Shader::GetShaderModule ()
	{
		return m_shader_module;
	}

	VkPipelineShaderStageCreateInfo Shader::GetStageCreateInfo ()
	{
		VkPipelineShaderStageCreateInfo stage_create_info {};
		if ( m_type == SHADER_TYPE::VERTEX )
		{
			stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		}
		else
		{
			stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info.module = m_shader_module;
		stage_create_info.pName = "main";
		stage_create_info.pSpecializationInfo = nullptr;  // used to optimize constant variables

		return stage_create_info;
	}

	void Shader::ReadFile ( char const* spvFile )
	{
		std::ifstream file ( spvFile , std::ios::ate | std::ios::binary );

		if ( !file.is_open () )
		{
			throw std::runtime_error ( "[ERROR!] VK_OBJECT::Shader::ReadFile - Failed to open file!" );
		}

		size_t fileSize = ( size_t ) file.tellg ();
		m_shader_code.resize ( fileSize );

		file.seekg ( 0 );
		file.read ( m_shader_code.data () , fileSize );

		file.close ();

		std::cout << "[SUCCESS] VK_OBJECT::Shader::ReadFile - File read success!" << std::endl;
	}

	void Shader::CreateShaderModule ( VkDevice vkLogicalDevice )
	{
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_shader_code.size ();
		createInfo.pCode = reinterpret_cast< uint32_t const* >( m_shader_code.data () );

		if ( vkCreateShaderModule ( vkLogicalDevice , &createInfo , nullptr , &m_shader_module ) != VK_SUCCESS )
		{
			throw std::runtime_error ( "[ERROR!] VK_OBJECT::Shader::CreateShaderModule - Failed to create shader module!" );
		}
		std::cout << "[SUCCESS] VK_OBJECT::Shader::CreateShaderModule - Shader module create success!" << std::endl;
	}
}