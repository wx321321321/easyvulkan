#pragma once
#include "EasyVKStart.h"
//����vulkan�����ռ䣬֮����Vulkan��һЩ��������ķ�װд������
namespace vulkan {

    class graphicsBase {

        //ʵ�����
        uint32_t apiVersion = VK_API_VERSION_1_0;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        std::vector<VkPhysicalDevice> availablePhysicalDevices;
        
        //�豸���
        VkDevice device;
        uint32_t queueFamilyIndex_graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_compute = VK_QUEUE_FAMILY_IGNORED;
        VkQueue queue_graphics;
        VkQueue queue_presentation;
        VkQueue queue_compute;

        //�������
        VkSurfaceKHR surface;
        std::vector <VkSurfaceFormatKHR> availableSurfaceFormats;

        //���������
        VkSwapchainKHR swapchain;
        std::vector <VkImage> swapchainImages;
        std::vector <VkImageView> swapchainImageViews;
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

        //��չ�Ͳ�
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
        std::vector<const char*> deviceExtensions;

        VkDebugUtilsMessengerEXT debugMessenger;

        //��̬����
        static graphicsBase singleton;
        //�����ƶ���������
        graphicsBase() = default;
        graphicsBase(graphicsBase&&) = delete;
        ~graphicsBase() {
            /*��Ch1-4���*/
        }

        //������ʹ
        VkResult CreateDebugMessenger() {
            static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback = [](
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData)->VkBool32 {
                    std::cout << std::format("{}\n\n", pCallbackData->pMessage);
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
                    std::cout << std::format("[ graphicsBase ] ERROR\nFailed to create a debug messenger!\nError code: {}\n", int32_t(result));
                return result;
            }
            std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the function pointer of vkCreateDebugUtilsMessengerEXT!\n");
            return VK_RESULT_MAX_ENUM;
        }
    public:

        //Getter
        //ʵ�������Ϣ�洢�ӿ�
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

        //�豸�����Ϣ�洢�ӿ�
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

        //���������Ϣ�洢�ӿ�
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

        //�����������Ϣ�洢�ӿ�
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

        //�����չ�洢�ӿ�
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
        //Const���� �������չ
 
        VkResult CheckInstanceLayers(std::span<const char*> layersToCheck) const {
            uint32_t layerCount;
            std::vector<VkLayerProperties> availableLayers;
            if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr)) {
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance layers!\n");
                return result;
            }
            if (layerCount) {
                availableLayers.resize(layerCount);
                if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data())) {
                    std::cout << std::format("[ graphicsBase ] ERROR\nFailed to enumerate instance layer properties!\nError code: {}\n", int32_t(result));
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
            //һ��˳���򷵻�VK_SUCCESS
            return VK_SUCCESS;
        }
 
        VkResult CheckInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
            uint32_t extensionCount;
            std::vector<VkExtensionProperties> availableExtensions;
            if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr)) {
                layerName ?
                    std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\nLayer name:{}\n", layerName) :
                    std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\n");
                return result;
            }
            if (extensionCount) {
                availableExtensions.resize(extensionCount);
                if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, availableExtensions.data())) {
                    std::cout << std::format("[ graphicsBase ] ERROR\nFailed to enumerate instance extension properties!\nError code: {}\n", int32_t(result));
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
            /*��Ch1-3���*/
        }

        //Non-const����  ��Ӳ����չ
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


        //ʵ����������غ���
        VkResult UseLatestApiVersion() {
            if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")) {
                return vkEnumerateInstanceVersion(&apiVersion);
            }
            return VK_SUCCESS;
        }

        VkResult CreateInstance(VkInstanceCreateFlags flags = 0) {
#ifndef NDEBUG
            AddInstanceLayer("VK_LAYER_KHRONOS_validation");
            AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
            VkApplicationInfo applicationInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .apiVersion = apiVersion
            };
            VkInstanceCreateInfo instanceCreateInfo = {
                 .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                 .flags = flags,
                 .pApplicationInfo = &applicationInfo,
                 .enabledLayerCount = uint32_t(instanceLayers.size()),
                 .ppEnabledLayerNames = instanceLayers.data(),
                 .enabledExtensionCount = uint32_t(instanceExtensions.size()),
                 .ppEnabledExtensionNames = instanceExtensions.data()
            };
            if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to create a vulkan instance!\nError code: {}\n", int32_t(result));
                return result;
            }
            std::cout << std::format(
                "Vulkan API Version: {}.{}.{}\n",
                VK_VERSION_MAJOR(apiVersion),
                VK_VERSION_MINOR(apiVersion),
                VK_VERSION_PATCH(apiVersion));
#ifndef NDEBUG
            //������Vulkanʵ��������Ŵ���debug messenger
            CreateDebugMessenger();
#endif
            return VK_SUCCESS;





        }
    
        void Surface(VkSurfaceKHR surface) {
            if (!this->surface)
                this->surface = surface;
        }
  
        VkResult GetSurfaceFormats() {
            /*��Ch1-4���*/
        }

        VkResult SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
            /*��Ch1-4���*/
        }


        //�豸��غ���

        VkResult GetPhysicalDevices() {
            uint32_t deviceCount;
            if (VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) {
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the count of physical devices!\nError code: {}\n", int32_t(result));
                return result;
            }
            if (!deviceCount)
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to find any physical device supports vulkan!\n"),
                abort();
            availablePhysicalDevices.resize(deviceCount);
            VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
            if (result)
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to enumerate physical devices!\nError code: {}\n", int32_t(result));
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
                //������VkBool32����ָʾ�Ƿ�ɻ�ȡ��ָӦ�ñ���ȡ���ܻ�ȡ����Ӧ����������
                VkBool32
                    //ֻ��enableGraphicsQueueΪtrueʱ��ȡ֧��ͼ�β����Ķ����������
                    supportGraphics = enableGraphicsQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
                    supportPresentation = false,
                    //ֻ��enableComputeQueueΪtrueʱ��ȡ֧�ּ���Ķ����������
                    supportCompute = enableComputeQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                //ֻ�ڴ�����window surfaceʱ��ȡ֧�ֳ��ֵĶ����������
                if (surface)
                    if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation)) {
                        std::cout << std::format("[ graphicsBase ] ERROR\nFailed to determine if the queue family supports presentation!\nError code: {}\n", int32_t(result));
                        return result;
                    }
                //��ĳ������ͬʱ֧��ͼ�β����ͼ���
                if (supportGraphics && supportCompute) {
                    //����Ҫ���֣��������������������ȫ����ͬ
                    if (supportPresentation) {
                        ig = ip = ic = i;
                        break;
                    }
                    //����ig��ic����ȡ������ͬ���������ǵ�ֵ��дΪi����ȷ������������������ͬ
                    if (ig != ic ||
                        ig == VK_QUEUE_FAMILY_IGNORED)
                        ig = ic = i;
                    //�������Ҫ���֣���ô�Ѿ�����break��
                    if (!surface)
                        break;
                }
                //���κ�һ���������������Ա�ȡ�õ���δ��ȡ�ã�����ֵ��дΪi
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
            //����һ������ֵ���ڱ��һ�������������ѱ��ҹ���δ�ҵ�
            static constexpr uint32_t notFound = INT32_MAX;//== VK_QUEUE_FAMILY_IGNORED & INT32_MAX
            //���������������ϵĽṹ��
            struct queueFamilyIndexCombination {
                uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
                uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
                uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
            };
            //queueFamilyIndexCombinations����Ϊ���������豸����һ�ݶ������������
            static std::vector<queueFamilyIndexCombination> queueFamilyIndexCombinations(availablePhysicalDevices.size());
            auto& [ig, ip, ic] = queueFamilyIndexCombinations[deviceIndex];

            //������κζ����������ѱ��ҹ���δ�ҵ�������VK_RESULT_MAX_ENUM
            if (ig == notFound && enableGraphicsQueue ||
                ip == notFound && surface ||
                ic == notFound && enableComputeQueue)
                return VK_RESULT_MAX_ENUM;

            //������κζ���������Ӧ����ȡ����δ���ҹ�
            if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
                ip == VK_QUEUE_FAMILY_IGNORED && surface ||
                ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
                uint32_t indices[3];
                VkResult result = GetQueueFamilyIndices(availablePhysicalDevices[deviceIndex], enableGraphicsQueue, enableComputeQueue, indices);
                //��GetQueueFamilyIndices(...)����VK_SUCCESS��VK_RESULT_MAX_ENUM��vkGetPhysicalDeviceSurfaceSupportKHR(...)ִ�гɹ���û������������壩��
                //˵��������������������н��ۣ���������queueFamilyIndexCombinations[deviceIndex]����Ӧ����
                //Ӧ����ȡ����������ΪVK_QUEUE_FAMILY_IGNORED��˵��δ�ҵ���Ӧ�����壬VK_QUEUE_FAMILY_IGNORED��~0u����INT32_MAX��λ��õ�����ֵ����notFound
                if (result == VK_SUCCESS ||
                    result == VK_RESULT_MAX_ENUM) {
                    if (enableGraphicsQueue)
                        ig = indices[0] & INT32_MAX;
                    if (surface)
                        ip = indices[1] & INT32_MAX;
                    if (enableComputeQueue)
                        ic = indices[2] & INT32_MAX;
                }
                //���GetQueueFamilyIndices(...)ִ��ʧ�ܣ�return
                if (result)
                    return result;
            }

            //����������if��֧�Բ�ִ�У���˵������Ķ������������ѱ���ȡ����queueFamilyIndexCombinations[deviceIndex]��ȡ������
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
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to create a vulkan logical device!\nError code: {}\n", int32_t(result));
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
            //������õ������豸����
            std::cout << std::format("Renderer: {}\n", physicalDeviceProperties.deviceName);
            /*��Ch1-4���*/
            return VK_SUCCESS;
        }

        //��������غ���
        VkResult CreateSwapchain(bool limitFrameRate = true, VkSwapchainCreateFlagsKHR flags = 0) {
            /*��Ch1-4���*/
        }

        VkResult RecreateSwapchain() {
            /*��Ch1-4���*/
        }
        //��̬����

        VkResult CreateSwapchain_Internal() {
            /*��Ch1-4���*/
        }


        //������
        static graphicsBase& Base() {
            return singleton;
        }
    };
    inline graphicsBase graphicsBase::singleton;


}