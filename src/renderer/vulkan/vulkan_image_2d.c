#include "vulkan_image_2d.h"

#define VULKAN_IMAGE_MIPMAPS 4
#define VULKAN_IMAGE_SAMPLES 1

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
) {
    vulkan_image_2d_t image;

    image.width = width;
    image.height = height;

    VkImageCreateInfo image_info;
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = VULKAN_IMAGE_MIPMAPS;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VULKAN_IMAGE_SAMPLES;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.pNext = NULL;
    image_info.flags = 0;

    if(api_data->result = vkCreateImage(api_data->device.logical_device, &image_info, api_data->allocator_callback, &image) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_IMAGE_CREATION;
        return image;
    }

    VkMemoryRequirements image_memory;
    vkGetImageMemoryRequirements(api_data->device.logical_device, &image, &image_memory);

    VkMemoryAllocateInfo memory_info;
    memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_info.allocationSize = image_memory.size;
    memory_info.pNext = NULL;
    memory_info.memoryTypeIndex = vulkan_device_find_memory_index(api_data, &api_data->device, image_memory.memoryTypeBits, memory_flags);

    if((api_data->result = vkAllocateMemory(api_data->device.logical_device, &memory_info, api_data->allocator_callback, 0)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_IMAGE_MEMORY_ALLOCATION;
        return image;
    }
    
    if((api_data->result = vkBindImageMemory(api_data->device.logical_device, &image.image, &image.memory, 0)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_IMAGE_MEMORY_BIND;
        return image;
    }

    image.view = NULL;
    if(create_view) {
        vulkan_image_2d_create_view(api_data, format, aspect_flags, &image);
    }

    return image;
}

int vulkan_image_2d_create_view(
    vulkan_instance_t* api_data,  
    VkFormat format, 
    VkImageAspectFlags aspect_flags,
    vulkan_image_2d_t* image
) {
    VkImageViewCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspect_flags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    if((api_data->result = vkCreateImageView(api_data->device.logical_device, &createInfo, NULL, &image->view)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_IMAGE_VIEW_CREATION;
        LOG_DEBUG("Failed to create image views");
        return 0;
    }

    return 1;
}

int vulkan_image_2d_destroy(vulkan_instance_t* api_data, vulkan_image_2d_t* image) {
    if(image->view) {
        vkDestroyImageView(api_data->device.logical_device, image->view, api_data->allocator_callback);
    }
    vkFreeMemory(api_data->device.logical_device, image->memory, api_data->allocator_callback);
    vkDestroyImage(api_data->device.logical_device, image->image, api_data->allocator_callback);
}

#undef VULKAN_IMAGE_MIPMAPS
#undef VULKAN_IMAGE_SAMPLES