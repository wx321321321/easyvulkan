#pragma once
#include "EasyVKStart.h"
#define DestroyHandleBy(Func) if (handle) { Func(graphicsBase::Base().Device(), handle, nullptr); handle = VK_NULL_HANDLE; }
#define MoveHandle handle = other.handle; other.handle = VK_NULL_HANDLE;
#define DefineHandleTypeOperator operator decltype(handle)() const { return handle; }
#define DefineAddressFunction const decltype(handle)* Address() const { return &handle; }
#define ExecuteOnce(...) { static bool executed = false; if (executed) return __VA_ARGS__; executed = true; }
#ifndef NDEBUG
#define ENABLE_DEBUG_MESSENGER true
#else
#define ENABLE_DEBUG_MESSENGER false
#endif
//定义vulkan命名空间，之后会把Vulkan中一些基本对象的封装写在其中
namespace vulkan {

    

    /*constexpr struct outStream_t {
        static std::stringstream ss;
        struct returnedStream_t {
            returnedStream_t operator<<(const std::string& string) const {
                ss << string;
                return {};
            }
        };
        returnedStream_t operator<<(const std::string& string) const {
            ss.clear();
            ss << string;
            return {};
        }
    } outStream;*/

   // inline  std::stringstream outStream_t::ss;

    constexpr VkExtent2D defaultWindowSize = { 1280, 720 };
    
   

    class graphicsBase {

        //实例相关
        uint32_t apiVersion = VK_API_VERSION_1_0;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        std::vector<VkPhysicalDevice> availablePhysicalDevices;
        
        //设备相关
        VkDevice device;
        uint32_t queueFamilyIndex_graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_compute = VK_QUEUE_FAMILY_IGNORED;
        VkQueue queue_graphics;
        VkQueue queue_presentation;
        VkQueue queue_compute;

        //表面相关
        VkSurfaceKHR surface;
        std::vector <VkSurfaceFormatKHR> availableSurfaceFormats;

        //交换链相关
        VkSwapchainKHR swapchain;
        std::vector <VkImage> swapchainImages;
        std::vector <VkImageView> swapchainImageViews;
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

        //扩展和层
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
        std::vector<const char*> deviceExtensions;

        VkDebugUtilsMessengerEXT debugMessenger;

        //静态变量
        static graphicsBase singleton;
        //构造移动析构函数
        graphicsBase() = default;
        graphicsBase(graphicsBase&&) = delete;
        ~graphicsBase() {
            if (!instance)
                return;
            if (device) {
                WaitIdle();
                if (swapchain) {
                    for (auto& i : callbacks_destroySwapchain)
                    {
                        i();
                    }
                    for (auto& i : swapchainImageViews)
                    {
                        if (i)
                        {
                            vkDestroyImageView(device, i, nullptr);
                        }
                    }
                    vkDestroySwapchainKHR(device, swapchain, nullptr);
                }
                for (auto& i : callbacks_destroyDevice)
                {
                    i();
                }
                vkDestroyDevice(device, nullptr);
            }
            if (surface)
            {
                vkDestroySurfaceKHR(instance, surface, nullptr);
            }
            if (debugMessenger) {
                {
                    PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger =
                        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
                    {
                        if (DestroyDebugUtilsMessenger)
                            DestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
                    }
                }
            }
            vkDestroyInstance(instance, nullptr);
        }
        

        //构造信使
        VkResult CreateDebugMessenger() {
            static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback = [](
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData)->VkBool32 {
                    outStream << std::format("{}\n\n", pCallbackData->pMessage);
                    return VK_FALSE;
                };
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity =
                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType =
                     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = DebugUtilsMessengerCallback
            };
            PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
            if (vkCreateDebugUtilsMessenger) {
                VkResult result = vkCreateDebugUtilsMessenger(instance, &debugUtilsMessengerCreateInfo, nullptr, &debugMessenger);
                if (result)
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to create a debug messenger!\nError code: {}\n", int32_t(result));
                return result;
            }
            outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the function pointer of vkCreateDebugUtilsMessengerEXT!\n");
            return VK_RESULT_MAX_ENUM;
        }
//回调函数相关
    private:
        std::vector<void(*)()> callbacks_createSwapchain;
        std::vector<void(*)()> callbacks_destroySwapchain;
        std::vector<void(*)()> callbacks_createDevice;
        std::vector<void(*)()> callbacks_destroyDevice;

    public:
        void AddCallback_CreateSwapchain(void(*function)()) {
            callbacks_createSwapchain.push_back(function);
        }
        void AddCallback_DestroySwapchain(void(*function)()) {
            callbacks_destroySwapchain.push_back(function);
        }
        void AddCallback_CreateDevice(void(*function)()) {
            callbacks_createDevice.push_back(function);
        }
        void AddCallback_DestroyDevice(void(*function)()) {
            callbacks_destroyDevice.push_back(function);
        }
        void Terminate() {
            this->~graphicsBase();
            instance = VK_NULL_HANDLE;
            physicalDevice = VK_NULL_HANDLE;
            device = VK_NULL_HANDLE;
            surface = VK_NULL_HANDLE;
            swapchain = VK_NULL_HANDLE;
            swapchainImages.resize(0);
            swapchainImageViews.resize(0);
            swapchainCreateInfo = {};
            debugMessenger = VK_NULL_HANDLE;
        }




        //Getter
        //实例相关信息存储接口
        uint32_t ApiVersion() const {
            return apiVersion;
        }
        VkInstance Instance() const {
            return instance;
        }
        VkPhysicalDevice PhysicalDevice() const {
            return physicalDevice;
        }
        const VkPhysicalDeviceProperties& PhysicalDeviceProperties() const {
            return physicalDeviceProperties;
        }
        const VkPhysicalDeviceMemoryProperties& PhysicalDeviceMemoryProperties() const {
            return physicalDeviceMemoryProperties;
        }
        VkPhysicalDevice AvailablePhysicalDevice(uint32_t index) const {
            return availablePhysicalDevices[index];
        }
        uint32_t AvailablePhysicalDeviceCount() const {
            return uint32_t(availablePhysicalDevices.size());
        }

        //设备相关信息存储接口
        VkDevice Device() const {
            return device;
        }
        uint32_t QueueFamilyIndex_Graphics() const {
            return queueFamilyIndex_graphics;
        }
        uint32_t QueueFamilyIndex_Presentation() const {
            return queueFamilyIndex_presentation;
        }
        uint32_t QueueFamilyIndex_Compute() const {
            return queueFamilyIndex_compute;
        }
        VkQueue Queue_Graphics() const {
            return queue_graphics;
        }
        VkQueue Queue_Presentation() const {
            return queue_presentation;
        }
        VkQueue Queue_Compute() const {
            return queue_compute;
        }

        //表面相关信息存储接口
        VkSurfaceKHR Surface() const {
            return surface;
        }
        const VkFormat& AvailableSurfaceFormat(uint32_t index) const {
            return availableSurfaceFormats[index].format;
        }
        const VkColorSpaceKHR& AvailableSurfaceColorSpace(uint32_t index) const {
            return availableSurfaceFormats[index].colorSpace;
        }
        uint32_t AvailableSurfaceFormatCount() const {
            return uint32_t(availableSurfaceFormats.size());
        }

        //交换链相关信息存储接口
        VkSwapchainKHR Swapchain() const {
            return swapchain;
        }
        VkImage SwapchainImage(uint32_t index) const {
            return swapchainImages[index];
        }
        VkImageView SwapchainImageView(uint32_t index) const {
            return swapchainImageViews[index];
        }
        uint32_t SwapchainImageCount() const {
            return uint32_t(swapchainImages.size());
        }
        const VkSwapchainCreateInfoKHR& SwapchainCreateInfo() const {
            return swapchainCreateInfo;
        }

        //层和扩展存储接口
        const std::vector<const char*>& InstanceLayers() const {
            return instanceLayers;
        }
        const std::vector<const char*>& InstanceExtensions() const {
            return instanceExtensions;
        }
        const std::vector<const char*>& DeviceExtensions() const {
            return deviceExtensions;
        }
        //GetterEND

        //function
        //Const函数 检查层和扩展
 
        VkResult CheckInstanceLayers(std::span<const char*> layersToCheck) const {
            uint32_t layerCount;
            std::vector<VkLayerProperties> availableLayers;
            if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance layers!\n");
                return result;
            }
            if (layerCount) {
                availableLayers.resize(layerCount);
                if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data())) {
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to enumerate instance layer properties!\nError code: {}\n", int32_t(result));
                    return result;
                }
                for (auto& i : layersToCheck) {
                    bool found = false;
                    for (auto& j : availableLayers)
                        if (!strcmp(i, j.layerName)) {
                            found = true;
                            break;
                        }
                    if (!found)
                        i = nullptr;
                }
            }
            else
                for (auto& i : layersToCheck)
                {
                    i = nullptr;
                }
            //一切顺利则返回VK_SUCCESS
            return VK_SUCCESS;
        }
 
        VkResult CheckInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
            uint32_t extensionCount;
            std::vector<VkExtensionProperties> availableExtensions;
            if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr)) {
                layerName ?
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\nLayer name:{}\n", layerName) :
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\n");
                return result;
            }
            if (extensionCount) {
                availableExtensions.resize(extensionCount);
                if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, availableExtensions.data())) {
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to enumerate instance extension properties!\nError code: {}\n", int32_t(result));
                    return result;
                }
                for (auto& i : extensionsToCheck) {
                    bool found = false;
                    for (auto& j : availableExtensions)
                        if (!strcmp(i, j.extensionName)) {
                            found = true;
                            break;
                        }
                    if (!found)
                        i = nullptr;
                }
            }
            else
                for (auto& i : extensionsToCheck)
                    i = nullptr;
            return VK_SUCCESS;
        }
 
        VkResult CheckDeviceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const {
            /*待Ch1-3填充*/
        }

        //Non-const函数  添加层和扩展
        void AddInstanceLayer(const char* layerName) {
            instanceLayers.push_back(layerName);
        }

        void AddInstanceExtension(const char* extensionName) {
            instanceExtensions.push_back(extensionName);
        }

        void AddDeviceExtension(const char* extensionName) {
            deviceExtensions.push_back(extensionName);
        }

        void InstanceLayers(const std::vector<const char*>& layerNames) {
            instanceLayers = layerNames;
        }

        void InstanceExtensions(const std::vector<const char*>& extensionNames) {
            instanceExtensions = extensionNames;
        }

        void DeviceExtensions(const std::vector<const char*>& extensionNames) {
            deviceExtensions = extensionNames;
        }


        //实例、表面相关函数
        VkResult UseLatestApiVersion() {
            if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")) {
                return vkEnumerateInstanceVersion(&apiVersion);
            }
            return VK_SUCCESS;
        }

        result_t CreateInstance(VkInstanceCreateFlags flags = 0) {
            if constexpr (ENABLE_DEBUG_MESSENGER)
                AddInstanceLayer("VK_LAYER_KHRONOS_validation"),
                AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            VkApplicationInfo applicatianInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .apiVersion = apiVersion
            };
            VkInstanceCreateInfo instanceCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .flags = flags,
                .pApplicationInfo = &applicatianInfo,
                .enabledLayerCount = uint32_t(instanceLayers.size()),
                .ppEnabledLayerNames = instanceLayers.data(),
                .enabledExtensionCount = uint32_t(instanceExtensions.size()),
                .ppEnabledExtensionNames = instanceExtensions.data()
            };
            if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to create a vulkan instance!\nError code: {}\n", int32_t(result));
                return result;
            }
            outStream << std::format(
                "Vulkan API Version: {}.{}.{}\n",
                VK_VERSION_MAJOR(apiVersion),
                VK_VERSION_MINOR(apiVersion),
                VK_VERSION_PATCH(apiVersion));
            if constexpr (ENABLE_DEBUG_MESSENGER)
                CreateDebugMessenger();
            return VK_SUCCESS;
        }
       
        void Surface(VkSurfaceKHR surface) {
            if (!this->surface)
                this->surface = surface;
        }
  
        VkResult GetSurfaceFormats() {
            uint32_t surfaceFormatCount;
            if (VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of surface formats!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (!surfaceFormatCount)
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to find any supported surface format!\n"),
                abort();
            availableSurfaceFormats.resize(surfaceFormatCount);
            VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, availableSurfaceFormats.data());
            if (result)
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get surface formats!\nError code: {}\n", int32_t(result));
            return result;
        }

        VkResult SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
            bool formatIsAvailable = false;
            if (!surfaceFormat.format) {
                //如果格式未指定，只匹配色彩空间，图像格式有啥就用啥
                for (auto& i : availableSurfaceFormats)
                    if (i.colorSpace == surfaceFormat.colorSpace) {
                        swapchainCreateInfo.imageFormat = i.format;
                        swapchainCreateInfo.imageColorSpace = i.colorSpace;
                        formatIsAvailable = true;
                        break;
                    }
            }
            else
                //否则匹配格式和色彩空间
                for (auto& i : availableSurfaceFormats)
                    if (i.format == surfaceFormat.format &&
                        i.colorSpace == surfaceFormat.colorSpace) {
                        swapchainCreateInfo.imageFormat = i.format;
                        swapchainCreateInfo.imageColorSpace = i.colorSpace;
                        formatIsAvailable = true;
                        break;
                    }
            //如果没有符合的格式，恰好有个语义相符的错误代码
            if (!formatIsAvailable)
                return VK_ERROR_FORMAT_NOT_SUPPORTED;
            //如果交换链已存在，调用RecreateSwapchain()重建交换链
            if (swapchain)
                return RecreateSwapchain();
            return VK_SUCCESS;
        }


        //设备相关函数


        VkResult WaitIdle() const {
            VkResult result = vkDeviceWaitIdle(device);
            if (result)
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to wait for the device to be idle!\nError code: {}\n", int32_t(result));
            return result;
        }
       

        VkResult GetPhysicalDevices() {
            uint32_t deviceCount;
            if (VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of physical devices!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (!deviceCount)
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to find any physical device supports vulkan!\n"),
                abort();
            availablePhysicalDevices.resize(deviceCount);
            VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
            if (result)
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to enumerate physical devices!\nError code: {}\n", int32_t(result));
            return result;
        }
     
        VkResult GetQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphicsQueue, bool enableComputeQueue, uint32_t(&queueFamilyIndices)[3]) {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
            if (!queueFamilyCount)
                return VK_RESULT_MAX_ENUM;
            std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertieses.data());
            auto& [ig, ip, ic] = queueFamilyIndices;
            ig = ip = ic = VK_QUEUE_FAMILY_IGNORED;
            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                //这三个VkBool32变量指示是否可获取（指应该被获取且能获取）相应队列族索引
                VkBool32
                    //只在enableGraphicsQueue为true时获取支持图形操作的队列族的索引
                    supportGraphics = enableGraphicsQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
                    supportPresentation = false,
                    //只在enableComputeQueue为true时获取支持计算的队列族的索引
                    supportCompute = enableComputeQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                //只在创建了window surface时获取支持呈现的队列族的索引
                if (surface)
                    if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation)) {
                        outStream << std::format("[ graphicsBase ] ERROR\nFailed to determine if the queue family supports presentation!\nError code: {}\n", int32_t(result));
                        return result;
                    }
                //若某队列族同时支持图形操作和计算
                if (supportGraphics && supportCompute) {
                    //若需要呈现，最好是三个队列族索引全部相同
                    if (supportPresentation) {
                        ig = ip = ic = i;
                        break;
                    }
                    //除非ig和ic都已取得且相同，否则将它们的值覆写为i，以确保两个队列族索引相同
                    if (ig != ic ||
                        ig == VK_QUEUE_FAMILY_IGNORED)
                        ig = ic = i;
                    //如果不需要呈现，那么已经可以break了
                    if (!surface)
                        break;
                }
                //若任何一个队列族索引可以被取得但尚未被取得，将其值覆写为i
                if (supportGraphics &&
                    ig == VK_QUEUE_FAMILY_IGNORED)
                    ig = i;
                if (supportPresentation &&
                    ip == VK_QUEUE_FAMILY_IGNORED)
                    ip = i;
                if (supportCompute &&
                    ic == VK_QUEUE_FAMILY_IGNORED)
                    ic = i;
            }
            if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
                ip == VK_QUEUE_FAMILY_IGNORED && surface ||
                ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue)
                return VK_RESULT_MAX_ENUM;
            queueFamilyIndex_graphics = ig;
            queueFamilyIndex_presentation = ip;
            queueFamilyIndex_compute = ic;
            return VK_SUCCESS;
        }

        VkResult DeterminePhysicalDevice(uint32_t deviceIndex = 0, bool enableGraphicsQueue = true, bool enableComputeQueue = true) {
            //定义一个特殊值用于标记一个队列族索引已被找过但未找到
            static constexpr uint32_t notFound = INT32_MAX;//== VK_QUEUE_FAMILY_IGNORED & INT32_MAX
            //定义队列族索引组合的结构体
            struct queueFamilyIndexCombination {
                uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
                uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
                uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
            };
            //queueFamilyIndexCombinations用于为各个物理设备保存一份队列族索引组合
            static std::vector<queueFamilyIndexCombination> queueFamilyIndexCombinations(availablePhysicalDevices.size());
            auto& [ig, ip, ic] = queueFamilyIndexCombinations[deviceIndex];

            //如果有任何队列族索引已被找过但未找到，返回VK_RESULT_MAX_ENUM
            if (ig == notFound && enableGraphicsQueue ||
                ip == notFound && surface ||
                ic == notFound && enableComputeQueue)
                return VK_RESULT_MAX_ENUM;

            //如果有任何队列族索引应被获取但还未被找过
            if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
                ip == VK_QUEUE_FAMILY_IGNORED && surface ||
                ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
                uint32_t indices[3];
                VkResult result = GetQueueFamilyIndices(availablePhysicalDevices[deviceIndex], enableGraphicsQueue, enableComputeQueue, indices);
                //若GetQueueFamilyIndices(...)返回VK_SUCCESS或VK_RESULT_MAX_ENUM（vkGetPhysicalDeviceSurfaceSupportKHR(...)执行成功但没找齐所需队列族），
                //说明对所需队列族索引已有结论，保存结果到queueFamilyIndexCombinations[deviceIndex]中相应变量
                //应被获取的索引若仍为VK_QUEUE_FAMILY_IGNORED，说明未找到相应队列族，VK_QUEUE_FAMILY_IGNORED（~0u）与INT32_MAX做位与得到的数值等于notFound
                if (result == VK_SUCCESS ||
                    result == VK_RESULT_MAX_ENUM) {
                    if (enableGraphicsQueue)
                        ig = indices[0] & INT32_MAX;
                    if (surface)
                        ip = indices[1] & INT32_MAX;
                    if (enableComputeQueue)
                        ic = indices[2] & INT32_MAX;
                }
                //如果GetQueueFamilyIndices(...)执行失败，return
                if (result)
                    return result;
            }

            //若以上两个if分支皆不执行，则说明所需的队列族索引皆已被获取，从queueFamilyIndexCombinations[deviceIndex]中取得索引
            else {
                queueFamilyIndex_graphics = enableGraphicsQueue ? ig : VK_QUEUE_FAMILY_IGNORED;
                queueFamilyIndex_presentation = surface ? ip : VK_QUEUE_FAMILY_IGNORED;
                queueFamilyIndex_compute = enableComputeQueue ? ic : VK_QUEUE_FAMILY_IGNORED;
            }
            physicalDevice = availablePhysicalDevices[deviceIndex];
            return VK_SUCCESS;
        }
 
        VkResult CreateDevice(VkDeviceCreateFlags flags = 0) {
            float queuePriority = 1.f;
            VkDeviceQueueCreateInfo queueCreateInfos[3] = {
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueCount = 1,
                    .pQueuePriorities = &queuePriority },
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueCount = 1,
                    .pQueuePriorities = &queuePriority },
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueCount = 1,
                    .pQueuePriorities = &queuePriority } };
            uint32_t queueCreateInfoCount = 0;
            if (queueFamilyIndex_graphics != VK_QUEUE_FAMILY_IGNORED)
                queueCreateInfos[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndex_graphics;
            if (queueFamilyIndex_presentation != VK_QUEUE_FAMILY_IGNORED &&
                queueFamilyIndex_presentation != queueFamilyIndex_graphics)
                queueCreateInfos[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndex_presentation;
            if (queueFamilyIndex_compute != VK_QUEUE_FAMILY_IGNORED &&
                queueFamilyIndex_compute != queueFamilyIndex_graphics &&
                queueFamilyIndex_compute != queueFamilyIndex_presentation)
                queueCreateInfos[queueCreateInfoCount++].queueFamilyIndex = queueFamilyIndex_compute;
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
            VkDeviceCreateInfo deviceCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .flags = flags,
                .queueCreateInfoCount = queueCreateInfoCount,
                .pQueueCreateInfos = queueCreateInfos,
                .enabledExtensionCount = uint32_t(deviceExtensions.size()),
                .ppEnabledExtensionNames = deviceExtensions.data(),
                .pEnabledFeatures = &physicalDeviceFeatures
            };
            if (VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to create a vulkan logical device!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (queueFamilyIndex_graphics != VK_QUEUE_FAMILY_IGNORED)
                vkGetDeviceQueue(device, queueFamilyIndex_graphics, 0, &queue_graphics);
            if (queueFamilyIndex_presentation != VK_QUEUE_FAMILY_IGNORED)
                vkGetDeviceQueue(device, queueFamilyIndex_presentation, 0, &queue_presentation);
            if (queueFamilyIndex_compute != VK_QUEUE_FAMILY_IGNORED)
                vkGetDeviceQueue(device, queueFamilyIndex_compute, 0, &queue_compute);
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
            //输出所用的物理设备名称
            outStream << std::format("Renderer: {}\n", physicalDeviceProperties.deviceName);
            /*待Ch1-4填充*/
            return VK_SUCCESS;
        }

        //该函数用于重建逻辑设备
        VkResult RecreateDevice(VkDeviceCreateFlags flags = 0) {
            if (VkResult result = WaitIdle())
            {
                return result;
            }
            if (swapchain) {
                //调用销毁交换链时的回调函数
                for (auto& i : callbacks_destroySwapchain)
                    i();
                //销毁交换链图像的image view
                for (auto& i : swapchainImageViews)
                    if (i)
                        vkDestroyImageView(device, i, nullptr);
                swapchainImageViews.resize(0);
                //销毁交换链
                vkDestroySwapchainKHR(device, swapchain, nullptr);
                //重置交换链handle
                swapchain = VK_NULL_HANDLE;
                //重置交换链创建信息
                swapchainCreateInfo = {};
            }
            for (auto& i : callbacks_destroyDevice)
                i();
            if (device)
                vkDestroyDevice(device, nullptr),
                device = VK_NULL_HANDLE;

            outStream << std::format("Renderer: {}\n", physicalDeviceProperties.deviceName);
            for (auto& i : callbacks_createDevice)
            {
                i();
            }
            return CreateDevice(flags);
        }

        //交换链相关函数
        VkResult CreateSwapchain(bool limitFrameRate = true, VkSwapchainCreateFlagsKHR flags = 0) {
           
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
            if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get physical device surface capabilities!\nError code: {}\n", int32_t(result));
                return result;
            }
            swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + (surfaceCapabilities.maxImageCount > surfaceCapabilities.minImageCount);
            swapchainCreateInfo.imageExtent =
            surfaceCapabilities.currentExtent.width == -1 ?
                VkExtent2D{
                    glm::clamp(defaultWindowSize.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
                    glm::clamp(defaultWindowSize.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height) } :
                    surfaceCapabilities.currentExtent;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
            if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
            {
                swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
            }
            else
            {
                for (size_t i = 0; i < 4; i++)
                    if (surfaceCapabilities.supportedCompositeAlpha & 1 << i) {
                        swapchainCreateInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(surfaceCapabilities.supportedCompositeAlpha & 1 << i);
                        break;
                    }
            }
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
            {
                swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }
            if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            {
                swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            }
            else
            {
                outStream << std::format("[ graphicsBase ] WARNING\nVK_IMAGE_USAGE_TRANSFER_DST_BIT isn't supported!\n");
            }
            if (availableSurfaceFormats.empty())
                if (VkResult result = GetSurfaceFormats())
                    return result;
            if (!swapchainCreateInfo.imageFormat)
            {
                //用&&操作符来短路执行
                if (SetSurfaceFormat({ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }) &&
                    SetSurfaceFormat({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })) {
                    swapchainCreateInfo.imageFormat = availableSurfaceFormats[0].format;
                    swapchainCreateInfo.imageColorSpace = availableSurfaceFormats[0].colorSpace;
                    outStream << std::format("[ graphicsBase ] WARNING\nFailed to select a four-component UNORM surface format!\n");
                }
            }
            uint32_t surfacePresentModeCount;
            if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of surface present modes!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (!surfacePresentModeCount)
            {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to find any surface present mode!\n"),
                    abort();
            }
            std::vector<VkPresentModeKHR> surfacePresentModes(surfacePresentModeCount);
            if (VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes.data())) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get surface present modes!\nError code: {}\n", int32_t(result));
                return result;
            }
            swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            if (!limitFrameRate)
            {
                for (size_t i = 0; i < surfacePresentModeCount; i++)
                    if (surfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                    }
            }
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.flags = flags;
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.clipped = VK_TRUE;
            if (VkResult result = CreateSwapchain_Internal())
            {
                return result;
            }
            for (auto& i : callbacks_createSwapchain)
            {
                i();
            }
            return VK_SUCCESS;
        }
        
        VkResult CreateSwapchain_Internal() {
            if (VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to create a swapchain!\nError code: {}\n", int32_t(result));
                return result;
            }
            uint32_t swapchainImageCount;
            if (VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get the count of swapchain images!\nError code: {}\n", int32_t(result));
                return result;
            }
            swapchainImages.resize(swapchainImageCount);
            if (VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data())) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get swapchain images!\nError code: {}\n", int32_t(result));
                return result;
            }
            swapchainImageViews.resize(swapchainImageCount);
            VkImageViewCreateInfo imageViewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchainCreateInfo.imageFormat,
                //.components = {},//四个成员皆为VK_COMPONENT_SWIZZLE_IDENTITY
                .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
            };
            for (size_t i = 0; i < swapchainImageCount; i++) {
                imageViewCreateInfo.image = swapchainImages[i];
                if (VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i])) {
                    outStream << std::format("[ graphicsBase ] ERROR\nFailed to create a swapchain image view!\nError code: {}\n", int32_t(result));
                    return result;
                }
            }
           
            return VK_SUCCESS;


        }

        VkResult RecreateSwapchain() {
          
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
            if (VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities)) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to get physical device surface capabilities!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (surfaceCapabilities.currentExtent.width == 0 ||
                surfaceCapabilities.currentExtent.height == 0)
                return VK_SUBOPTIMAL_KHR;
            swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
            swapchainCreateInfo.oldSwapchain = swapchain;
            VkResult result = vkQueueWaitIdle(queue_graphics);
            //仅在等待图形队列成功，且图形与呈现所用队列不同时等待呈现队列
            if (!result &&
                queue_graphics != queue_presentation)
                result = vkQueueWaitIdle(queue_presentation);
            if (result) {
                outStream << std::format("[ graphicsBase ] ERROR\nFailed to wait for the queue to be idle!\nError code: {}\n", int32_t(result));
                return result;
            }
            for (auto& i : callbacks_destroySwapchain)
            {
                i();
            }
            for (auto& i : swapchainImageViews)
            {
                if (i)
                    vkDestroyImageView(device, i, nullptr);
            }
            swapchainImageViews.resize(0);
            //创建新交换链及与之相关的对象
            if (VkResult result = CreateSwapchain_Internal())
                return result;
            for (auto& i : callbacks_createSwapchain)
            {
                i();
            }
            return VK_SUCCESS;
        }
        //静态函数

        


        //单例化
        static graphicsBase& Base() {
            return singleton;
        }
    };

    inline graphicsBase graphicsBase::singleton;


    class fence {
        VkFence handle = VK_NULL_HANDLE;
    public:
        //fence() = default;
        fence(VkFenceCreateInfo& createInfo) {
            Create(createInfo);
        }
        //默认构造器创建未置位的栅栏
        fence(VkFenceCreateFlags flags = 0) {
            Create(flags);
        }
        fence(fence&& other) noexcept { MoveHandle; }
        ~fence() { DestroyHandleBy(vkDestroyFence); }
        //Getter
        DefineHandleTypeOperator;
        DefineAddressFunction;
        //Const Function
        result_t Wait() const {
            VkResult result = vkWaitForFences(vulkan::graphicsBase::Base().Device(), 1, &handle, false, UINT64_MAX);
            if (result)
                outStream << std::format("[ fence ] ERROR\nFailed to wait for the fence!\nError code: {}\n", int32_t(result));
            return result;
        }
        result_t Reset() const {
            VkResult result = vkResetFences(vulkan::graphicsBase::Base().Device(), 1, &handle);
            if (result)
                outStream << std::format("[ fence ] ERROR\nFailed to reset the fence!\nError code: {}\n", int32_t(result));
            return result;
        }
        //因为“等待后立刻重置”的情形经常出现，定义此函数
        result_t WaitAndReset() const {
            VkResult result = Wait();
            result || (result = Reset());
            return result;
        }
        result_t Status() const {
            VkResult result = vkGetFenceStatus(vulkan::graphicsBase::Base().Device(), handle);
            if (result < 0) //vkGetFenceStatus(...)成功时有两种结果，所以不能仅仅判断result是否非0
                outStream << std::format("[ fence ] ERROR\nFailed to get the status of the fence!\nError code: {}\n", int32_t(result));
            return result;
        }
        //Non-const Function
        result_t Create(VkFenceCreateInfo& createInfo) {
            createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            VkResult result = vkCreateFence(vulkan::graphicsBase::Base().Device(), &createInfo, nullptr, &handle);
            if (result)
                outStream << std::format("[ fence ] ERROR\nFailed to create a fence!\nError code: {}\n", int32_t(result));
            return result;
        }
        result_t Create(VkFenceCreateFlags flags = 0) {
            VkFenceCreateInfo createInfo = {
                .flags = flags
            };
            return Create(createInfo);
        }
    };

    class semaphore {
        VkSemaphore handle = VK_NULL_HANDLE;
    public:
        //semaphore() = default;
        semaphore(VkSemaphoreCreateInfo& createInfo) {
            Create(createInfo);
        }
        //默认构造器创建未置位的信号量
        semaphore(/*VkSemaphoreCreateFlags flags*/) {
            Create();
        }
        semaphore(semaphore&& other) noexcept { MoveHandle; }
        ~semaphore() { DestroyHandleBy(vkDestroySemaphore); }
        //Getter
        DefineHandleTypeOperator;
        DefineAddressFunction;
        //Non-const Function
        result_t Create(VkSemaphoreCreateInfo& createInfo) {
            createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkResult result = vkCreateSemaphore(vulkan::graphicsBase::Base().Device(), &createInfo, nullptr, &handle);
            if (result)
                outStream << std::format("[ semaphore ] ERROR\nFailed to create a semaphore!\nError code: {}\n", int32_t(result));
            return result;
        }
        result_t Create(/*VkSemaphoreCreateFlags flags*/) {
            VkSemaphoreCreateInfo createInfo = {};
            return Create(createInfo);
        }
    };


}