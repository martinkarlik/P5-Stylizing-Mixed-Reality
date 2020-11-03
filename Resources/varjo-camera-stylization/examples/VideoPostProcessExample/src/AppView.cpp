// Copyright 2020 Varjo Technologies Oy. All rights reserved.

#include "AppView.hpp"

#include <unordered_map>
#include <tchar.h>
#include <iostream>
#include <imgui_internal.h>
#include <map>

// VarjoExamples namespace contains simple example wrappers for using Varjo API features.
// These are only meant to be used in SDK example applications. In your own application,
// use your own production quality integration layer.
using namespace VarjoExamples;

namespace
{
// Input actions
enum class Action {
    None = 0,
    Help,
    ToggleTestPresets,
    ApplyPreset_0,
    ApplyPreset_1,
    ApplyPreset_2,
    ApplyPreset_3,
    ApplyPreset_4,
    ApplyPreset_5,
    ApplyPreset_6,
    ApplyPreset_7,
    ApplyPreset_8,
    ApplyPreset_9,
};

//! Action info structure
struct ActionInfo {
    std::string name;  //!< Action name
    int keyCode;       //!< Shortcut keycode
    std::string help;  //!< Help string
};

// Key shortcut mapping
std::unordered_map<int, Action> c_keyMappings;

// Action names
const std::map<Action, ActionInfo> c_actions = {
    {Action::None, {"None", 0, "(no action)"}},
    {Action::Help, {"Help", VK_F1, "F1   Print help"}},
    {Action::ToggleTestPresets, {"Toggle test presets", 'T', "T    Toggle test presets"}},
    {Action::ApplyPreset_0, {"Apply Preset 0", '0', "0    Apply preset 0"}},
    {Action::ApplyPreset_1, {"Apply Preset 1", '1', "1    Apply preset 1"}},
    {Action::ApplyPreset_2, {"Apply Preset 2", '2', "2    Apply preset 2"}},
    {Action::ApplyPreset_3, {"Apply Preset 3", '3', "3    Apply preset 3"}},
    {Action::ApplyPreset_4, {"Apply Preset 4", '4', "4    Apply preset 4"}},
    {Action::ApplyPreset_5, {"Apply Preset 5", '5', "5    Apply preset 5"}},
    {Action::ApplyPreset_6, {"Apply Preset 6", '6', "6    Apply preset 6"}},
    {Action::ApplyPreset_7, {"Apply Preset 7", '7', "7    Apply preset 7"}},
    {Action::ApplyPreset_8, {"Apply Preset 8", '8', "8    Apply preset 8"}},
    {Action::ApplyPreset_9, {"Apply Preset 9", '9', "9    Apply preset 9"}},
};

// Window client area margin
constexpr int c_windowMargin = 8;

// Window client area size and log height
constexpr glm::ivec2 c_windowClientSize(720, 800);
constexpr int c_logHeight = 230;

// Default preset
constexpr int c_defaultPresetIndex = 1;

// Post process GUI presets
const std::vector<std::pair<std::string, AppState::PostProcess>> c_guiPresets = {
    {"Off",
        {
            false, PostProcess::ShaderSource::None, PostProcess::GraphicsAPI::D3D11, TestTexture::Type::Noise, 
            false, 0,
            false, 0.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
            false, true, 0.0f, 0.0f                                                                            
        }},
    {"Default",
        {
            true, PostProcess::ShaderSource::Binary, PostProcess::GraphicsAPI::D3D11, TestTexture::Type::Noise,
            true, 10,
            true, 0.5f, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),             
            true, true, 0.1f, 1.0f                                                    
        }}

};



}  

AppView::AppView(AppLogic& logic)
    : m_logic(logic)
{
    // Fill in key mappings
    for (auto& ai : c_actions) {
        c_keyMappings[ai.second.keyCode] = ai.first;
    }

    // Create user interface instance
    m_ui = std::make_unique<UI>(std::bind(&AppView::onFrame, this, std::placeholders::_1),    //
        std::bind(&AppView::onKeyPress, this, std::placeholders::_1, std::placeholders::_2),  //
        _T("Varjo Stylization Interface"), c_windowClientSize.x, c_windowClientSize.y);

    // Set log function
    LOG_INIT(std::bind(&UI::writeLogEntry, m_ui.get(), std::placeholders::_1, std::placeholders::_2), LogLevel::Info);

    // Create contexts
    m_context = std::make_unique<GfxContext>(m_ui->getWindowHandle());

    // Additional ImgUi setup
    auto& io = ImGui::GetIO();

    // Disable storing UI ini file
    io.IniFilename = NULL;

    LOGI("Video Post Process Test Client");
    LOGI("(C) 2020 Varjo Technologies");
    LOGI("-------------------------------");
}

AppView::~AppView()
{
    // Deinit logger
    LOG_DEINIT();

    // Free UI
    m_ui.reset();
}

bool AppView::init()
{
    if (!m_logic.init(*m_context)) {
        LOGE("Initializing application failed.");
        return false;
    }

    // Reset states
    m_uiState = {};
    AppState appState;
    appState.general = m_logic.getState().general;
    appState.postProcess = c_guiPresets[c_defaultPresetIndex].second;

    // Force set initial state
    m_logic.setState(appState, true);

    return true;
}

void AppView::run()
{
    LOGD("Entering main loop.");

    // Run UI main loop
    m_ui->run();
}

bool AppView::onFrame(UI& ui)
{
    // Check UI instance
    if (&ui != m_ui.get()) {
        return false;
    }

    // Check for varjo events
    m_logic.checkEvents();

    // Update state to logic state
    updateUI();

    // Update application logic
    m_logic.update();

    // Return true to continue running
    return true;
}

void AppView::onKeyPress(UI& ui, int keyCode)
{
    if (m_uiState.anyItemActive) {
        // Ignore key handling if UI items active
        return;
    }

    bool stateDirty = false;
    auto appState = m_logic.getState();

    // Check for input action
    Action action = Action::None;
    if (c_keyMappings.count(keyCode)) {
        action = c_keyMappings.at(keyCode);
    }

    if (action != Action::None) {
        LOGI("Action: %s", c_actions.at(action).name.c_str());
    }

    // Handle input actions
    switch (action) {
        case Action::None: {
            // Ignore
        } break;
        case Action::Help: {
            LOGI("\nKeyboard Shortcuts:\n");
            for (const auto& ai : c_actions) {
                if (ai.first != Action::None) {
                    LOGI("  %s", ai.second.help.c_str());
                }
            }
            LOGI("");
        } break;
        case Action::ToggleTestPresets: {
            m_uiState.testPresets = !m_uiState.testPresets;
            LOGI("Test presets: %s", (m_uiState.testPresets ? "ON" : "OFF"));
        } break;
        case Action::ApplyPreset_0:
        case Action::ApplyPreset_1:
        case Action::ApplyPreset_2:
        case Action::ApplyPreset_3:
        case Action::ApplyPreset_4:
        case Action::ApplyPreset_5:
        case Action::ApplyPreset_6:
        case Action::ApplyPreset_7:
        case Action::ApplyPreset_8:
        case Action::ApplyPreset_9: {
            int i = static_cast<int>(action) - static_cast<int>(Action::ApplyPreset_0);
            const auto& presets = c_guiPresets;
            if (i >= 0 && i < static_cast<int>(presets.size())) {
                const auto& preset = presets[i];
                LOGI("Apply preset: %s", preset.first.c_str());
                m_uiState.postProcessShaderSourceIndex = static_cast<int>(preset.second.shaderSource);
                m_uiState.graphicsApiIndex = 1 + static_cast<int>(preset.second.graphicsAPI);
                m_uiState.textureTypeIndex = 1 + static_cast<int>(preset.second.textureType);
                appState.postProcess = preset.second;
                stateDirty = true;
            } else {
                LOGI("No preset to apply: %d", i);
            }
        } break;
        default: {
            // Ignore unknown action
        } break;
    }

    // Update state if changed
    if (stateDirty) {
        m_logic.setState(appState, false);
    }
}

void AppView::updateUI()
{
#define _PUSHSTYLE_ALPHA(X) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, X);
#define _POPSTYLE ImGui::PopStyleVar();

#define _PUSHDISABLEDIF(C)                                  \
    if (C) {                                                \
        _PUSHSTYLE_ALPHA(0.5f);                             \
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); \
    }

#define _POPDISABLEDIF(C)     \
    if (C) {                  \
        _POPSTYLE;            \
        ImGui::PopItemFlag(); \
    }

    // Update from logic state
    AppState appState = m_logic.getState();

    // Update UI state from logic state
    m_uiState.postProcessShaderSourceIndex = static_cast<int>(appState.postProcess.shaderSource);
    m_uiState.graphicsApiIndex = static_cast<int>(appState.postProcess.graphicsAPI) - 1;
    m_uiState.textureTypeIndex = static_cast<int>(appState.postProcess.textureType) - 1;

    constexpr float h = 8.0f;

    // VST Post Process window
    {
        ImGui::Begin("Video Post Process");

        // Set initial size and pos
        {
            const float m = static_cast<float>(c_windowMargin);
            const float w = static_cast<float>(c_windowClientSize.x);
            const float h = static_cast<float>(c_windowClientSize.y - c_logHeight);
            ImGui::SetWindowPos(ImVec2(m, m), ImGuiCond_FirstUseEver);
            ImGui::SetWindowSize(ImVec2(w - 2 * m, h - 2 * m), ImGuiCond_FirstUseEver);
        }

        ImGui::Checkbox("Render video", &appState.general.vstEnabled);
// #if (!USE_HEADLESS_MODE)
//         ImGui::SameLine();
//         ImGui::Checkbox("Render VR scene", &appState.general.vrEnabled);
// #endif

        ImGui::SameLine();
        ImGui::Checkbox("Post process video", &appState.postProcess.enabled);

        {
            std::array<char*, 3> items = {"None", "Binary Blob", "HLSL Source"};
            ImGui::Combo("Shader source", &m_uiState.postProcessShaderSourceIndex, items.data(), static_cast<int>(items.size()));
            appState.postProcess.shaderSource = static_cast<VarjoExamples::PostProcess::ShaderSource>(m_uiState.postProcessShaderSourceIndex);
        }

        {
            std::array<char*, 3> items = {"D3D11 textures", "OpenGL textures", "D3D12 textures"};
            ImGui::Combo("Graphics API", &m_uiState.graphicsApiIndex, items.data(), static_cast<int>(items.size()));
            appState.postProcess.graphicsAPI = static_cast<VarjoExamples::PostProcess::GraphicsAPI>(1 + m_uiState.graphicsApiIndex);
        }

        {
            std::array<char*, 2> items = {"Noise", "Gradient"};
            ImGui::Combo("Texture type", &m_uiState.textureTypeIndex, items.data(), static_cast<int>(items.size()));
            appState.postProcess.textureType = static_cast<TestTexture::Type>(1 + m_uiState.textureTypeIndex);
        }

        // Force CPU generation on D3D12 for now
        const bool gpuGenerateDisabled = (m_uiState.graphicsApiIndex > 1);
        if (gpuGenerateDisabled) {
            appState.postProcess.textureGeneratedOnGPU = false;
        }

        ImGui::Dummy(ImVec2(0.0f, h));


#define _TAG "##noise"

        ImGui::Checkbox("Enable texture" _TAG, &appState.postProcess.textureEnabled);

        _PUSHDISABLEDIF(gpuGenerateDisabled);
        ImGui::Checkbox("GPU generated" _TAG, &appState.postProcess.textureGeneratedOnGPU);
        _POPDISABLEDIF(gpuGenerateDisabled);

        ImGui::SliderFloat("Amount" _TAG, &appState.postProcess.textureAmount, 0.0f, 1.0f);
        ImGui::SliderFloat("Scale" _TAG, &appState.postProcess.textureScale, 0.0f, 5.0f);
        ImGui::Dummy(ImVec2(0.0f, h));

#undef _TAG

#define _TAG "##color_clustering"

        ImGui::Checkbox("Color clustering" _TAG, &appState.postProcess.colorClusteringEnabled);
        ImGui::SliderInt("Cluster size" _TAG, &appState.postProcess.clusterSize, 1, 30);
        ImGui::Dummy(ImVec2(0.0f, h));

#undef _TAG

#define _TAG "##outlines"

        ImGui::Checkbox("Outlines" _TAG, &appState.postProcess.outlinesEnabled);
        ImGui::ColorEdit3("Outline color" _TAG, (float*)&appState.postProcess.outlineColor);
        ImGui::SliderFloat("Outline strength" _TAG, &appState.postProcess.outlineStrength, 0.0f, 1.0f);
        ImGui::Dummy(ImVec2(0.0f, h));

#undef _TAG


        ImGui::Text("Apply preset: ");
        const auto presets = c_guiPresets;
        for (const auto preset : presets) {
            ImGui::SameLine();
            if (ImGui::Button(preset.first.c_str())) {
                LOGI("Apply preset: %s", preset.first.c_str());
                m_uiState.postProcessShaderSourceIndex = static_cast<int>(preset.second.shaderSource);
                m_uiState.graphicsApiIndex = 1 + static_cast<int>(preset.second.graphicsAPI);
                m_uiState.textureTypeIndex = 1 + static_cast<int>(preset.second.textureType);
                appState.postProcess = preset.second;
            }
        }

        ImGui::Dummy(ImVec2(0.0f, h));
        ImGui::Text("Frame timing: %.3f fps / %.3f ms / %.3f s / %d frames",  //
            ImGui::GetIO().Framerate,                                         //
            1000.0f / ImGui::GetIO().Framerate,                               //
            appState.general.frameTime, appState.general.frameCount);
        ImGui::End();
    }

    // Log window
    {
        ImGui::Begin("Log");

        // Set initial size and pos
        {
            const float m = static_cast<float>(c_windowMargin);
            const float w = static_cast<float>(c_windowClientSize.x);
            const float h0 = static_cast<float>(c_windowClientSize.y - c_logHeight);
            const float h1 = static_cast<float>(c_logHeight);
            ImGui::SetWindowPos(ImVec2(m, h0), ImGuiCond_FirstUseEver);
            ImGui::SetWindowSize(ImVec2(w - 2 * m, h1 - m), ImGuiCond_FirstUseEver);
        }

        m_ui->drawLog();
        ImGui::End();
    }

    // Set UI item active flag
    m_uiState.anyItemActive = ImGui::IsAnyItemActive();

    // Update state from UI back to logic
    m_logic.setState(appState, false);
}
