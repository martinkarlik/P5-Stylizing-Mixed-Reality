using UnityEngine;

namespace VarjoExample
{
	public class VarjoGazeCalibrationRequest : MonoBehaviour
	{
		public enum CalibrationType
		{
			LEGACY = 0,
			FAST = 1,
		}

		public enum OutputFilterType
		{
			STANDARD = 0,
			NONE = 1,
		}

		public KeyCode key;
		public bool useApplicationButton;
		public bool useCalibrationParameters;
		public CalibrationType calibrationType;
		public OutputFilterType outputFilterType;
	}
}
