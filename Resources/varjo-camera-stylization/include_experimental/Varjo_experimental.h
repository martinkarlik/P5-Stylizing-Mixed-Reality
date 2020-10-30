// Copyright 2020 Varjo Technologies Oy. All rights reserved.
#ifndef VARJO_EXPERIMENTAL_H
#define VARJO_EXPERIMENTAL_H

#include "Varjo_types.h"
#include "Varjo_types_experimental.h"
#include "Varjo_export.h"

#if defined __cplusplus
extern "C" {
#endif

/**
 * Gets texture size for specific varjo_TextureSize_type and view index.
 * Check #varjo_TextureSize_Type description for details about different varjo_TextureSize_type.
 *
 * @param session Varjo session handle
 * @param type texture size type
 * @param viewIndex view index
 * @param width pointer to an int which will contain width (can't be null)
 * @param height pointer to an int which will contain height (can't be null)
 */
VARJO_EXPERIMENTAL_API void varjo_GetTextureSize(
    struct varjo_Session* session, varjo_TextureSize_Type type, int32_t viewIndex, int32_t* width, int32_t* height);

/**
 * Enables specified foveation mode.
 *
 * @param session Varjo session handle
 * @param mode foveation mode
 * @param enabled varjo_True to enable and varjo_False to disable
 */
VARJO_EXPERIMENTAL_API void varjo_SetFoveationMode(struct varjo_Session* session, varjo_FoveationMode mode, varjo_Bool enabled);

/**
 * Creates variable rate shading texture. This function will add execution of compute shader which will calculate best possible
 * VRS map based on flags. Command list should be executed as late as possible to get most recent gaze.
 * Function should be called as many times as there are views: 4 or 2.
 *
 * @see varjo_VariableRateShadingConfig for details.
 *
 * @param session Varjo session handle
 * @param commandList d3d12 command list
 * @param texture texture which will be used as VRS map
 * @param textureState texture state of given texture (texture will be transitioned in this state after update)
 * @param config configuration flags
 */
VARJO_EXPERIMENTAL_API void varjo_D3D12UpdateVariableRateShadingTexture(struct varjo_Session* session, struct ID3D12GraphicsCommandList* commandList,
    struct ID3D12Resource* texture, enum D3D12_RESOURCE_STATES textureState, struct varjo_VariableRateShadingConfig* config);

#if defined __cplusplus
}
#endif

#endif  // VARJO_EXPERIMENTAL_H