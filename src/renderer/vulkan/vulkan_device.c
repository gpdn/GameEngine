#include "vulkan_device.h"

/* int renderer_api_get_devices() {
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
} */