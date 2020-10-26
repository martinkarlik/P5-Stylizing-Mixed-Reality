using UnityEngine;
using System;
using System.Collections.Generic;

public class UIManager : MonoBehaviour
{
	[Serializable]
	public struct UIStep
	{
		public bool done;
		public GameObject button;
		public Texture2D MDFLeft;
		public Texture2D MDFRight;
		public Texture2D navDisplay;
	}

	[SerializeField]
	public List<UIManager.UIStep> steps;
	public UIStep currentStep;
	public InputManager inputManager;
	public Material MDFLeftMaterial;
	public Material MDFRightMaterial;
	public Material navDisplayMaterial;
	public int currentIndex;
}
