#pragma once

#include <stdint.h>

#define CPPPROJECT_PLUGIN_ABI_VERSION 1u

#if defined(_WIN32)
#define CPPPROJECT_PLUGIN_CALL __cdecl
#if defined(CPPPROJECT_PLUGIN_IMPLEMENTATION)
#define CPPPROJECT_PLUGIN_EXPORT __declspec(dllexport)
#else
#define CPPPROJECT_PLUGIN_EXPORT
#endif
#else
#define CPPPROJECT_PLUGIN_CALL
#if defined(CPPPROJECT_PLUGIN_IMPLEMENTATION)
#define CPPPROJECT_PLUGIN_EXPORT __attribute__((visibility("default")))
#else
#define CPPPROJECT_PLUGIN_EXPORT
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cppproject_host_api {
    uint32_t abi_version;
    void(CPPPROJECT_PLUGIN_CALL* log_message)(const char* message);
} cppproject_host_api;

typedef struct cppproject_plugin_api {
    uint32_t abi_version;
    const char* name;
    int(CPPPROJECT_PLUGIN_CALL* initialize)(const cppproject_host_api* host);
    void(CPPPROJECT_PLUGIN_CALL* shutdown)(void);
} cppproject_plugin_api;

typedef const cppproject_plugin_api*(CPPPROJECT_PLUGIN_CALL* cppproject_plugin_get_api_fn)(
    uint32_t host_abi_version);

CPPPROJECT_PLUGIN_EXPORT const cppproject_plugin_api* CPPPROJECT_PLUGIN_CALL
cppproject_plugin_get_api(uint32_t host_abi_version);

#ifdef __cplusplus
}
#endif
