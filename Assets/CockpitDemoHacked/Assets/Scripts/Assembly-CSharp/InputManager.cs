using UnityEngine;

public class InputManager : MonoBehaviour
{
	public Transform cursor;
	public bool invertX;
	public bool invertY;
	public bool invertButtons;
	public float speed;
	public float yLimit;
	public float xLimit;
	public KeyCode altLeftButton;
	public KeyCode altRightButton;
	public KeyCode speedIncButton;
	public KeyCode speedDecButton;
	public bool secondaryButtonDown;
}
