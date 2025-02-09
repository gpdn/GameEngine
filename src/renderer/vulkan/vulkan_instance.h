#ifndef H_VULKAN_INSTANCE
#define H_VULKAN_INSTANCE

#define VK_USE_PLATFORM_WIN32_KHR

#include "utils/macro_controller.h"
#include "utils/logger.h"
#include "utils/file.h"
#include "platform/platform.h" 
#include <vulkan/vulkan.h>
#include "vulkan_types.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

vulkan_instance_t vulkan_init(const platform_state_t* const platform_data);
int vulkan_destroy(vulkan_instance_t* instance);

#endif