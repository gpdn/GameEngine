#ifndef H_VULKAN_DEVICE
#define H_VULKAN_DEVICE

#include <vulkan/vulkan.h>

typedef struct vulkan_device_t {
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

#endif