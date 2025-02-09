#include "vulkan_instance.h"

static int vulkan_check_extensions(const char** required_extensions);
static int vulkan_check_layers(const char* required_layer);
static int vulkan_create_surface(vulkan_instance_t* api_data, const platform_state_t* const platform_data);
static int vulkan_init_semaphores(vulkan_instance_t* api_data);
static int vulkan_init_fences(vulkan_instance_t* api_data);

vulkan_instance_t vulkan_init(const platform_state_t* const platform_data) {

    vulkan_instance_t api_data;

    if(!vulkan_create_instance(&api_data)) return api_data;

    vulkan_create_surface(&api_data, platform_data);
    vulkan_device_init(&api_data);
    
    api_data.swapchain = vulkan_swapchain_create(&api_data);

    if(!vulkan_init_semaphores(&api_data)) return api_data;
    if(!vulkan_init_fences(&api_data)) return api_data;

    /* 
    if(!renderer_api_create_render_pass()) return 0;
    if(!renderer_api_create_pipeline()) return 0;
    if(!renderer_api_create_framebuffers()) return 0;
    if(!renderer_api_create_command_pool()) return 0;
    if(!renderer_api_create_command_buffers()) return 0;
    if(!renderer_api_create_vertex_buffer()) return 0; */

    return api_data;
}

static int vulkan_create_surface(vulkan_instance_t* api_data, const platform_state_t* const platform_data) {
    #if PLATFORM_WINDOWS
        
        LOG_DEBUG("%s\n", "Creating surface for windows");

        VkWin32SurfaceCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.hinstance = platform_data->instance;
        createInfo.hwnd = platform_data->window;

        if((api_data->result = vkCreateWin32SurfaceKHR(api_data->instance, &createInfo, NULL, &api_data->surface)) != VK_SUCCESS) {
            LOG_DEBUG("%s\n", "Failed to create renderer api window");
            api_data->stage_error = VULKAN_ERROR_SURFACE_CREATION;
            return 0;
        }

        return 1;
    #endif

    return 0;
}

static int vulkan_check_extensions(const char** required_extensions) {
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

static int vulkan_check_layers(const char* required_layer) {
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

static int vulkan_create_instance(vulkan_instance_t* api_data) {
    api_data->allocator_callback = NULL;

    const char* validation_layers  = "VK_LAYER_KHRONOS_validation";
    const char* required_extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};

    if(!vulkan_check_extensions(required_extensions)) {
        LOG_WARNING("The extensions required are not suported\n");
        api_data->stage_error = VULKAN_ERROR_INSTANCE_EXTENSIONS;
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
        if(vulkan_check_layers(validation_layers)) {
            const char* instance_validation_layers[] = {validation_layers}; 

            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = (const char *const *)instance_validation_layers;
        } else {
            LOG_WARNING("%s\n", "Validation layer not found");
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = NULL;
            api_data->stage_error = VULKAN_ERROR_INSTANCE_VALIDATION_LAYERS;
        }
    #else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = NULL;
    #endif
    

    if((api_data->result = vkCreateInstance(&createInfo, NULL, &api_data->instance)) != VK_SUCCESS) {
        LOG_DEBUG("Failed to initialise rendered api\n");
        api_data->stage_error = VULKAN_ERROR_INSTANCE_INIT;
        return 0;
    }

    return 1;
}

static int vulkan_init_semaphores(vulkan_instance_t* api_data) {
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = NULL;
    semaphoreInfo.flags = 0;

    if((api_data->result = vkCreateSemaphore(api_data->device.logical_device, &semaphoreInfo, NULL, &api_data->semaphore_image_available)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_INSTANCE_SEMAPHORE_IMAGE_CREATION;
        LOG_DEBUG("%s\n", "Failed to create semaphores");
        return 0;
    }

    if((api_data->result = vkCreateSemaphore(api_data->device.logical_device, &semaphoreInfo, NULL, &api_data->semaphore_render_available)) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_INSTANCE_SEMAPHORE_RENDER_CREATION;
        LOG_DEBUG("%s\n", "Failed to create semaphores");
        return 0;
    }

    return 1;
}

static int vulkan_init_fences(vulkan_instance_t* api_data) {
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(api_data->device.logical_device, &fenceInfo, NULL, &api_data->fence_in_flight) != VK_SUCCESS) {
        api_data->stage_error = VULKAN_ERROR_INSTANCE_FENCE_CREATION;
        LOG_DEBUG("%s\n", "Failed to create fences");
        return 0;
    }

    return 1;
}

int vulkan_destroy(vulkan_instance_t* api_data) {

    vulkan_device_wait(&api_data->device);

    //vkDestroyBuffer(api_data.logical_device, api_data.vertex_buffer, NULL);

    vkDestroySemaphore(api_data->device.logical_device, api_data->semaphore_image_available, NULL);
    vkDestroySemaphore(api_data->device.logical_device, api_data->semaphore_render_available, NULL);
    vkDestroyFence(api_data->device.logical_device, api_data->fence_in_flight, NULL);

    /*
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

    */

    vulkan_swapchain_destroy(&api_data, &api_data->swapchain);
    vulkan_device_destroy(&api_data->device);
    vkDestroySurfaceKHR(api_data->instance, api_data->surface, NULL);
    vkDestroyInstance(api_data->instance, NULL);

    return 1;
}