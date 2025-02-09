#include "vulkan_swapchain.h"

vulkan_swapchain_t vulkan_swapchain_create(vulkan_instance_t* api_data) {

    vulkan_device_get_surface_info(api_data, &api_data->device);

    vulkan_swapchain_t swapchain;

    swapchain.format = api_data->device.swapchain_format;
    swapchain.images_count = api_data->device.swapchain_images_count;
    swapchain.presentation_mode = api_data->device.swapchain_presentation_mode;
    swapchain.transform = api_data->device.swapchain_transform;
    swapchain.extent = api_data->device.swapchain_extent;
    swapchain.images = NULL;
    swapchain.image_views = NULL;

    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = api_data->surface;
    createInfo.pNext = NULL,
    createInfo.minImageCount = swapchain.images_count;
    createInfo.imageFormat = swapchain.format.format;
    createInfo.imageColorSpace = swapchain.format.colorSpace;
    createInfo.imageExtent = swapchain.extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapchain.presentation_mode;
    createInfo.preTransform = swapchain.transform;
    createInfo.clipped = VK_TRUE;
    //createInfo.oldSwapchain = api_data.swapchain;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.flags = 0;

    VkSwapchainKHR new_swapchain;

    if((api_data->result = vkCreateSwapchainKHR(api_data->device.logical_device, &createInfo, NULL, &new_swapchain)) != VK_SUCCESS) {
       api_data->stage_error = VULKAN_ERROR_SWAPCHAIN_CREATION;
       LOG_WARNING("Failed to create swap chain");
       return swapchain;
    }

    vkGetSwapchainImagesKHR(api_data->device.logical_device, new_swapchain, &swapchain.images_count, NULL);

    swapchain.images = (VkImage*)malloc(sizeof(VkImage) * swapchain.images_count);
    swapchain.image_views = (VkImageView*)malloc(sizeof(VkImageView) * swapchain.images_count);

    vkGetSwapchainImagesKHR(api_data->device.logical_device, new_swapchain, &swapchain.images_count, swapchain.images);

    for(uint32_t i = 0; i < swapchain.images_count; ++i) {
        VkImageViewCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchain.images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchain.format.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.pNext = NULL;
        createInfo.flags = 0;

        if((api_data->result = vkCreateImageView(api_data->device.logical_device, &createInfo, NULL, swapchain.image_views + i)) != VK_SUCCESS) {
            api_data->stage_error = VULKAN_ERROR_SWAPCHAIN_IMAGE_VIEW_CREATION;
            LOG_DEBUG("Failed to create image views");
            break;
        }
    }

    swapchain.depth_image = vulkan_image_2d_create(
        api_data,
        VK_IMAGE_TYPE_2D,
        swapchain.extent.width,
        swapchain.extent.height,
        api_data->device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        1,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    return swapchain;
}

int vulkan_swapchain_recreate(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain) {
    vkDeviceWaitIdle(api_data->device.logical_device);

    vulkan_swapchain_destroy(api_data, swapchain);

    return vulkan_create_swapchain();
    //&& renderer_api_create_framebuffers();
}

int vulkan_swapchain_present(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain, uint32_t image_index) {
    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &api_data->semaphore_render_available;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchain->swapchain;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = NULL;
    presentInfo.pNext = NULL;

    VkResult presentation_result = vkQueuePresentKHR(api_data->device.logical_device_queue, &presentInfo);

    if(presentation_result == VK_ERROR_OUT_OF_DATE_KHR || presentation_result == VK_SUBOPTIMAL_KHR) return vulkan_recreate_swapchain(api_data, swapchain);

    return 1;
}

inline int vulkan_swapchain_get_image(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain, uint32_t* image_index) {
    VkResult acquisition_result = vkAcquireNextImageKHR(api_data->device.logical_device, swapchain->swapchain, UINT64_MAX, api_data->semaphore_image_available, VK_NULL_HANDLE, image_index);
    if(acquisition_result == VK_ERROR_OUT_OF_DATE_KHR || acquisition_result == VK_SUBOPTIMAL_KHR) return renderer_api_recreate_swapchain();
    return 1;
}

int vulkan_swapchain_destroy(vulkan_instance_t* api_data, vulkan_swapchain_t* swapchain) {
    for(uint32_t i = 0; i < swapchain->images_count; ++i) {
        vkDestroyImageView(api_data->device.logical_device, swapchain->image_views[i], NULL);
    }
    vulkan_image_2d_destroy(api_data, &swapchain->depth_image);
    /* for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        vkDestroyFramebuffer(api_data.logical_device, api_data.frame_buffers[i], NULL);
    } */
    vkDestroySwapchainKHR(api_data->device.logical_device, swapchain->swapchain, NULL);

    //free(api_data.frame_buffers);
    free(swapchain->images);
    free(swapchain->image_views);

    return 1;
}