#include "renderer_api.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>

static renderer_api_t api_data;

static int renderer_api_check_extensions(const char** required_extensions);
static int renderer_api_check_layers(const char* required_layer);
static int renderer_api_evaluate_device(VkPhysicalDevice* device);
static int renderer_api_create_device();
static int renderer_api_create_surface(const platform_state_t* const platform_data);
static int renderer_api_create_swapchain();
static inline int renderer_api_create_shader(VkShaderModule* module, const char* file_path);
static int renderer_api_create_render_pass();
static int renderer_api_create_pipeline();
static int renderer_api_create_framebuffers();
static int renderer_api_create_command_pool();
static int renderer_api_create_command_buffers();
static inline int renderer_api_record_command_buffers(uint32_t image_index);
static int renderer_api_init_semaphores();
static int renderer_api_init_fences();
int renderer_api_draw_frame();
static inline void renderer_api_destroy_swapchain();
static inline int renderer_api_recreate_swapchain();
static inline int renderer_api_create_vertex_buffer();

static vertex_t vertices[3] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

static inline int renderer_api_create_vertex_buffer() {
    VkBufferCreateInfo bufferInfo;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    bufferInfo.pNext = NULL;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = NULL;

    if (vkCreateBuffer(api_data.logical_device, &bufferInfo, NULL, &api_data.vertex_buffer) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create vertex buffer");
        return 0;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(api_data.logical_device, api_data.vertex_buffer, &memRequirements);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(api_data.graphic_card, &memProperties);

    return 1;
}

inline void renderer_api_wait() {
    vkDeviceWaitIdle(api_data.logical_device);
}

static inline int renderer_api_recreate_swapchain() {
    vkDeviceWaitIdle(api_data.logical_device);

    renderer_api_destroy_swapchain();

    return renderer_api_create_swapchain() && renderer_api_create_framebuffers();
}

int renderer_api_draw_frame() {
    vkWaitForFences(api_data.logical_device, 1, &api_data.fence_in_flight, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult acquisition_result = vkAcquireNextImageKHR(api_data.logical_device, api_data.swapchain, UINT64_MAX, api_data.semaphore_image_available, VK_NULL_HANDLE, &imageIndex);

    if(acquisition_result == VK_ERROR_OUT_OF_DATE_KHR || acquisition_result == VK_SUBOPTIMAL_KHR) return renderer_api_recreate_swapchain();

    vkResetFences(api_data.logical_device, 1, &api_data.fence_in_flight);

    vkResetCommandBuffer(api_data.command_buffer, 0);

    renderer_api_record_command_buffers(imageIndex);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &api_data.semaphore_image_available;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &api_data.command_buffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &api_data.semaphore_render_available;

    if(vkQueueSubmit(api_data.logical_device_queue, 1, &submitInfo, api_data.fence_in_flight) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to submit draw command buffer");
        return 0;
    }

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &api_data.semaphore_render_available;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &api_data.swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;
    presentInfo.pNext = NULL;

    VkResult presentation_result = vkQueuePresentKHR(api_data.logical_device_queue, &presentInfo);

    if(presentation_result == VK_ERROR_OUT_OF_DATE_KHR || presentation_result == VK_SUBOPTIMAL_KHR) return renderer_api_recreate_swapchain();

    return 1;
}

static int renderer_api_check_extensions(const char** required_extensions) {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);

    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

    #if DEBUG_EXTENSIONS_LIST
        LOG_DEBUG("%s\n", "Extensions");
        for(uint32_t i = 0; i < extensionCount; ++i) {
            LOG_DEBUG("%s\n", extensions[i].extensionName);
        }
    #endif

    int extensions_found[2] = {0, 0};

    for(uint32_t i = 0; i < extensionCount; ++i) {
        for(int j = 0; j < 2; ++j) {
            if(strlen(extensions[i].extensionName) == strlen(required_extensions[j]) && strcmp(extensions[i].extensionName, required_extensions[j]) == 0) {
                extensions_found[j] = 1;
            }
        }
    }

    free(extensions);

    for(int i = 0; i < 2; ++i) {
        if(extensions_found[i] == 0) return 0;
    }

    return 1;

}

static int renderer_api_check_layers(const char* required_layer) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    #if DEBUG_LAYERS_LIST
        LOG_DEBUG("Layers: %u\n", layerCount);
    #endif

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    size_t required_layer_length = strlen(required_layer); 

    int layer_found = 0;

    for(uint32_t i = 0; i < layerCount; ++i) {
        #if DEBUG_LAYERS_LIST
            LOG_DEBUG("%s\n", availableLayers[i].layerName);
        #endif
        if(strlen(availableLayers[i].layerName) == required_layer_length && strcmp(availableLayers[i].layerName, required_layer) == 0) {
            layer_found = 1;
        }
    }

    return layer_found;
}

static int renderer_api_evaluate_device(VkPhysicalDevice* device) {    
    int device_score = 0;

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, NULL);

    VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, extensions);

    const char* required_device_extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    size_t required_device_extension_length = strlen(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    #if DEBUG_DEVICES_LIST
        LOG_DEBUG("%s\n", "Device extensions");
    #endif 

    for(uint32_t i = 0; i < extensionCount; ++i) {

        #if DEBUG_DEVICES_LIST
            LOG_DEBUG("%s\n", extensions[i].extensionName);
        #endif 

        if(strlen(extensions[i].extensionName) == required_device_extension_length && strcmp(extensions[i].extensionName, required_device_extension) == 0) {
            device_score += 1;
        }
    }

    if(device_score == 0) {
        #if DEBUG_DEVICES_LIST
            LOG_DEBUG("%s\n", "Required device extension not supported.");
        #endif 
        return 0;
    }

    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, api_data.surface, &surface_capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*device, api_data.surface, &formatCount, NULL);

    if(formatCount == 0) {
        LOG_DEBUG("%s\n", "No available formats for device.");
        return 0;
    }

    VkSurfaceFormatKHR* device_formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*device, api_data.surface, &formatCount, device_formats);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*device, api_data.surface, &presentModeCount, NULL);

    if(presentModeCount == 0) {
        LOG_DEBUG("%s\n", "No available presentations for device.");
        return 0;
        free(device_formats);
    }

    VkPresentModeKHR* device_presentations = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(*device, api_data.surface, &presentModeCount, device_presentations);

    int device_format_found = 0;
    int device_presentation_found = 0;

    for(uint32_t i = 0; i < formatCount; ++i) {
        if(device_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && device_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            device_format_found = 1;
            break;
        }
    }

    for(uint32_t i = 0; i < presentModeCount; ++i) {
        if(device_presentations[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            device_presentation_found = 1;
            break;
        }
    }

    free(device_formats);
    free(device_presentations);

    if(device_format_found == 0) return 0;
    if(device_presentation_found == 0) return 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(*device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(*device, &deviceFeatures);

    #if DEBUG_DEVICES_LIST
        LOG_DEBUG("%s\n", deviceProperties.deviceName);
    #endif 

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) device_score += 1000;

    device_score += deviceProperties.limits.maxImageDimension2D;

    if(!deviceFeatures.geometryShader) return 0;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, queueFamilies);

    int required_queue_family_found = 0;

    for(uint32_t i = 0; i < queueFamilyCount; ++i) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {

            VkBool32 presentSupport = 0;
            vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, api_data.surface, &presentSupport);

            if(presentSupport) {
                required_queue_family_found = 1;
            }
            break;
        }
    }

    free(queueFamilies);

    if(required_queue_family_found == 0) {
        LOG_DEBUG("%s\n", "Required queue families not found on device");
        return 0;
    }

    return device_score;
}

static int renderer_api_create_device() {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(api_data.graphic_card, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(api_data.graphic_card, &queueFamilyCount, queueFamilies);

    api_data.graphic_queue_index = -1;

    for(uint32_t i = 0; i < queueFamilyCount; ++i) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            
            VkBool32 presentSupport = 0;
            vkGetPhysicalDeviceSurfaceSupportKHR(api_data.graphic_card, i, api_data.surface, &presentSupport);

            if(presentSupport) {
                api_data.graphic_queue_index = i;
            }
            break;
        }
    }

    LOG_DEBUG("Queue family index: %d\n", api_data.graphic_queue_index);

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = api_data.graphic_queue_index;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pNext = NULL;
    queueCreateInfo.flags = 0;

    float queue_priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures deviceFeatures;
    memset(&deviceFeatures, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = device_extensions;

    if(vkCreateDevice(api_data.graphic_card, &createInfo, NULL, &api_data.logical_device) != VK_SUCCESS) {
        LOG_WARNING("%s\n", "Failed to create logical device");
        return 0;
    }

    vkGetDeviceQueue(api_data.logical_device, api_data.graphic_queue_index, 0, &api_data.logical_device_queue);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(api_data.graphic_card, api_data.surface, &formatCount, NULL);

    VkSurfaceFormatKHR* device_formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(api_data.graphic_card, api_data.surface, &formatCount, device_formats);

    for(uint32_t i = 0; i < formatCount; ++i) {
        if(device_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && device_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            api_data.swapchain_format = device_formats[i];
            break;
        }
    }

    free(device_formats);

    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(api_data.graphic_card, api_data.surface, &surface_capabilities);

    uint32_t images_count = 3;

    if(surface_capabilities.minImageCount > images_count) images_count = surface_capabilities.minImageCount;

    if(surface_capabilities.maxImageCount > 0 && images_count < surface_capabilities.maxImageCount) {
        images_count = surface_capabilities.maxImageCount;
    }

    api_data.swapchain_presentation_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    api_data.swapchain_images_count = images_count;
    api_data.swapchain_transform = surface_capabilities.currentTransform;
    api_data.swapchain = VK_NULL_HANDLE;
    api_data.swapchain_images = NULL;

    return renderer_api_create_swapchain();
}


static int renderer_api_create_surface(const platform_state_t* const platform_data) {
    #if PLATFORM_WINDOWS
        
        LOG_DEBUG("%s\n", "Creating surface for windows");

        VkWin32SurfaceCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.hinstance = platform_data->instance;
        createInfo.hwnd = platform_data->window;

        if(vkCreateWin32SurfaceKHR(api_data.instance, &createInfo, NULL, &api_data.surface) != VK_SUCCESS) {
            LOG_DEBUG("%s\n", "Failed to create renderer api window");
            return 0;
        }

        return 1;
    #endif

    return 0;
}


static int renderer_api_create_swapchain() {

    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(api_data.graphic_card, api_data.surface, &surface_capabilities);

    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = api_data.surface;
    createInfo.pNext = NULL,
    createInfo.minImageCount = api_data.swapchain_images_count;
    createInfo.imageFormat = api_data.swapchain_format.format;
    createInfo.imageColorSpace = api_data.swapchain_format.colorSpace;
    createInfo.imageExtent = surface_capabilities.currentExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = api_data.swapchain_presentation_mode;
    createInfo.preTransform = api_data.swapchain_transform;
    createInfo.clipped = VK_TRUE;
    //createInfo.oldSwapchain = api_data.swapchain;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.flags = 0;

    VkSwapchainKHR new_swapchain;

    if(vkCreateSwapchainKHR(api_data.logical_device, &createInfo, NULL, &new_swapchain) != VK_SUCCESS) {
       LOG_WARNING("Failed to create swap chain");
       return 0;
    }

    uint32_t swapchain_images_count;
    vkGetSwapchainImagesKHR(api_data.logical_device, new_swapchain, &swapchain_images_count, NULL);

    api_data.swapchain_images = (VkImage*)malloc(sizeof(VkImage) * swapchain_images_count);
    api_data.swapchain_image_views = (VkImageView*)malloc(sizeof(VkImageView) * swapchain_images_count);

    vkGetSwapchainImagesKHR(api_data.logical_device, new_swapchain, &swapchain_images_count, api_data.swapchain_images);

    int swapchain_images_created = 1;

    for(uint32_t i = 0; i < swapchain_images_count; ++i) {
        VkImageViewCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = api_data.swapchain_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = api_data.swapchain_format.format;
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

        if(vkCreateImageView(api_data.logical_device, &createInfo, NULL, api_data.swapchain_image_views + i) != VK_SUCCESS) {
            LOG_DEBUG("Failed to create image views");
            swapchain_images_created = 0;
            break;
        }
    }

    api_data.swapchain_extent = surface_capabilities.currentExtent;
    api_data.swapchain = new_swapchain;

    return swapchain_images_created;

}

static inline int renderer_api_create_shader(VkShaderModule* module, const char* file_path) {
    size_t file_size = 0; 

    const char* code = read_file_no_terminator(file_path, &file_size);

    if(code == NULL) return 0;

    VkShaderModuleCreateInfo createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.codeSize = file_size;
    createInfo.pCode = (const uint32_t*)code;
    
    if(vkCreateShaderModule(api_data.logical_device, &createInfo, NULL, module) != VK_SUCCESS) return 0;
    return 1;
}

static int renderer_api_create_render_pass() {
    VkAttachmentDescription colorAttachment;
    colorAttachment.format = api_data.swapchain_format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachment.flags = 0;

    VkAttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = NULL;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;

    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.flags = 0;
    renderPassInfo.pNext = NULL;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(api_data.logical_device, &renderPassInfo, NULL, &api_data.render_pass) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to generate render pass");
        return 0;
    }

    return 1;
}

static int renderer_api_create_pipeline() {
    VkShaderModule vertex_shader;
    renderer_api_create_shader(&vertex_shader, "vert.spv");

    VkShaderModule fragment_shader;
    renderer_api_create_shader(&fragment_shader, "frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertex_shader;
    vertShaderStageInfo.pName = "main";
    vertShaderStageInfo.flags = 0;
    vertShaderStageInfo.pSpecializationInfo = NULL;
    vertShaderStageInfo.pNext = NULL;

    VkPipelineShaderStageCreateInfo fragmShaderStageInfo;
    fragmShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmShaderStageInfo.module = fragment_shader;
    fragmShaderStageInfo.pName = "main";
    fragmShaderStageInfo.flags = 0;
    fragmShaderStageInfo.pSpecializationInfo = NULL;
    fragmShaderStageInfo.pNext = NULL;

    VkPipelineShaderStageCreateInfo pipeline_stages[] = {
        vertShaderStageInfo,
        fragmShaderStageInfo
    };

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState;
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    dynamicState.flags = 0;
    dynamicState.pNext = NULL;

    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(vertex_t);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributes_description[2];
    attributes_description[0].binding = 0;
    attributes_description[0].location = 0;
    attributes_description[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributes_description[0].offset = offsetof(vertex_t, position);
    
    attributes_description[1].binding = 0;
    attributes_description[1].location = 1;
    attributes_description[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributes_description[1].offset = offsetof(vertex_t, color);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = attributes_description;
    vertexInputInfo.flags = 0;
    vertexInputInfo.pNext = NULL;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    inputAssembly.flags = 0;
    inputAssembly.pNext = NULL;

    /* 
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) api_data.swapchain_extent.width;
    viewport.height = (float) api_data.swapchain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = api_data.swapchain_extent;
    */

    VkPipelineViewportStateCreateInfo viewportState;
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    viewportState.flags = 0;
    viewportState.pNext = NULL;
    viewportState.pScissors = NULL;
    viewportState.pViewports = NULL;

    VkPipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    rasterizer.lineWidth = 1.0f;
    rasterizer.flags = 0;
    rasterizer.pNext = NULL;

    VkPipelineMultisampleStateCreateInfo multisampling;
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = NULL;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    multisampling.flags = 0;
    multisampling.pNext = NULL;

    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending;
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    colorBlending.flags = 0;
    colorBlending.pNext = NULL;

    VkPipelineLayout pipelineLayout;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = NULL;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.pNext = NULL;

    if(vkCreatePipelineLayout(api_data.logical_device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create pipeline layout");
        return 0;
    }

    api_data.pipeline_layout = pipelineLayout;

    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = pipeline_stages;
    pipelineInfo.flags = 0;
    pipelineInfo.pNext = NULL;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pTessellationState = NULL;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = NULL;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = api_data.render_pass;
    pipelineInfo.subpass = 0;

    if(vkCreateGraphicsPipelines(api_data.logical_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &api_data.pipeline) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Faild to create pipeline");
        return 0;
    }

    vkDestroyShaderModule(api_data.logical_device, vertex_shader, NULL);
    vkDestroyShaderModule(api_data.logical_device, fragment_shader, NULL);

    return 1;
}

static int renderer_api_create_framebuffers() {
    VkFramebuffer* framebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * api_data.swapchain_images_count);

    for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        VkFramebufferCreateInfo framebufferInfo;
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = api_data.render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = api_data.swapchain_image_views + i;
        framebufferInfo.width = api_data.swapchain_extent.width;
        framebufferInfo.height = api_data.swapchain_extent.height;
        framebufferInfo.layers = 1;
        framebufferInfo.flags = 0;
        framebufferInfo.pNext = NULL;

        if(vkCreateFramebuffer(api_data.logical_device, &framebufferInfo, NULL, framebuffers + i) != VK_SUCCESS) {
            LOG_DEBUG("%s\n", "Failed to create framebuffer");
            free(framebuffers);
            return 0;
        }
    }

    api_data.frame_buffers = framebuffers;

    return 1;    
}

static int renderer_api_create_command_pool() {
    VkCommandPoolCreateInfo poolInfo;
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = api_data.graphic_queue_index;
    poolInfo.pNext = NULL;

    if(vkCreateCommandPool(api_data.logical_device, &poolInfo, NULL, &api_data.command_pool) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create command pool");
        return 0;
    }

    return 1;
}

static int renderer_api_create_command_buffers() {
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = api_data.command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.pNext = NULL;

    if(vkAllocateCommandBuffers(api_data.logical_device, &allocInfo, &api_data.command_buffer) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to allocate command buffers");
        return 0;
    }
    
    return 1;
}

static inline int renderer_api_record_command_buffers(uint32_t image_index) {
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;
    beginInfo.pNext = NULL;

    if(vkBeginCommandBuffer(api_data.command_buffer, &beginInfo) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to begin recording command buffer");
        return 0;
    }

    VkRenderPassBeginInfo renderPassInfo;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = api_data.render_pass;
    renderPassInfo.framebuffer = api_data.frame_buffers[image_index];
    renderPassInfo.pNext = NULL;
    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = api_data.swapchain_extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 0.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(api_data.command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(api_data.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, api_data.pipeline);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)api_data.swapchain_extent.width;
    viewport.height = (float)api_data.swapchain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(api_data.command_buffer, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = api_data.swapchain_extent;
    vkCmdSetScissor(api_data.command_buffer, 0, 1, &scissor);

    vkCmdDraw(api_data.command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(api_data.command_buffer);

    if(vkEndCommandBuffer(api_data.command_buffer) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to record command buffer");
        return 0;
    }

    return 1;
}

static int renderer_api_init_semaphores() {
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = NULL;
    semaphoreInfo.flags = 0;

    if (vkCreateSemaphore(api_data.logical_device, &semaphoreInfo, NULL, &api_data.semaphore_image_available) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create semaphores");
        return 0;
    }

    if (vkCreateSemaphore(api_data.logical_device, &semaphoreInfo, NULL, &api_data.semaphore_render_available) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create semaphores");
        return 0;
    }

    return 1;
}

static int renderer_api_init_fences() {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(api_data.logical_device, &fenceInfo, NULL, &api_data.fence_in_flight) != VK_SUCCESS) {
        LOG_DEBUG("%s\n", "Failed to create fences");
        return 0;
    }

    return 1;
}

static int renderer_create_instance() {
    api_data.allocator_callback = NULL;

    const char* validation_layers  = "VK_LAYER_KHRONOS_validation";
    const char* required_extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};

    if(!renderer_api_check_extensions(required_extensions)) {
        LOG_WARNING("The extensions required are not suported\n");
        return 0;
    }

    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Game Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Game Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
    appInfo.pNext = NULL;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    
    createInfo.enabledExtensionCount = 2;
    createInfo.ppEnabledExtensionNames = (const char *const *)required_extensions;
    
    #if DEBUG_VALIDATION_ENABLED
        if(renderer_api_check_layers(validation_layers)) {
            const char* instance_validation_layers[] = {validation_layers}; 

            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = (const char *const *)instance_validation_layers;
        } else {
            LOG_WARNING("%s\n", "Validation layer not found");
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = NULL;
        }
    #else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = NULL;
    #endif
    

    if(vkCreateInstance(&createInfo, NULL, &api_data.instance) != VK_SUCCESS) {
        LOG_DEBUG("Failed to initialise rendered api\n");
        return 0;
    }

    return 1;
}

int renderer_api_init(const platform_state_t* const platform_data) {

    if(!renderer_create_instance()) return 0;

    renderer_api_create_surface(platform_data);

    if(!renderer_api_get_devices()) return 0;
    if(!renderer_api_create_device()) return 0;
    if(!renderer_api_create_render_pass()) return 0;
    if(!renderer_api_create_pipeline()) return 0;
    if(!renderer_api_create_framebuffers()) return 0;
    if(!renderer_api_create_command_pool()) return 0;
    if(!renderer_api_create_command_buffers()) return 0;
    if(!renderer_api_init_semaphores()) return 0;
    if(!renderer_api_init_fences()) return 0;
    if(!renderer_api_create_vertex_buffer()) return 0;

    return 1;
}

int renderer_api_get_devices() {
    uint32_t devices_count;

    if(vkEnumeratePhysicalDevices(api_data.instance, &devices_count, NULL) != VK_SUCCESS) return 0;

    LOG_DEBUG("Devices found: %u\n", devices_count);

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * devices_count);

    if(vkEnumeratePhysicalDevices(api_data.instance, &devices_count, devices) != VK_SUCCESS) { 
        free(devices);
        return 0;
    }
    
    if(devices_count == 0) { 
        free(devices);
        LOG_ERROR("%s\n", "No devices found");
        return 0;
    }

    VkPhysicalDevice selected_device = VK_NULL_HANDLE;
    int device_score = 0;

    for(uint32_t i = 0; i < devices_count; ++i) {
        LOG_DEBUG("Device %d\n", i);

        int new_score = renderer_api_evaluate_device(devices + i);

        #if DEBUG_DEVICES_LIST
            LOG_DEBUG("Device Score: %d\n", new_score);
        #endif

        if(new_score > device_score) {
            selected_device = devices[i];
            device_score = new_score; 
        }
    }

    if(selected_device == VK_NULL_HANDLE) {
        free(devices);
        LOG_ERROR("%s\n", "No devices has the minimum required properties");
        return 0;
    }

    #if DEBUG_DEVICES_LIST
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(selected_device, &deviceProperties);
        LOG_DEBUG("Selected device: %s\n", deviceProperties.deviceName);
    #endif

    api_data.graphic_card = selected_device;

    free(devices);

    return 1;
}

static inline void renderer_api_destroy_swapchain() {
    for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        vkDestroyImageView(api_data.logical_device, api_data.swapchain_image_views[i], NULL);
    }
    for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        vkDestroyFramebuffer(api_data.logical_device, api_data.frame_buffers[i], NULL);
    }
    vkDestroySwapchainKHR(api_data.logical_device, api_data.swapchain, NULL);

    free(api_data.frame_buffers);
    free(api_data.swapchain_images);
    free(api_data.swapchain_image_views);
}

int renderer_api_destroy() {    
    vkDeviceWaitIdle(api_data.logical_device);

    vkDestroyBuffer(api_data.logical_device, api_data.vertex_buffer, NULL);
    vkDestroySemaphore(api_data.logical_device, api_data.semaphore_image_available, NULL);
    vkDestroySemaphore(api_data.logical_device, api_data.semaphore_render_available, NULL);
    vkDestroyFence(api_data.logical_device, api_data.fence_in_flight, NULL);
    vkDestroyCommandPool(api_data.logical_device, api_data.command_pool, NULL);
    for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        vkDestroyFramebuffer(api_data.logical_device, api_data.frame_buffers[i], NULL);
    }
    vkDestroyPipeline(api_data.logical_device, api_data.pipeline, NULL);
    vkDestroyPipelineLayout(api_data.logical_device, api_data.pipeline_layout, NULL);
    vkDestroyRenderPass(api_data.logical_device, api_data.render_pass, NULL);
    for(uint32_t i = 0; i < api_data.swapchain_images_count; ++i) {
        vkDestroyImageView(api_data.logical_device, api_data.swapchain_image_views[i], NULL);
    }
    vkDestroySwapchainKHR(api_data.logical_device, api_data.swapchain, NULL);
    vkDestroyDevice(api_data.logical_device, NULL);
    vkDestroySurfaceKHR(api_data.instance, api_data.surface, NULL);
    vkDestroyInstance(api_data.instance, NULL);

    free(api_data.frame_buffers);
    free(api_data.swapchain_images);
    free(api_data.swapchain_image_views);

    return 1;
}