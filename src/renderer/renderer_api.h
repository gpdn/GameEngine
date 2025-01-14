#ifndef H_RENDERER_API
#define H_RENDERER_API

#define VK_USE_PLATFORM_WIN32_KHR

#include "utils/macro_controller.h"
#include "utils/logger.h"
#include "utils/file.h" 
#include "platform/platform.h" 
#include "renderer/vertex.h" 
#include <vulkan/vulkan.h>

typedef struct renderer_api_t {
    VkInstance instance;
    void* allocator_callback;
    VkPhysicalDevice graphic_card;
    VkDevice logical_device;
    VkQueue logical_device_queue;
    int graphic_queue_index;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR swapchain_format;
    VkPresentModeKHR swapchain_presentation_mode;
    uint32_t swapchain_images_count;
    VkSurfaceTransformFlagBitsKHR swapchain_transform;
    VkSwapchainKHR swapchain;
    VkImage* swapchain_images;
    VkImageView* swapchain_image_views;
    VkExtent2D swapchain_extent;
    VkPipelineLayout pipeline_layout;
    VkRenderPass render_pass;
    VkPipeline pipeline;
    VkFramebuffer* frame_buffers;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore semaphore_image_available;
    VkSemaphore semaphore_render_available;
    VkFence fence_in_flight;
    VkBuffer vertex_buffer;
} renderer_api_t;

int renderer_api_init(const platform_state_t* const platform_data);
int renderer_api_get_devices();
void renderer_api_wait();
int renderer_api_draw_frame();
int renderer_api_destroy();

#endif