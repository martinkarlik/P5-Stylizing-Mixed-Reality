using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class FPSscript : MonoBehaviour
{
    public Text fpsText;
    bool showFps = true;
    bool firstFrame = true;
    public float averageFps;
    int totalFrames = 0;
    float current;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (showFps && !firstFrame)
        {
            current = (int)(1f / Time.unscaledDeltaTime);
            updateAverageFps(current);
            fpsText.text = "FPS: " + current.ToString() + "\n" + "Average:  " + averageFps.ToString();
        }

        if (firstFrame) { firstFrame = false; }
    }

    private void updateAverageFps(float newFps)
    {
        ++totalFrames;
        averageFps += (newFps - averageFps) / totalFrames;
    }

}
