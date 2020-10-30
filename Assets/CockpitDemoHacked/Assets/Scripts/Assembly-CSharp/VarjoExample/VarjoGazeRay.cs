using UnityEngine;

namespace VarjoExample
{
	public class VarjoGazeRay : MonoBehaviour
	{
		public enum Eye
		{
			both = 0,
			left = 1,
			right = 2,
		}

		public Eye eye;
		public float gazeRayRadius;
		public bool drawDebug;
	}
}
