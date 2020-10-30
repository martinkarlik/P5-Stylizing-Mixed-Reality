// Copyright 2019 Varjo Technologies Oy. All rights reserved.

#ifndef VARJO_MATH_H
#define VARJO_MATH_H

#include "Varjo_export.h"
#include "Varjo_types.h"
#include <math.h>

#if defined __cplusplus
extern "C" {
#endif

/**
 * Extracts the euler angles in radians from the given matrix.
 *
 * Default convention is right handed counter clockwise rotation, where
 * X-axis is negative and Y and Z axes are positive.
 *
 * Rotations are global, to get local rotations flip the axes. Eg:
 *   Global rotation XYZ -> local rotation ZYX.
 *
 * @param matrix Matrix to extract the angles from.
 * @param order Euler axis order.
 * @param direction Rotation direction.
 * @param handedness Rotation Handedness.
 */
VARJO_API struct varjo_Vector3D varjo_GetEulerAngles(
    struct varjo_Matrix* matrix, varjo_EulerOrder order, varjo_RotationDirection direction, varjo_Handedness handedness);

/**
 * Gets the translation part from a transformation matrix.
 *
 * @param matrix Matrix to extract the translation from.
 */
VARJO_API struct varjo_Vector3D varjo_GetPosition(struct varjo_Matrix* matrix);

/**
 * Updates the near and far clip plane distances in the projection matrix.
 *
 * @param projectionMatrix Projection matrix to modify.
 * @param clipRange Clip space range (0..1 or -1..1).
 * @param nearZ Positive distance in meters to the near plane. To indicate depth values reversed, can be greater than farZ.
 * @param farZ Positive distance in meters to the far plane. To indicate depth values reversed, can be smaller than nearZ.
 */
static void varjo_UpdateNearFarPlanes(double* projectionMatrix, varjo_ClipRange clipRange, double nearZ, double farZ)
{
    if (projectionMatrix == NULL) {
        return;
    }

    if (clipRange == varjo_ClipRangeZeroToOne) {
        if (farZ == HUGE_VAL) {
            // Normal Z-range, infinite far clip plane, D3D/Vulkan convention
            projectionMatrix[10] = -1.;
            projectionMatrix[14] = -nearZ;
        } else if (nearZ == HUGE_VAL) {
            // Inverted Z-range, infinite far clip plane, D3D/Vulkan convention
            projectionMatrix[10] = 0.;
            projectionMatrix[14] = farZ;
        } else {
            // Note: Following formula handles both cases: when farZ > nearZ and when nearZ > farZ
            projectionMatrix[10] = farZ / (nearZ - farZ);
            projectionMatrix[14] = (nearZ * farZ) / (nearZ - farZ);
        }
    } else {
        if (nearZ == HUGE_VAL || farZ == HUGE_VAL) {
            // Handles both normal and reverted Z-range in OpenGL convention in
            // the case of far clip plane located at +infinity
            projectionMatrix[10] = -1.;
            projectionMatrix[14] = nearZ <= farZ ? nearZ : farZ;
        } else {
            // Handles both normal and reverted Z-ranges in OpenGL convention
            projectionMatrix[10] = (nearZ + farZ) / (nearZ - farZ);
            projectionMatrix[14] = (2 * nearZ * farZ) / (nearZ - farZ);
        }
    }
}

#if defined __cplusplus
}
#endif

#endif  // VARJO_MATH_H
