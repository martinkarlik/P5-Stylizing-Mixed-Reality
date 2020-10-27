using UnityEngine;
using System;
using System.Collections.Generic;
using TMPro;

public class Analytics : MonoBehaviour
{
	[Serializable]
	public struct Target
	{
		public GameObject gameObject;
		public bool seen;
	}

	[SerializeField]
	public List<Analytics.Target> fireTestTargets;
	[SerializeField]
	public List<Analytics.Target> preFlightTargets;
	public float fireTestTotalTime;
	public float preFlightTotalTime;
	public bool fireTestRunning;
	public bool preFlightRunning;
	public TextMeshPro results;
}
