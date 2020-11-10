using UnityEngine;

using UnityEngine.Rendering;

using UnityEngine.Rendering.HighDefinition;

using System;

using UnityEditor;

<<<<<<< Updated upstream:Assets/Post-process shaders/GrayScale.cs
[Serializable, VolumeComponentMenu("Post-processing/Custom/GrayScale")]
=======

[Serializable, VolumeComponentMenu("Post-processing/Custom/vrPostProcess")]
>>>>>>> Stashed changes:Assets/Post-process shaders/vrPostProcess.cs

public sealed class vrPostProcess : CustomPostProcessVolumeComponent, IPostProcessComponent

{

    [Tooltip("Controls the intensity of the effect.")]

    public ClampedFloatParameter intensity = new ClampedFloatParameter(0f, 0f, 1f);

    [Tooltip("Controls the lineStrength of the effect.")]

    public ClampedFloatParameter line_strength = new ClampedFloatParameter(0f, 0f, 1f);

<<<<<<< Updated upstream:Assets/Post-process shaders/GrayScale.cs
    [Tooltip("Controls the radius of the effect of the effect.")]

    public ClampedFloatParameter radius = new ClampedFloatParameter(0f, 0f, 1f);
=======
    [Tooltip("Controls the radius of the water color effect.")]

    public ClampedIntParameter water_color_radius = new ClampedIntParameter(1, 1, 6);
>>>>>>> Stashed changes:Assets/Post-process shaders/vrPostProcess.cs

    Material m_Material;

    public bool IsActive() => m_Material != null && intensity.value > 0f;

    public override CustomPostProcessInjectionPoint injectionPoint => CustomPostProcessInjectionPoint.AfterPostProcess;

    public override void Setup()

    {

        if (Shader.Find("Hidden/Shader/vrPostProcess") != null)

            m_Material = new Material(Shader.Find("Hidden/Shader/vrPostProcess"));

    }

    public override void Render(CommandBuffer cmd, HDCamera camera, RTHandle source, RTHandle destination)

    {

        if (m_Material == null)

            return;

        m_Material.SetFloat("_Intensity", intensity.value);

        m_Material.SetTexture("_InputTexture", source);

        // our variables
<<<<<<< Updated upstream:Assets/Post-process shaders/GrayScale.cs
        m_Material.SetFloat("_LineStrength", lineStrength.value);

        m_Material.SetFloat("_Radius", radius.value);
        
=======
        m_Material.SetFloat("_LineStrength", line_strength.value);
        m_Material.SetInt("_WaterColorRadius", water_color_radius.value);      
>>>>>>> Stashed changes:Assets/Post-process shaders/vrPostProcess.cs

        HDUtils.DrawFullScreen(cmd, m_Material, destination);

    }

    public override void Cleanup() => CoreUtils.Destroy(m_Material);

}