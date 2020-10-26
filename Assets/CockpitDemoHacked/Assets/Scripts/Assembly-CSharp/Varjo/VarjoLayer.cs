using UnityEngine;
using System.Collections.Generic;

namespace Varjo
{
	public class VarjoLayer : MonoBehaviour
	{
		public enum MSAAMode
		{
			None = 0,
			MSAA_2X = 1,
			MSAA_4X = 2,
			MSAA_8X = 3,
		}

		public bool layerEnabled;
		public int layerOrder;
		public bool faceLocked;
		public bool submitDepth;
		public bool depthTestRangeEnabled;
		public double depthTestNearZ;
		public double depthTestFarZ;
		public List<VarjoViewCamera> viewportCameras;
		public float contextDisplayFactor;
		public float focusDisplayFactor;
		public bool flipY;
		public bool opaque;
		public bool depthTesting;
		public MSAAMode antiAliasing;
		public bool copyCameraComponents;
		public bool useOcclusionMesh;
		public Material occlusionMaterial;
		public Camera varjoCamera;
	}
}
