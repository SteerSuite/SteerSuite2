using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;

public class FootstepScript_new : MonoBehaviour 
{

	IntPtr reader;
	//Direct function calls
	[DllImport("RecFileIO", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]//Constructor
	public static extern IntPtr CreateRecReader([MarshalAs(UnmanagedType.LPStr)] String filename);
	[DllImport("RecFileIO")]//Destructor
	public static extern void DisposeRecReader();
	[DllImport("RecFileIO")]
	public static extern uint CallGetNumAgents(IntPtr reader);
	[DllImport("RecFileIO")]
	public static extern uint CallGetNumObstacles(IntPtr reader);
	
	//OBSTACLE DATA
	[DllImport("RecFileIO")]
	public static extern float GetObstacleXMin(IntPtr reader, uint index);
	[DllImport("RecFileIO")]
	public static extern float GetObstacleXMax(IntPtr reader, uint index);
	[DllImport("RecFileIO")]
	public static extern float GetObstacleZMin(IntPtr reader, uint index);
	[DllImport("RecFileIO")]
	public static extern float GetObstacleZMax(IntPtr reader, uint index);
	
	//STEP DATA
	[DllImport("RecFileIO")]
	public static extern float GetCenterOfMassX(IntPtr reader, uint agentIndex, float timeStamp);
	[DllImport("RecFileIO")]
	public static extern float GetCenterOfMassZ(IntPtr reader, uint agentIndex, float timeStamp);
	[DllImport("RecFileIO")]
	public static extern float GetOrientationX(IntPtr reader, uint agentIndex, float timeStamp);
	[DllImport("RecFileIO")]
	public static extern float GetOrientationZ(IntPtr reader, uint agentIndex, float timeStamp);
	[DllImport("RecFileIO")]
	public static extern float GetTargetX(IntPtr reader, uint agentIndex, float timeStamp);
	[DllImport("RecFileIO")]
	public static extern float GetTargetZ(IntPtr reader, uint agentIndex, float timeStamp);
	
	//Struct for holding an agent's data
	public struct Agent{
		public uint id;
		public AnimationCurve anim_x, anim_z, anim_o, anim_tx, anim_tz;
		public Transform unity_walker;
	}
	
	//IMPORTANT
	//AnimationCurve anim_x;
	//AnimationCurve anim_z;
	//AnimationCurve anim_o;
//	AnimationCurve anim_tx;
	//AnimationCurve anim_tz;
	//Use this to refer to the character.
	//public Transform target;
	//
	
	public Agent [] agents;
	public String filename;
	public GameObject prefab;

	// Use this for initialization
	void Start () {
		reader = CreateRecReader(filename);
		
		uint num_obstacles = CallGetNumObstacles(reader);
		float [] obstacle_data = new float[4];
		for(uint i = 0; i < num_obstacles; i++){
			//Store the obstacle's X and Z data.
			obstacle_data[0] = GetObstacleXMin(reader, i);
			obstacle_data[1] = GetObstacleXMax(reader, i);
			obstacle_data[2] = GetObstacleZMin(reader, i);
			obstacle_data[3] = GetObstacleZMax(reader, i);
			//Make a unit cube
			GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
			//Scale it
			Vector3 tmp = new Vector3();
			tmp.x = obstacle_data[1] - obstacle_data[0];
			tmp.y = 1.0f;
			tmp.z = obstacle_data[3] - obstacle_data[2];
			cube.transform.localScale = tmp;
			//Translate it
			tmp.x = obstacle_data[0] + (obstacle_data[1] - obstacle_data[0])/2;
			tmp.y = 18.5f;
			tmp.z = obstacle_data[2] + (obstacle_data[3] - obstacle_data[2])/2;
			cube.transform.localPosition = tmp;
		}
		uint num_agents = CallGetNumAgents(reader);
		agents = new Agent[num_agents];
		for(uint i = 0; i < num_agents; i++){
			agents[i].id = i;
			//For each agent, get the number of steps it takes then sample each step to build a position curve.
			//uint num_steps = CallGetNumStepsForAgent(reader, i);
			//Make some keyframes to store the position and time data
			//Keyframe [] keyframes_x = new Keyframe[num_steps];
			//Keyframe [] keyframes_z = new Keyframe[num_steps];
			//Keyframe [] keyframes_o = new Keyframe[num_steps];
			//Keyframe [] keyframes_tx = new Keyframe[num_steps];
			//Keyframe [] keyframes_tz = new Keyframe[num_steps];
			//for(uint j = 0; j < num_steps; j++){
				//Get agent's position and orientation
			//	float x = GetCenterOfMassX(reader, i, j);//Replace 0 with i
			//	float z = GetCenterOfMassZ(reader, i, j);
			//	float o = GetOrientation(reader, i, j);
				//Get time of agent's position
			//	float t = GetStepTime(reader, i, j);
				//Get target's position
			//	float tx = GetTargetX(reader, i, j);
			//	float tz = GetTargetZ(reader, i, j);
				//Add them to a keyframe
			//	keyframes_x[j] = new Keyframe(t, x);
			//	keyframes_z[j] = new Keyframe(t, z);
			//	keyframes_o[j] = new Keyframe(t, o);
			//	keyframes_tx[j] = new Keyframe(t,tx);
			//	keyframes_tz[j] = new Keyframe(t,tz);
			//}
			//Make the animation curve.
			//agents[i].anim_x = new AnimationCurve();
			//agents[i].anim_z = new AnimationCurve();
			//agents[i].anim_o = new AnimationCurve();
			//agents[i].anim_tx = new AnimationCurve();
			//agents[i].anim_tz = new AnimationCurve();
			//agents[i].anim_x.keys = keyframes_x;
			//agents[i].anim_z.keys = keyframes_z;
			//agents[i].anim_o.keys = keyframes_o;
			//agents[i].anim_tx.keys = keyframes_tx;
			//agents[i].anim_tz.keys = keyframes_tz;
		
			//Smooth out the animation curves
			//for(uint j = 0; j < num_steps; j++){
			//	agents[i].anim_x.SmoothTangents((int)j,0);
			//	agents[i].anim_z.SmoothTangents((int)j,0);
			//	agents[i].anim_o.SmoothTangents((int)j,0);
			//	agents[i].anim_tx.SmoothTangents((int)j,0);
			//	agents[i].anim_tz.SmoothTangents((int)j,0);
			//}
			//Finally, set the initial position of the walker (maybe).
			//Makes a new UnityWalker based on the prefab and makes the current agent hold onto that instance.
			agents[i].unity_walker = ((GameObject)Instantiate(prefab, new Vector3(0,18,0), Quaternion.identity)).transform;
		}
		
		//Rotate the agent so it faces the correct way initially
		//float offset = (float)Math.PI/2;
		//target.rotation = new Quaternion(0,1,0, offset + anim_o.Evaluate(0));
		
	}
	// Update is called once per frame
	void Update () {
		for(uint i = 0; i < agents.Length; i++){
			//Transform walker = agents[i].unity_walker;
			//float x = agents[i].anim_x.Evaluate(Time.time);
			float x = GetCenterOfMassX(reader, i, Time.time);
			//float z = agents[i].anim_z.Evaluate(Time.time);
			float z = GetCenterOfMassZ(reader, i, Time.time);
			agents[i].unity_walker.localPosition = new Vector3(x, 18, z);
			//Might be very dumb
			//float o = agents[i].anim_o.Evaluate(Time.time);
			float ox = GetOrientationX(reader, i, Time.time);
			//float o2 = agents[i].anim_o.Evaluate(Time.time - Time.fixedDeltaTime);
			float oz = GetOrientationZ(reader, i, Time.time);
			//Rotate the character by the change in its rotation from the last frame.
			//agents[i].unity_walker.Rotate(0,o-o2,0);
			float angle = Mathf.Atan2(ox, oz);
			Quaternion dfa = new Quaternion(0,angle,0,1);
			agents[i].unity_walker.localRotation = dfa ; // localRotation(0, angle, 0);
			
			
			//Update the head's target.
			float tx = GetTargetX(reader, i, Time.time);
			float ty = 18.0f;
			float tz = GetTargetZ(reader, i, Time.time);
			HeadLookController otherscript = agents[i].unity_walker.GetComponent<HeadLookController>();
			otherscript.target = new Vector3(tx, ty, tz);
		}
	}
}
