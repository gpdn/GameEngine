#ifndef H_VULKAN_IMAGE_2D
#define H_VULKAN_IMAGE_2D

#include "utils/macro_controller.h"
#include "utils/logger.h"
#include "utils/file.h"
#include "platform/platform.h" 
#include <vulkan/vulkan.h>
#include "vulkan_types.h"
#include "vulkan_device.h"

vulkan_image_2d_t vulkan_image_2d_create(
    vulkan_instance_t* api_data, 
    VkImageType type, 
    unsigned int width, 
    unsigned int height, 
    VkFormat format, 
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryAllocateFlags memory_flags,
    VkImageAspectFlags aspect_flags,
    int create_view
);
int vulkan_image_2d_create_view(
    vulkan_instance_t* api_data, 
    VkImageType type, 
    unsigned int width, 
    unsigned int height, 
    VkFormat format, 
    VkImageTiling tiling, 
    VkMemoryAllocateFlags memory_flags,
    VkImageAspectFlags aspect_flags,
    int create_view
);
int vulkan_image_2d_destroy(vulkan_instance_t* api_data, vulkan_image_2d_t* image);

#endif