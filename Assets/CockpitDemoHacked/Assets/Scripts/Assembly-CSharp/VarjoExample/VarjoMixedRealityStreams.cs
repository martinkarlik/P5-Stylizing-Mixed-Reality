using UnityEngine;
using System;
using UnityEngine.Events;
using Varjo;

namespace VarjoExample
{
	public class VarjoMixedRealityStreams : MonoBehaviour
	{
		[Serializable]
		public class DistortedColorFrameEvent : UnityEvent<VarjoDistortedColorStream.VarjoDistortedColorFrame>
		{
		}

		[Serializable]
		public class CubemapFrameEvent : UnityEvent<VarjoEnvironmentCubemapStream.VarjoEnvironmentCubemapFrame>
		{
		}

		public bool distortedColor;
		public bool environmentCubemap;
		public DistortedColorFrameEvent onNewFrame;
		public CubemapFrameEvent onCubemapFrame;
	}
}
