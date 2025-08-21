#pragma once
#include "VKBase+.h"

#include <vector>

class VContext;

/**
* A structure that points to a part of a buffer
*/
struct VBufferSection
{
	VkBuffer handle = VK_NULL_HANDLE; 
	VkDeviceSize offset = 0;
	VkDeviceSize size = 0;

	VBufferSection() = default;

	VBufferSection(const vulkan::buffer& buf, VkDeviceSize offset, VkDeviceSize size)
		: handle(buf),  // 假设 buffer 有获取句柄的方法
		offset(offset),
		size(size) {
	}
};

struct VMeshPart
{
	// todo: separate mesh part with material?
	// (material as another global storage??)
	VBufferSection vertex_buffer_section = {};
	VBufferSection index_buffer_section = {};
	VBufferSection material_uniform_buffer_section = {};
	size_t index_count = 0;
	vulkan::descriptorSet material_descriptor_set = {};  // TODO: I still need a per-instance descriptor set

	vulkan::imageView albedo_map = {};
	vulkan::imageView normal_map = {};



	VMeshPart(const VBufferSection& vertex_buffer_section, const VBufferSection& index_buffer_section, size_t index_count)
		: vertex_buffer_section(vertex_buffer_section)
		, index_buffer_section(index_buffer_section)
		, index_count(index_count)
	{
	}
};

/**
* A class to manage resources of a static mesh.
* Must be destructed before the vk::Device used to construct it
*/
class VModel
{
public:
	VModel() = default;
	~VModel() = default;
	VModel(VModel&&) = default;
	VModel& operator= (VModel&&) = default;

	const std::vector<VMeshPart>& getMeshParts() const
	{
		return mesh_parts;
	}

	static VModel loadModelFromFile(const VContext& vulkan_context, const std::string& path
		, const vulkan::sampler& texture_sampler, const vulkan::descriptorPool& descriptor_pool,
		const vulkan::descriptorSetLayout& material_descriptor_set_layout);

	VModel(const VModel&) = delete;
	VModel& operator= (const VModel&) = delete;

private:
	vulkan::vertexBuffer vertex_buffer;
	vulkan::indexBuffer index_buffer;
	vulkan::uniformBuffer uniform_buffer;
	std::vector<vulkan::image> images;
	std::vector<vulkan::imageView> imageviews;
	std::vector<vulkan::imageMemory> image_memories; //TODO: use a single memory, or two

	std::vector<VMeshPart> mesh_parts;

};

