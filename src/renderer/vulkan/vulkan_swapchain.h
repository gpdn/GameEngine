#ifndef H_VULKAN_SWAPCHAIN
#define H_VULKAN_SWAPCHAIN

#include <vulkan/vulkan.h>
#include "vulkan_types.h"
#include "vulkan_device.h"
#include "vulkan_image_2d.h"

vulkan_swapchain_t vulkan_swapchain_create(vulkan_instance_t* api_data);
int vulkan_swapchain_get_image(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain, uint32_t* image_index);
int vulkan_swapchain_recreate(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain);
int vulkan_swapchain_present(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain, uint32_t image_index);
int vulkan_swapchain_destroy(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain);

#endif