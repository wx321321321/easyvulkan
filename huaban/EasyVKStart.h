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

//���2����������������ֵ���ñ�������������
#elif defined VK_RESULT_NODISCARD
struct [[nodiscard]] result_t {
    VkResult result;
    result_t(VkResult result) :result(result) {}
    operator VkResult() const { return result; }
};
//�ڱ��ļ��йر���ֵ���ѣ���Ϊ������������
#pragma warning(disable:4834)
#pragma warning(disable:6031)

//���3��ɶ������
#else
using result_t = VkResult;
#endif

template<typename T>
class arrayRef {
    T* const pArray = nullptr;
    size_t count = 0;
public:
    //�ӿղ������죬countΪ0
    arrayRef() = default;
    //�ӵ��������죬countΪ1
    arrayRef(T& data) :pArray(&data), count(1) {}
    //�Ӷ������鹹��
    template<size_t elementCount>
    arrayRef(T(&data)[elementCount]) : pArray(data), count(elementCount) {}
    //��ָ���Ԫ�ظ�������
    arrayRef(T* pData, size_t elementCount) :pArray(pData), count(elementCount) {}
    //���ƹ��죬��T��const���Σ����ݴӶ�Ӧ����const���ΰ汾��arrayRef����
    //24.01.07 ��������ճ��������typo����pArray(&other)��ΪpArray(other.Pointer())
    arrayRef(const arrayRef<std::remove_const_t<T>>& other) :pArray(other.Pointer()), count(other.Count()) {}
    //Getter
    T* Pointer() const { return pArray; }
    size_t Count() const { return count; }
    //Const Function
    T& operator[](size_t index) const { return pArray[index]; }
    T* begin() const { return pArray; }
    T* end() const { return pArray + count; }
    //Non-const Function
    //��ֹ����/�ƶ���ֵ
    arrayRef& operator=(const arrayRef&) = delete;
};

inline auto& outStream = std::cout;

template <typename T>
bool Between_Closed(T lower, T value, T upper) {
    return (value >= lower && value <= upper);
}