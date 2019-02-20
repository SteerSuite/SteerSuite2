//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//




#ifndef __STEERLIB_RECFILE_IO_H__
#define __STEERLIB_RECFILE_IO_H__

/// @file RecFileIO.h
/// @brief Defines the public interfaces for reading/writing rec files.

#include "Geometry.h"
#include "RecFileIOPrivate.h"

	/** 
	 * @brief The public interface for reading SteerSuite rec files (recordings of agents steering).
	 *

	 * Use this class to read SteerSuite rec files.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Reading rec files is straightforward:
	 *  -# Instantiate the RecFileReader class.
	 *  -# If you did not specify the filename in the constructor, call #open().
	 *  -# Use any of the functions to read information about agents, obstacles, 
	 *     camera views, or meta-data.  Data does not have to be read in any particular order.
	 *  -# When done, call #close().
	 *
	 * <h3> Notes </h3>
	 *
	 * If any information is read before calling #open(), or after calling #close(), the results are undefined.
	 * This particular error-check was omitted for performance reasons.
	 *
	 * Internally, the rec file is memory mapped, so randomly accessing data at
	 * different frames or timestamps should still perform well.
	 *
	 * There are two different sets of agent queries.  The first returns exact values for position
	 * and orientation for a given recorded frame.  The second returns interpolated values for 
	 * position and orientation for a given time stamp.
	 *
	 * Additionally, many of the queries have two forms, one using the Util geometry 
	 * data types (e.g., Util::Vector, Util::Point), and another using general floating-point passed by reference.
	 * If you want to use your own data types without passing each element by reference (which can
	 * result in annoying extra lines of less-readable code)
	 * you can create your own wrapper functions.  In most cases performance differences will be 
	 * negligible, but the general floating-point functions (passed by reference) will be slightly faster.
	 *
	 * In the current version of the rec file format, obstacles are static and cannot
	 * change over different frames.  Therefore, the parameter for frame number or time stamp in 
	 * getObstacleBoundsAtFrame() and getObstacleBoundsAtTime() are not used yet.
	 *
	 * @see 
	 *  - RecFileWriter to write rec files
	 *
	 */
	class RecFileReader : public RecFileReaderPrivate {
	public:
		/// @name Constructors and destructors
		//@{
		RecFileReader();
		/// This constructor opens the rec file, so there is no need to call open()
		RecFileReader(const std::string & filename);
		~RecFileReader();
		//@}

		/// @name Opening and closing
		//@{
		/// Returns true if the specified filename seems to be a valid rec file.
		static bool isAValidRecFile(const std::string & filename);
		/// Opens the rec file from the filename.
		void open(const std::string & filename);
		/// Closes the rec file, if one was open.
		void close();
		//@}

		/// @name Meta data queries
		//@{
		/// Returns true if a rec file is currently open in this class.
		bool isOpen() { return _opened; }
		/// Returns the version of the opened rec file, or 0 if no rec file is open.
		unsigned int getVersion() { return _version; }
		/// Returns the filename of the opened file, or an empty string if no rec file is open.
		const std::string & getFilename() { return _filename; }
		/// Returns the name of the test case associated with the rec file.
		const std::string & getTestCaseName() { return _testCaseName; }
		/// Returns the total number of frames in the rec file.
		unsigned int getNumFrames();
		/// Returns the number of agents in the rec file.
		unsigned int getNumAgents();
		/// Returns the number of obstacles in the rec file.
		unsigned int getNumObstacles();
		/// Returns the number of suggested camera views in the rec file.
		unsigned int getNumCameraViews();
		/// Returns parameters of the particular camera view indexed by cameraIndex.
		void getCameraView( unsigned int cameraIndex, float &origx, float &origy, float &origz, float &lookatx, float &lookaty, float &lookatz);
		/// Returns the time stamp for a particular frame.
		float getTimeStampForFrame( unsigned int frameNumber );
		/// Returns the total time elapsed between the first and last frames of the rec file.
		float getTotalElapsedTime();
		/// Returns the time elapsed between any two frames of the rec file.
		float getElapsedTimeBetweenFrames( unsigned int startFrame, unsigned int endFrame);
		//@}

		/// @name Agent/Obstacle queries by frame number
		/// @brief These functions give the exact data that was recorded on a particular frame.
		//@{
		/// Returns the agent location at the specified frame number.
		void getAgentLocationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &posx, float &posy, float &posz );
		/// Returns the agent location at the specified frame number.
		inline Util::Point getAgentLocationAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Point p; getAgentLocationAtFrame(agentIndex, frameNumber, p.x, p.y, p.z); return p;}

		/// Returns the agent orientation at the specified frame number
		void getAgentOrientationAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &dirx, float &diry, float &dirz );
		/// Returns the agent orientation at the specified frame number
		inline Util::Vector getAgentOrientationAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Vector v; getAgentOrientationAtFrame(agentIndex, frameNumber, v.x, v.y, v.z); return v;}

		/// Returns the agent goal location at the specified frame number
		void getAgentGoalAtFrame( unsigned int agentIndex, unsigned int frameNumber, float &goalx, float &goaly, float &goalz );
		/// Returns the agent goal location at the specified frame number
		inline Util::Point getAgentGoalAtFrame( unsigned int agentIndex, unsigned int frameNumber ) { Util::Point p; getAgentGoalAtFrame(agentIndex, frameNumber, p.x, p.y, p.z); return p;}

		/// Returns the agent radius at the specified frame number.
		float getAgentRadiusAtFrame( unsigned int agentIndex, unsigned int frameNumber );

		/// Returns true if the agent is enabled at the specified frame number.
		bool isAgentEnabledAtFrame( unsigned int agentIndex, unsigned int frameNumber );

		/// Returns the obstacle bounds of an obstacle at the specified frame number.
		void getObstacleBoundsAtFrame( unsigned int obstacleIndex, unsigned int frameNumber, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax );
		/// Returns the obstacle bounds of an obstacle at the specified frame number.
		inline Util::AxisAlignedBox getObstacleBoundsAtFrame( unsigned int obstacleIndex, unsigned int frameNumber ) { Util::AxisAlignedBox b; getObstacleBoundsAtFrame(obstacleIndex, frameNumber, b.xmin, b.xmax, b.ymin, b.ymax, b.zmin, b.zmax); return b; }
		//@}

		/// @name Agent/Obstacle queries by timestamp
		/// @brief These functions interpolate locations and orientations between frames, and they consider both surrounding frames to determine the agent's goal or enabled status appropriately.
		//@{
		/// Returns the agent location at the specified time stamp.
		void getAgentLocationAtTime( unsigned int agentIndex, float time, float &posx, float &posy, float &posz );
		/// Returns the agent location at the specified time stamp.
		inline Util::Point getAgentLocationAtTime( unsigned int agentIndex, float time ) { Util::Point p; getAgentLocationAtTime(agentIndex, time, p.x, p.y, p.z); return p;}

		/// Returns the agent orientation at the specified time stamp
		void getAgentOrientationAtTime( unsigned int agentIndex, float time, float &dirx, float &diry, float &dirz );
		/// Returns the agent orientation at the specified time stamp
		inline Util::Vector getAgentOrientationAtTime( unsigned int agentIndex, float time ) { Util::Vector v; getAgentOrientationAtTime(agentIndex, time, v.x, v.y, v.z); return v;}

		/// Returns the agent goal location at the specified time stamp
		void getAgentGoalAtTime( unsigned int agentIndex, float time, float &goalx, float &goaly, float &goalz );
		/// Returns the agent goal location at the specified time stamp
		inline Util::Point getAgentGoalAtTime( unsigned int agentIndex, float time ) { Util::Point p; getAgentGoalAtTime(agentIndex, time, p.x, p.y, p.z); return p;}

		/// Returns the agent radius at the specified time stamp.
		float getAgentRadiusAtTime( unsigned int agentIndex, float time );

		/// Returns true if the agent is enabled at the specified time stamp.
		bool isAgentEnabledAtTime( unsigned int agentIndex, float time );

		/// Returns the obstacle bounds of an obstacle at the specified time stamp.
		void getObstacleBoundsAtTime( unsigned int obstacleIndex, float time, float &xmin, float &xmax, float &ymin, float &ymax, float &zmin, float &zmax );
		/// Returns the obstacle bounds of an obstacle at the specified time stamp.
		inline Util::AxisAlignedBox getObstacleBoundsAtTime( unsigned int obstacleIndex, float time ) { Util::AxisAlignedBox b; getObstacleBoundsAtTime(obstacleIndex, time, b.xmin, b.xmax, b.ymin, b.ymax, b.zmin, b.zmax); return b; }
		//@}
	};


#endif
