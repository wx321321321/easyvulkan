#pragma once

#include<iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <span>
#include <memory>
#include <functional>
#include <concepts>
#include <format>
#include <chrono>
#include <numeric>
#include <numbers>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include"stb_image.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#pragma comment (lib,"vulkan-1.lib")
#endif
#include <vulkan/vulkan.h>


#ifdef VK_RESULT_THROW
class result_t {
    VkResult result;
public:
    static void(*callback_throw)(VkResult);
    result_t(VkResult result) :result(result) {}
    result_t(result_t&& other) noexcept :result(other.result) { other.result = VK_SUCCESS; }
    ~result_t() noexcept(false) {
        if (uint32_t(result) < VK_RESULT_MAX_ENUM)
            return;
        if (callback_throw)
            callback_throw(result);
        throw result;
    }
    operator VkResult() {
        VkResult result = this->result;
        this->result = VK_SUCCESS;
        return result;
    }
};
inline void(*result_t::callback_throw)(VkResult);

//情况2：若抛弃函数返回值，让编译器发出警告
#elif defined VK_RESULT_NODISCARD
struct [[nodiscard]] result_t {
    VkResult result;
    result_t(VkResult result) :result(result) {}
    operator VkResult() const { return result; }
};
//在本文件中关闭弃值提醒（因为我懒得做处理）
#pragma warning(disable:4834)
#pragma warning(disable:6031)

//情况3：啥都不干
#else
using result_t = VkResult;
#endif

template<typename T>
class arrayRef {
    T* const pArray = nullptr;
    size_t count = 0;
public:
    //从空参数构造，count为0
    arrayRef() = default;
    //从单个对象构造，count为1
    arrayRef(T& data) :pArray(&data), count(1) {}
    //从顶级数组构造
    template<size_t elementCount>
    arrayRef(T(&data)[elementCount]) : pArray(data), count(elementCount) {}
    //从指针和元素个数构造
    arrayRef(T* pData, size_t elementCount) :pArray(pData), count(elementCount) {}
    //复制构造，若T带const修饰，兼容从对应的无const修饰版本的arrayRef构造
    //24.01.07 修正因复制粘贴产生的typo：从pArray(&other)改为pArray(other.Pointer())
    arrayRef(const arrayRef<std::remove_const_t<T>>& other) :pArray(other.Pointer()), count(other.Count()) {}
    //Getter
    T* Pointer() const { return pArray; }
    size_t Count() const { return count; }
    //Const Function
    T& operator[](size_t index) const { return pArray[index]; }
    T* begin() const { return pArray; }
    T* end() const { return pArray + count; }
    //Non-const Function
    //禁止复制/移动赋值
    arrayRef& operator=(const arrayRef&) = delete;
};

inline auto& outStream = std::cout;

template <typename T>
bool Between_Closed(T lower, T value, T upper) {
    return (value >= lower && value <= upper);
}