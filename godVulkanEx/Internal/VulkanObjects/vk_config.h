#pragma once
#include "../../godVulkanEx.h"

#include <vector>
#include <memory>

namespace VK_OBJECT
{
	using ConfigInfo = std::vector<char const*>;

	struct Config
	{
		GODVULKANEX_API Config ( bool validation = false , bool renderdoc = false );

		bool Validation () const;
		bool Renderdoc () const;

		ConfigInfo const& GetValidationLayers () const;
		ConfigInfo const& GetInstanceExtensions () const;
		ConfigInfo const& GetDeviceExtensions () const;

	private:
		bool m_validation { false };
		bool m_renderdoc { false };

		ConfigInfo m_validation_layers;
		ConfigInfo m_instance_extensions;
		ConfigInfo m_device_extensions;
	};

	using ConfigRef = std::shared_ptr<Config>;
}