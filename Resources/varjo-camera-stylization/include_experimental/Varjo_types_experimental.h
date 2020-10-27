// Copyright 2020 Varjo Technologies Oy. All rights reserved.
#ifndef VARJO_TYPES_EXPERIMENTAL_H
#define VARJO_TYPES_EXPERIMENTAL_H

#include "Varjo_types.h"

#if defined __cplusplus
extern "C" {
#endif

typedef int64_t varjo_FoveationMode;
/**
 * Projection matrix will be updated every frame to follow gaze.
 * Gaze has to be enabled and calibrated.
 */
const varjo_FoveationMode varjo_FoveationMode_DynamicFocus = 1;

typedef int64_t varjo_StructureType;

struct varjo_StructureExtension {
    varjo_StructureType type;
    struct varjo_StructureExtension* next;
};

typedef int64_t varjo_VariableRateShadingFlags;
const varjo_VariableRateShadingFlags varjo_VariableRateShadingFlag_None = 0;
const varjo_VariableRateShadingFlags varjo_VariableRateShadingFlag_Stereo = 1;  //!< Generates VRS map for stereo mode (2 views)
const varjo_VariableRateShadingFlags varjo_VariableRateShadingFlag_Gaze = 2;    //!< Generates VRS map taking gaze into account
const varjo_VariableRateShadingFlags varjo_VariableRateShadingFlag_OcclusionMap =
    4;  //!< Generates VRS with coarsest shading rate in corners which are not visible

struct varjo_VariableRateShadingConfig {
    struct varjo_StructureExtension* next;
    int32_t viewIndex;               //!< view for which VRS map should be generated
    struct varjo_Viewport viewport;  //!< viewport where VRS map should be generated inside given texture (can be whole texture or part of it)
    varjo_VariableRateShadingFlags flags;
    /**
     * Radius of best quality shading rate of foveated circle if gaze is enabled and calibrated(flags should contain varjo_VariableRateShadingFlag_Gaze).
     * Normalized value between 0 and 1 where 1 is half width of the texture.
     */
    float innerRadius;
    /**
     * Radius of outer edge of foveated circle, medium quality shading rate if gaze is enabled and calibrated, should be bigger than innerRadius.
     * Normalized value between 0 and 1 where 1 is half width of the texture.
     */
    float outerRadius;
};

#if defined __cplusplus
}
#endif

#endif  // VARJO_TYPES_EXPERIMENTAL_H