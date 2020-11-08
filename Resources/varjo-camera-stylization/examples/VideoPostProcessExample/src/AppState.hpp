// Copyright 2020 Varjo Technologies Oy. All rights reserved.

#pragma once

// Compile time flag to disable swapchain creation, rendering, and layer submission.
// In headless mode this application only alters the video-see-through image feed,
// but does not render anything by itself.
#define USE_HEADLESS_MODE 0

#include <glm/glm.hpp>

#include "Globals.hpp"
#include "PostProcess.hpp"
#include "TestTexture.hpp"

//! Application state struct
struct AppState {
    // General params structure
    struct General {
        double frameTime = 0.0f;   //!< Current frame time
        int64_t frameCount = 0;    //!< Current frame count
        bool mrAvailable = false;  //!< Mixed reality available flag
        bool vstEnabled = true;    //!< Render VST image flag
#if (!USE_HEADLESS_MODE)
        bool vrEnabled = false;  //!< Render VR scene flag
#endif
    } general;

    // VST Post process params
    struct PostProcess {
        bool enabled = false;
        VarjoExamples::PostProcess::ShaderSource shaderSource = VarjoExamples::PostProcess::ShaderSource::None;
        VarjoExamples::PostProcess::GraphicsAPI graphicsAPI = VarjoExamples::PostProcess::GraphicsAPI::None;
        TestTexture::Type textureType = TestTexture::Type::Noise;

        bool textureEnabled = false;
        bool textureGeneratedOnGPU = false;

        
        bool grayscaleEnabled = false;

        bool clustersEnabled = false;
        int clusterSize = 0;

        bool watercolorEnabled = false;
        int watercolorRadius = 0;

        bool outlinesEnabled = false;
        float outlineIntensity = 0.0f;

        bool sketchEnabled = false;
        float sketchIntensity = 0.0f;

        bool horizontalMirrorFuckery = false;
        bool verticalMirrorFuckery = false;
        bool puzzleFuckery = false;

    } postProcess;
};
