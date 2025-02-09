#ifndef H_VULKAN_TYPES
#define H_VULKAN_TYPES

#include "utils/macro_controller.h"
#include "utils/logger.h"
#include <vulkan/vulkan.h>

typedef enum vulkan_stage_error_t {
    VULKAN_NO_ERRORS,
    VULKAN_ERROR_INSTANCE_INIT,
    VULKAN_ERROR_INSTANCE_EXTENSIONS,
    VULKAN_ERROR_INSTANCE_VALIDATION_LAYERS,
    VULKAN_ERROR_INSTANCE_SEMAPHORE_IMAGE_CREATION,
    VULKAN_ERROR_INSTANCE_SEMAPHORE_RENDER_CREATION,
    VULKAN_ERROR_INSTANCE_FENCE_CREATION,
    VULKAN_ERROR_SURFACE_CREATION,
    VULKAN_ERROR_DEVICE_ENUMERATE_COUNT,
    VULKAN_ERROR_DEVICE_ENUMERATE,
    VULKAN_ERROR_DEVICE_ENUMERATE_NO_DEVICES,
    VULKAN_ERROR_DEVICE_NO_MINIMUM_REQUIRED,
    VULKAN_ERROR_DEVICE_CREATION,
    VULKAN_ERROR_DEVICE_NO_DEPTH_FORMAT,
    VULKAN_ERROR_SWAPCHAIN_CREATION,
    VULKAN_ERROR_SWAPCHAIN_IMAGE_VIEW_CREATION,
    VULKAN_ERROR_IMAGE_CREATION,
    VULKAN_ERROR_IMAGE_MEMORY_ALLOCATION,
    VULKAN_ERROR_IMAGE_MEMORY_BIND,
    VULKAN_ERROR_IMAGE_VIEW_CREATION
} vulkan_stage_error_t;

typedef struct vulkan_device_t {
    VkPhysicalDevice graphic_card;
    VkDevice logical_device;
    VkQueue logical_device_queue;
    int graphic_queue_index;
    VkSurfaceFormatKHR swapchain_format;
    VkPresentModeKHR swapchain_presentation_mode;
    uint32_t swapchain_images_count;
    VkSurfaceTransformFlagBitsKHR swapchain_transform;
    VkExtent2D swapchain_extent;
    VkFormat depth_format;
} vulkan_device_t;

typedef struct vulkan_image_2d_t {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    unsigned int width;
    unsigned int height;
} vulkan_image_2d_t;

typedef struct vulkan_swapchain_t {
    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentation_mode;
    uint32_t images_count;
    VkSurfaceTransformFlagBitsKHR transform;
    VkSwapchainKHR swapchain;
    VkImage* images;
    VkImageView* image_views;
    VkExtent2D extent;
    vulkan_image_2d_t depth_image;
} vulkan_swapchain_t;

typedef struct vulkan_renderpass_t {
    int a;
} vulkan_renderpass_t;

typedef struct vulkan_instance_t {
    VkInstance instance;
    VkSurfaceKHR surface;
    vulkan_device_t device;
    vulkan_swapchain_t swapchain;
    VkSemaphore semaphore_image_available;
    VkSemaphore semaphore_render_available;
    VkFence fence_in_flight;
    void* allocator_callback;
    VkResult result;
    vulkan_stage_error_t stage_error;
} vulkan_instance_t;

#endif