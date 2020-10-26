using UnityEngine;
using System.Collections.Generic;

public class FireTest : MonoBehaviour
{
	public InputManager inputManager;
	public List<Material> lightMaterials;
	public List<Material> blinkMaterials;
	public float blinkSpeed;
	public float alarmSpeed;
	public AudioSource audioSource;
	public Texture2D leftDisplayTexture;
	public Texture2D rightDisplayTexture;
	public Material leftDisplay;
	public Material rightDisplay;
	public GameObject targets;
	public Analytics analytics;
}
