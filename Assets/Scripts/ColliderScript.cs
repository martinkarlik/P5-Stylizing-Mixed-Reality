﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderScript : MonoBehaviour
{
    public GameObject outsideCollider;
    public GameObject insideCollider;
    public GameObject door;
    public GameObject walls;
    public GameObject hiddenEnvironment;
    public GameObject vrOnlyEnvironment;
    public GameObject realOnlyEnvironment;
    public GameObject camera;
    public GameObject key;
    public GameObject grabber;
    public GameObject center;
    public GameObject vstWall;
    public bool doorEntered;
    public bool inVr = false;
    public bool inVrSide = false;
    public bool insideColliding;
    public bool outsideColliding;
    public bool grabbing = false;
    public bool grabbableIsNear = false;
    public bool debugging;
    public float turnSpeed = 0.5f;
    public float grabberBaseSize = 0.1777344f;
    public float grabberGrabSize = 0.1288948f;

    // Start is called before the first frame update
    void Start()
    {
       
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey("w"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0.05f, 0f, 0f);
        }

        if (Input.GetKey("s"))
        {
            camera.GetComponent<Transform>().position += new Vector3(-0.05f, 0f, 0f);
        }

        if (Input.GetKey("a"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0f, 0f, 0.05f);
        }

        if (Input.GetKey("d"))
        {
            camera.GetComponent<Transform>().position += new Vector3(0f, 0f, -0.05f);
        }
        if (Input.GetKey("q"))
        {
          camera.GetComponent<Transform>().Rotate(0.0f, -turnSpeed, 0.0f);
        }
        if (Input.GetKey("e"))
        {
          camera.GetComponent<Transform>().Rotate(0.0f, turnSpeed, 0.0f);
        }

        //FOR GRABBING
        if (Input.GetAxis("XRI_Right_Trigger") > 0.01f)
        {
            grabKey();
            grabber.GetComponent<Transform>().localScale = new Vector3(grabberGrabSize, grabberGrabSize, grabberGrabSize);
        }

        if (Input.GetAxis("XRI_Right_Trigger") < 0.01f)
        {
            unGrabKey();
            grabber.GetComponent<Transform>().localScale = new Vector3(grabberBaseSize, grabberBaseSize, grabberBaseSize);
        }

    }

    void enableVST()
    {
        hiddenEnvironment.SetActive(true);
        if (debugging) { Debug.Log("VST disbled"); }
    }

    void disableVST()
    {
        hiddenEnvironment.SetActive(false);
        if (debugging) { Debug.Log("VST disabled"); }
    }

    

    public void collide(string id,string collidingObjectId)
    {
        if (debugging) { Debug.Log("collision detected"); }
        if(id == "outside" && collidingObjectId == "Player")
        {
            if (doorEntered) { inVr = false; walls.SetActive(true); vrOnlyEnvironment.SetActive(false); realOnlyEnvironment.SetActive(true); vstWall.SetActive(true); }

            if (!inVr) { enableVST(); vstWall.SetActive(true); }

            inVrSide = false;

            outsideColliding = true;

            if (debugging) { Debug.Log("outside collision"); }

        } else if (id == "inside" && collidingObjectId == "Player")
        {

            if (doorEntered) { inVr = true; walls.SetActive(false); vrOnlyEnvironment.SetActive(true); realOnlyEnvironment.SetActive(false); vstWall.SetActive(false); }

            if (!inVr) { disableVST(); vstWall.SetActive(false); }

            inVrSide = true;

            insideColliding = true;
            if (debugging) { Debug.Log("inside collision"); }
        }

        if(id == "door" && collidingObjectId == "Player")
        {
            doorEntered = true;
            if (debugging) { Debug.Log("door entered"); }
        }


        if (id == "Grabber" && collidingObjectId == "Key") 
        {
            grabbableIsNear = true;
        }
    }

    public void uncollide(string id, string collidingObjectId)
    {
        if (id == "door" && collidingObjectId == "Player")
        {

            doorEntered = false;
            if (debugging) { Debug.Log("Door left"); }
        }

        if (id == "inside" && collidingObjectId == "Player")
        {
            insideColliding = false;
            if (outsideColliding && !doorEntered) { if (!inVr) { enableVST(); } inVrSide = false; }
        }
        if (id == "outside" && collidingObjectId == "Player")
        {
            outsideColliding = false;
            if (insideColliding && !doorEntered) { if (!inVr) { disableVST(); } inVrSide = true; }
        }

        if (id == "Grabber" && collidingObjectId == "Key")
        {
            grabbableIsNear = false;
        }
    }

    private void grabKey()
    {
        if(grabbableIsNear){
        grabbing = true;
        key.GetComponent<Rigidbody>().isKinematic = true;
        key.transform.parent = grabber.transform;
        }
    }

    private void unGrabKey()
    {
        if(grabbableIsNear){
        grabbing = false;
        key.GetComponent<Rigidbody>().isKinematic = false;
        if(inVr){key.transform.parent = center.transform;} else {key.transform.parent = outsideCollider.transform;}
        }
    }


}
