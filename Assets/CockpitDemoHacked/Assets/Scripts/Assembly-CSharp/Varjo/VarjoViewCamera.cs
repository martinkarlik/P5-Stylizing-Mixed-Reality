using UnityEngine;

namespace Varjo
{
	public class VarjoViewCamera : MonoBehaviour
	{
		public enum CAMERA_ID
		{
			CONTEXT_LEFT = 0,
			CONTEXT_RIGHT = 1,
			FOCUS_LEFT = 2,
			FOCUS_RIGHT = 3,
		}

		[SerializeField]
		private CAMERA_ID _cameraId;
		public Camera cam;
	}
}
