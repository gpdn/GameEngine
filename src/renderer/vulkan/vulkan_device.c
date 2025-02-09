#include "vulkan_device.h"

static int vulkan_evaluate_device(vulkan_instance_t* api_data, VkPhysicalDevice* device);
static int vulkan_create_device(vulkan_instance_t* api_data, vulkan_device_t* device);
static inline int vulkan_device_init_depth_format(vulkan_instance_t* api_data, vulkan_device_t* device);

int vulkan_device_init(vulkan_instance_t* api_data) {
    if(!vulkan_pick_device(api_data, &api_data->device)) return 0;
    if(!vulkan_create_device(api_data, &api_data->device)) return 0;
    if(!vulkan_device_init_depth_format(api_data, &api_data->device)) return 0;
    return 1;
}

VkPhysicalDevice* vulkan_get_devices(vulkan_instance_t* api_data, uint32_t* devices_count) {
    if((api_data->result = vkEnumeratePhysicalDevices(api_data->instance, &devices_count, NULL)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_DEVICE_ENUMERATE_COUNT;
    };

    LOG_DEBUG("Devices found: %u\n", devices_count);

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * (*devices_count));

    if((api_data->result = vkEnumeratePhysicalDevices(api_data->instance, &devices_count, devices)) != VK_SUCCESS) { 
        api_data->stage_error = VULKAN_ERROR_DEVICE_ENUMERATE;
        free(devices);
        return NULL;
    }
    
    if(devices_count == 0) { 
        free(devices);
        api_data->stage_error = VULKAN_ERROR_DEVICE_ENUMERATE_NO_DEVICES;
        LOG_ERROR("%s\n", "No devices found");
        return NULL;
    }

    return devices;
}

static int vulkan_evaluate_device(vulkan_instance_t* api_data, VkPhysicalDevice* device) {
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
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, api_data->surface, &surface_capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*device, api_data->surface, &formatCount, NULL);

    if(formatCount == 0) {
        LOG_DEBUG("%s\n", "No available formats for device.");
        return 0;
    }

    VkSurfaceFormatKHR* device_formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*device, api_data->surface, &formatCount, device_formats);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*device, api_data->surface, &presentModeCount, NULL);

    if(presentModeCount == 0) {
        LOG_DEBUG("%s\n", "No available presentations for device.");
        return 0;
        free(device_formats);
    }

    VkPresentModeKHR* device_presentations = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(*device, api_data->surface, &presentModeCount, device_presentations);

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
            vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, api_data->surface, &presentSupport);

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

int vulkan_pick_device(vulkan_instance_t* api_data, vulkan_device_t* device) {
    uint32_t devices_count;

    VkPhysicalDevice* devices = vulkan_get_devices(api_data, &devices_count);

    VkPhysicalDevice selected_device = VK_NULL_HANDLE;
    int device_score = 0;

    for(uint32_t i = 0; i < devices_count; ++i) {
        LOG_DEBUG("Device %d\n", i);

        int new_score = vulkan_evaluate_device(api_data, devices + i);

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
        api_data->stage_error = VULKAN_ERROR_DEVICE_NO_MINIMUM_REQUIRED;
        LOG_ERROR("%s\n", "No devices has the minimum required properties");
        return 0;
    }

    #if DEBUG_DEVICES_LIST
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(selected_device, &deviceProperties);
        LOG_DEBUG("Selected device: %s\n", deviceProperties.deviceName);
    #endif

    device->graphic_card = selected_device;

    free(devices);

    return 1;
}

static int vulkan_create_device(vulkan_instance_t* api_data, vulkan_device_t* device) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->graphic_card, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device->graphic_card, &queueFamilyCount, queueFamilies);

    device->graphic_queue_index = -1;

    for(uint32_t i = 0; i < queueFamilyCount; ++i) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            
            VkBool32 presentSupport = 0;
            vkGetPhysicalDeviceSurfaceSupportKHR(device->graphic_card, i, api_data->surface, &presentSupport);

            if(presentSupport) {
                device->graphic_queue_index = i;
            }
            break;
        }
    }

    LOG_DEBUG("Queue family index: %d\n", device->graphic_queue_index);

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = device->graphic_queue_index;
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

    if((api_data->result = vkCreateDevice(device->graphic_card, &createInfo, NULL, &device->logical_device)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_DEVICE_CREATION;
        LOG_WARNING("%s\n", "Failed to create logical device");
        return 0;
    }

    vkGetDeviceQueue(device->logical_device, device->graphic_queue_index, 0, &device->logical_device_queue);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->graphic_card, api_data->surface, &formatCount, NULL);

    VkSurfaceFormatKHR* device_formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->graphic_card, api_data->surface, &formatCount, device_formats);

    for(uint32_t i = 0; i < formatCount; ++i) {
        if(device_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && device_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            device->swapchain_format = device_formats[i];
            break;
        }
    }

    vulkan_device_get_surface_info(api_data, &device);

    free(device_formats);

    return 1;
}

inline void vulkan_device_get_surface_info(vulkan_instance_t* api_data, vulkan_device_t* device) {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->graphic_card, api_data->surface, &surface_capabilities);

    uint32_t images_count = 3;

    if(surface_capabilities.minImageCount > images_count) images_count = surface_capabilities.minImageCount;

    if(surface_capabilities.maxImageCount > 0 && images_count < surface_capabilities.maxImageCount) {
        images_count = surface_capabilities.maxImageCount;
    }

    device->swapchain_presentation_mode = VK_PRESENT_MODE_MAILBOX_KHR;
    device->swapchain_images_count = images_count;
    device->swapchain_transform = surface_capabilities.currentTransform;
    device->swapchain_extent = surface_capabilities.currentExtent;

}

inline int vulkan_device_init_depth_format(vulkan_instance_t* api_data, vulkan_device_t* device) {
    VkFormat formats[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    uint32_t flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for(size_t i = 0; i < sizeof(formats)/sizeof(formats[0]); ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->graphic_card, formats[i], &properties);

        if((properties.linearTilingFeatures & flags) == flags || (properties.optimalTilingFeatures & flags) == flags) {
            device->depth_format = formats[i];
            return 1;
        }
    }

    api_data->stage_error = VULKAN_ERROR_DEVICE_NO_DEPTH_FORMAT;

    return 0;
}

inline int vulkan_device_find_memory_index(vulkan_instance_t* api_data, vulkan_device_t* device, uint32_t type, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties device_memory;
    vkGetPhysicalDeviceMemoryProperties(device->graphic_card, &device_memory);

    for(uint32_t i = 0; i < device_memory.memoryTypeCount; i++) {
        if ((type & (1 << i)) && (device_memory.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return -1;
}

inline int vulkan_device_wait(vulkan_device_t* device) {
    vkDeviceWaitIdle(device->logical_device);
}

inline int vulkan_device_destroy(vulkan_device_t* device) {
    vkDestroyDevice(device->logical_device, NULL);
}