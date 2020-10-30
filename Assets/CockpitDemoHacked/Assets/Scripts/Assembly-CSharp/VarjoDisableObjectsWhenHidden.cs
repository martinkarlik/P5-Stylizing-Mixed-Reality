using UnityEngine;
using System.Collections.Generic;

public class VarjoDisableObjectsWhenHidden : MonoBehaviour
{
	public bool disableOnStandby;
	public bool disableInBackground;
	public bool disableIfNotVisible;
	public List<MonoBehaviour> scripts;
	public List<GameObject> gameObjects;
}
