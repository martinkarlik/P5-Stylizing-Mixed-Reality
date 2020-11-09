using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderScript : MonoBehaviour
{
    public GameObject outsideCollider;
    public GameObject insideCollider;
    public GameObject door;
    public GameObject walls;
    public GameObject hiddenEnvironment;
    public GameObject player;
    public GameObject camera;
    public bool doorEntered;
    public bool inVr = false;
    public bool inVrSide = false;

    // Start is called before the first frame update
    void Start()
    {
       



    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey("w"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0.1f, 0f, 0f);
        }

        if (Input.GetKey("s"))
        {
            camera.GetComponent<Transform>().position += new Vector3(-0.1f, 0f, 0f);
        }

        if (Input.GetKey("a"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0f, 0f, 0.1f);
        }

        if (Input.GetKey("d"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0f, 0f, -0.1f);
        }



    }

    void enableVST()
    {
        hiddenEnvironment.SetActive(true);
        Debug.Log("VST disbled LMAO");
    }

    void disableVST()
    {
        hiddenEnvironment.SetActive(false);
        Debug.Log("VST disabled LOL");
    }

    public void collide(string id)
    {
        //Debug.Log("collision detected");
        if(id == "outside")
        {
            if (!inVr) { enableVST(); }
            inVrSide = false;
        } else if (id == "inside")
        {
            if (!inVr) { disableVST(); }
            inVrSide = true;
        }
        if(id == "door")
        {
            doorEntered = true;

        }
    }

    public void unCollide(string id)
    {
        if(id == "door")
        {
            if (inVrSide)
            {
                inVr = true;
                walls.SetActive(false);
            }

            if (!inVrSide)
            {
                inVr = false;
                walls.SetActive(true);
            }
        }
    }


}
