using UnityEngine;

namespace VarjoExample
{
	public class VarjoMixedReality : MonoBehaviour
	{
		public bool videoSeeThrough;
		public bool depthEstimation;
		public float VREyeOffset;
		public bool environmentReflections;
		public bool environmentLighting;
		public int lightingRefreshRate;
		public float lightingIntensity;
		public Material environmentSkyboxMaterial;
	}
}
