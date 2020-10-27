// Copyright 2019-2020 Varjo Technologies Oy. All rights reserved.

#pragma once

#include <cstdio>
#include <cstdint>
#include <system_error>
#include <string>
#include <stdexcept>
#include <functional>

#include <wrl.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Varjo.h>

// Use MS COM smart pointers for DX objects
using Microsoft::WRL::ComPtr;

namespace VarjoExamples
{
enum class LogLevel { Critical = 0, Error, Warning, Info, Debug };
using LogFunc = std::function<void(LogLevel, const std::string&)>;

//! Initialize logging. If not initialized, logging goes to stdout.
extern void initLog(LogFunc logFunc, LogLevel logLevel);

//! Write log entry
extern void writeLog(LogLevel level, const char* funcName, int lineNum, const char* prefix, const char* format, ...);

//! Macro for initializing logging. If not initialized, logging goes to stdout.
#define LOG_INIT(LOGFUNC, LOGLEVEL)                \
    {                                              \
        VarjoExamples::initLog(LOGFUNC, LOGLEVEL); \
    }

//! Macro for deinitializing logging. After this, logging goes to stdout.
#define LOG_DEINIT()                                                        \
    {                                                                       \
        VarjoExamples::initLog(nullptr, VarjoExamples::LogLevel::Critical); \
    }

//! Macro for debug log
#define LOGD(FORMAT, ...)                                                                                         \
    {                                                                                                             \
        VarjoExamples::writeLog(VarjoExamples::LogLevel::Debug, __FUNCTION__, __LINE__, "", FORMAT, __VA_ARGS__); \
    }

//! Macro for info log
#define LOGI(FORMAT, ...)                                                                                        \
    {                                                                                                            \
        VarjoExamples::writeLog(VarjoExamples::LogLevel::Info, __FUNCTION__, __LINE__, "", FORMAT, __VA_ARGS__); \
    }

//! Macro for warn log
#define LOGW(FORMAT, ...)                                                                                                 \
    {                                                                                                                     \
        VarjoExamples::writeLog(VarjoExamples::LogLevel::Warning, __FUNCTION__, __LINE__, "WARN: ", FORMAT, __VA_ARGS__); \
    }

//! Macro for error log
#define LOGE(FORMAT, ...)                                                                                                \
    {                                                                                                                    \
        VarjoExamples::writeLog(VarjoExamples::LogLevel::Error, __FUNCTION__, __LINE__, "ERROR: ", FORMAT, __VA_ARGS__); \
    }

//! Macro for critical error. This will throw a std::runtime_error exception.
#define CRITICAL(FORMAT, ...)                                                                                                  \
    {                                                                                                                          \
        VarjoExamples::writeLog(VarjoExamples::LogLevel::Critical, __FUNCTION__, __LINE__, "CRITICAL: ", FORMAT, __VA_ARGS__); \
    }

//! Check Windows error code
inline void checkHResult(const char* func, int line, const char* what, HRESULT hr)
{
    if (FAILED(hr)) {
        LOGE("Fail result %s (%x) at %s():%d: %s", what, hr, func, line, std::system_category().message(hr).c_str());
        throw std::runtime_error(std::system_category().message(hr).c_str());
    }
}

//! Macro for checking microsoft HRESULT
#define CHECK_HRESULT(VALUE) VarjoExamples::checkHResult(__FUNCTION__, __LINE__, #VALUE, VALUE)

//! Check Varjo error code
inline varjo_Error checkVError(const char* func, int line, varjo_Session* session)
{
    if (session) {
        auto error = varjo_GetError(session);
        if (error != varjo_NoError) {
            LOGE("Varjo error code (%lld) at %s():%d: %s", error, func, line, varjo_GetErrorDesc(error));
            return error;
        }
    } else {
        LOGE("Invalid Varjo session at %s():%d.", func, line);
        throw std::runtime_error("Invalid Varjo session.");
    }
    return varjo_NoError;
}

//! Macro for checking Varjo error.
#define CHECK_VARJO_ERR(SESSION) VarjoExamples::checkVError(__FUNCTION__, __LINE__, SESSION)

//! Get Varjo matrix from GLM matrix
inline varjo_Matrix toVarjoMatrix(const glm::mat4x4& m)
{
    return {m[0][0], m[0][1], m[0][2], m[0][3],  //
        m[1][0], m[1][1], m[1][2], m[1][3],      //
        m[2][0], m[2][1], m[2][2], m[2][3],      //
        m[3][0], m[3][1], m[3][2], m[3][3]};
}

//! Get Varjo matrix from GLM matrix
inline varjo_Matrix3x3 toVarjoMatrix(const glm::mat3x3& m)
{
    return {m[0][0], m[0][1], m[0][2],  //
        m[1][0], m[1][1], m[1][2],      //
        m[2][0], m[2][1], m[2][2]};
}

//! Get Varjo vector from GLM vector
inline varjo_Vector3D toVarjoVector(const glm::vec3& v) { return {v.x, v.y, v.z}; }

//! Get GLM matrix from Varjo matrix
inline glm::mat4x4 fromVarjoMatrix(const varjo_Matrix& m)
{
    return glm::mat4x4(m.value[0], m.value[1], m.value[2], m.value[3],  //
        m.value[4], m.value[5], m.value[6], m.value[7],                 //
        m.value[8], m.value[9], m.value[10], m.value[11],               //
        m.value[12], m.value[13], m.value[14], m.value[15]);
}

//! Get GLM matrix from Varjo matrix
inline glm::mat3x3 fromVarjoMatrix(const varjo_Matrix3x3& m)
{
    return glm::mat3x3(m.value[0], m.value[1], m.value[2],  //
        m.value[3], m.value[4], m.value[5],                 //
        m.value[6], m.value[7], m.value[8]);
}

//! Get GLM vector from Varjo size
inline glm::vec3 fromVarjoSize(const varjo_Size3D& s) { return glm::vec3(s.width, s.height, s.depth); }

//! Get GLM vector from Varjo vecotr
inline glm::vec3 fromVarjoVector(const varjo_Vector3D& v) { return glm::vec3(v.x, v.y, v.z); }

}  // namespace VarjoExamples
