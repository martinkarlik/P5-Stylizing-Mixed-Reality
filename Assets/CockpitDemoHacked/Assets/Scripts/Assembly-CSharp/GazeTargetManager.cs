using UnityEngine;
using System;
using UnityEngine.Events;

public class GazeTargetManager : MonoBehaviour
{
	[Serializable]
	public class GazeHitEvent : UnityEvent<GameObject>
	{
	}

	[Serializable]
	public class GazeHitEndEvent : UnityEvent<GameObject>
	{
	}

	public GameObject currentObject;
	public LayerMask gazeTargets;
	public float spherecastRadius;
	public GazeHitEvent OnGazeHit;
	public GazeHitEndEvent OnGazeHitEnd;
	public Transform varjoCamera;
}
