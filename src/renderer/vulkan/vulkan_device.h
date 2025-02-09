#ifndef H_VULKAN_DEVICE
#define H_VULKAN_DEVICE

#include <vulkan/vulkan.h>
#include "vulkan_types.h"

int vulkan_device_init(vulkan_instance_t* api_data);
VkPhysicalDevice* vulkan_get_devices(vulkan_instance_t* api_data, uint32_t* devices_count);
void vulkan_device_get_surface_info(vulkan_instance_t* api_data, vulkan_device_t* device);
void vulkan_device_find_memory_index(vulkan_instance_t* api_data, vulkan_device_t* device);
int vulkan_device_wait(vulkan_device_t* device);
int vulkan_device_destroy(vulkan_device_t* device);

#endif