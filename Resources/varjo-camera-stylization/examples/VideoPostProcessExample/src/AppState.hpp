// Copyright 2020 Varjo Technologies Oy. All rights reserved.

#pragma once

// Compile time flag to disable swapchain creation, rendering, and layer submission.
// In headless mode this application only alters the video-see-through image feed,
// but does not render anything by itself.

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
    } general;

    // VST Post process params
    struct PostProcess {
        bool enabled = false;
        VarjoExamples::PostProcess::ShaderSource shaderSource = VarjoExamples::PostProcess::ShaderSource::None;
        VarjoExamples::PostProcess::GraphicsAPI graphicsAPI = VarjoExamples::PostProcess::GraphicsAPI::None;
        TestTexture::Type textureType = TestTexture::Type::Noise;

        bool textureEnabled = false;
        bool textureGeneratedOnGPU = false;

        
        bool cartoonEnabled = false;
        int clusterSize = 0;
        float outlineIntensity = 0.0f;

        bool watercolorEnabled = false;
        int watercolorRadius = 0;

        bool sketchEnabled = false;
        float sketchIntensity = 0.0f;

        bool pointilismEnabled = false;
        float pointilismStep = 0.0f;
        float pointilismThreshold = 0.0f;

        bool blurEnabled = false;
        int blurRadius = 0;
        bool separableFilterEnabled = false;

        
    } postProcess;
};
