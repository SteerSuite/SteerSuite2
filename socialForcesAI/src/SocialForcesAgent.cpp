//
// Copyright (c) 2009-2018 Brandon Haworth, Glen Berseth, Muhammad Usman, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//




#include "astar/AStarLite.h"
#include "planning/SteerSuiteGridAStar.h"
#include "SocialForcesAgent.h"
#include "SocialForcesAIModule.h"
#include "SocialForces_Parameters.h"
// #include <math.h>

// #include "util/Geometry.h"

/// @file SocialForcesAgent.cpp
/// @brief Implements the SocialForcesAgent class.

#undef min
#undef max

#define AGENT_MASS 1.0f

using namespace Util;
using namespace SocialForcesGlobals;
using namespace SteerLib;

// #define _DEBUG_ENTROPY 1

SocialForcesAgent::SocialForcesAgent()
{
	_SocialForcesParams.sf_acceleration = sf_acceleration;
	_SocialForcesParams.sf_personal_space_threshold = sf_personal_space_threshold;
	_SocialForcesParams.sf_agent_repulsion_importance = sf_agent_repulsion_importance;
	_SocialForcesParams.sf_query_radius = sf_query_radius;
	_SocialForcesParams.sf_body_force = sf_body_force;
	_SocialForcesParams.sf_agent_body_force = sf_agent_body_force;
	_SocialForcesParams.sf_sliding_friction_force = sf_sliding_friction_force;
	_SocialForcesParams.sf_agent_b = sf_agent_b;
	_SocialForcesParams.sf_agent_a = sf_agent_a;
	_SocialForcesParams.sf_wall_b = sf_wall_b;
	_SocialForcesParams.sf_wall_a = sf_wall_a;
	_SocialForcesParams.sf_max_speed = sf_max_speed;

	_enabled = false;
}

SocialForcesAgent::~SocialForcesAgent()
{
	// std::cout << this << " is being deleted" << std::endl;
	/*
	if (this->enabled())
	{
		Util::AxisAlignedBox bounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.0f, _position.z-_radius, _position.z+_radius);
		// getSimulationEngine()->getSpatialDatabase()->removeObject( this, bounds);
	}*/
	// std::cout << "Someone is removing an agent " << std::endl;
}

SteerLib::EngineInterface * SocialForcesAgent::getSimulationEngine()
{
	return _gEngine;
}

void SocialForcesAgent::setParameters(Behaviour behave)
{
	this->_SocialForcesParams.setParameters(behave);
}

void SocialForcesAgent::disable()
{
	// DO nothing for now
	// if we tried to disable a second time, most likely we accidentally ignored that it was disabled, and should catch that error.
	// std::cout << "this agent is being disabled " << this << std::endl;
	assert(_enabled==true);


	//  1. remove from database
	AxisAlignedBox b = AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	getSimulationEngine()->getSpatialDatabase()->removeObject(dynamic_cast<SpatialDatabaseItemPtr>(this), b);

	//  2. set enabled = false
	_enabled = false;


}

void SocialForcesAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
	// compute the "old" bounding box of the agent before it is reset.  its OK that it will be invalid if the agent was previously disabled
	// because the value is not used in that case.
	// std::cout << "resetting agent " << this << std::endl;
	_waypoints.clear();
	_midTermPath.clear();

	Util::AxisAlignedBox oldBounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.5f, _position.z-_radius, _position.z+_radius);


	// initialize the agent based on the initial conditions
	/*
	position_ = Vector2(initialConditions.position.x, initialConditions.position.z);
	radius_ = initialConditions.radius;
	velocity_ = normalize(Vector2(initialConditions.direction.x, initialConditions.direction.z));
	velocity_ = velocity_ * initialConditions.speed;
*/
	// initialize the agent based on the initial conditions
	_position = initialConditions.position;
	_last_position  = _position;
	_forward = normalize(initialConditions.direction);
	_radius = initialConditions.radius;
	_velocity = initialConditions.speed * _forward;
	// std::cout << "inital colour of agent " << initialConditions.color << std::endl;
	if ( initialConditions.colorSet == true )
	{
		this->_color = initialConditions.color;
	}
	else
	{
		this->_color = Util::gBlue;
	}

	// compute the "new" bounding box of the agent
	Util::AxisAlignedBox newBounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.5f, _position.z-_radius, _position.z+_radius);


	if (!_enabled) {
		// if the agent was not enabled, then it does not already exist in the database, so add it.
		getSimulationEngine()->getSpatialDatabase()->addObject( dynamic_cast<SpatialDatabaseItemPtr>(this), newBounds);
	}
	else 
	{
		// if the agent was enabled, then the agent already existed in the database, so update it instead of adding it.
		getSimulationEngine()->getSpatialDatabase()->updateObject( dynamic_cast<SpatialDatabaseItemPtr>(this), oldBounds, newBounds);
		// engineInfo->getSpatialDatabase()->updateObject( this, oldBounds, newBounds);
	}

	_enabled = true;

	if (initialConditions.goals.size() == 0)
	{
		throw Util::GenericException("No goals were specified!\n");
	}

	while (!_goalQueue.empty())
	{
		_goalQueue.pop();
	}

	for (unsigned int i=0; i<initialConditions.goals.size(); i++) {
		if (initialConditions.goals[i].goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET||
				initialConditions.goals[i].goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL)
		{
			if (initialConditions.goals[i].targetIsRandom)
			{
				// if the goal is random, we must randomly generate the goal.
				// std::cout << "assigning random goal" << std::endl;
				SteerLib::AgentGoalInfo _goal;
				_goal.targetLocation = gSpatialDatabase->randomPositionWithoutCollisions(1.0f, true);
				_goalQueue.push(_goal);
				_currentGoal.targetLocation = _goal.targetLocation;
			}
			else
			{
				_goalQueue.push(initialConditions.goals[i]);
			}
			if (_goalQueue.size()==1) {
				_currentGoal = initialConditions.goals[i];
			}
		}
		else {
			throw Util::GenericException("Unsupported goal type; FootstepAgent only supports GOAL_TYPE_SEEK_STATIC_TARGET and GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL.");
		}
	}

	//runLongTermPlanning(this->_goalQueue.front().targetLocation, false);

	_waypoints.clear();
	_currentWaypointIndex = 0;
	_midTermPathSize = 0;
	lostSightOfTargetFrames = 0;
	_localTargetLocation = _currentGoal.targetLocation;

	Util::Vector goalDirection;
	goalDirection = normalize( _localTargetLocation - position());

	_prefVelocity =
			(
				(
					(
						Util::Vector(goalDirection.x, 0.0f, goalDirection.z) *
						PERFERED_SPEED
					)
				- velocity()
				)
				/
				_SocialForcesParams.sf_acceleration
			)
			*
			MASS;

	// _velocity = _prefVelocity;
#ifdef _DEBUG_ENTROPY
	std::cout << "goal direction is: " << goalDirection << " prefvelocity is: " << prefVelocity_ <<
			" and current velocity is: " << velocity_ << std::endl;
#endif

	assert(_forward.length()!=0.0f);
	assert(_goalQueue.size() != 0);
	assert(_radius != 0.0f);
}


void SocialForcesAgent::calcNextStep(float dt)
{

}

std::pair<float, Util::Point> minimum_distance(Util::Point l1, Util::Point l2, Util::Point p)
{
  // Return minimum distance between line segment vw and point p
  float lSq = (l1 - l2).lengthSquared();  // i.e. |l2-l1|^2 -  avoid a sqrt
  if (lSq == 0.0)
	  return std::make_pair((p - l2).length(),l1 );   // l1 == l2 case
  // Consider the line extending the segment, parameterized as l1 + t (l2 - l1).
  // We find projection of point p onto the line.
  // It falls where t = [(p-l1) . (l2-l1)] / |l2-l1|^2
  const float t = dot(p - l1, l2 - l1) / lSq;
  if (t < 0.0)
  {
	  return std::make_pair((p - l1).length(), l1);       // Beyond the 'l1' end of the segment
  }
  else if (t > 1.0)
  {
	  return std::make_pair((p - l2).length(), l2);  // Beyond the 'l2' end of the segment
  }
  const Util::Point projection = l1 + t * (l2 - l1);  // Projection falls on the segment
  return std::make_pair((p - projection).length(), projection) ;
}


Util::Vector SocialForcesAgent::calcProximityForce(float dt)
{

	Util::Vector agent_repulsion_force = Util::Vector(0,0,0);

	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
		getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
				_position.x-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.x+(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z+(this->_radius + _SocialForcesParams.sf_query_radius),
				dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SteerLib::AgentInterface * tmp_agent;
	SteerLib::ObstacleInterface * tmp_ob;
	Util::Vector away = Util::Vector(0,0,0);
	Util::Vector away_obs = Util::Vector(0,0,0);

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin();  neighbour != _neighbors.end();  neighbour++)
	// for (int a =0; a < tmp_agents.size(); a++)
	{
		if ( (*neighbour)->isAgent() )
		{
			tmp_agent = dynamic_cast<SteerLib::AgentInterface *>(*neighbour);

			// direction away from other agent
			Util::Vector away_tmp = normalize(position() - tmp_agent->position());
			// std::cout << "away_agent_tmp vec" << away_tmp << std::endl;
			// Scale force
			// std::cout << "the exp of agent distance is " << exp((radius() + tmp_agent->radius()) -
				//	(position() - tmp_agent->position()).length()) << std::endl;


			// away = away + (away_tmp * ( radius() / ((position() - tmp_agent->position()).length() * B) ));
			away = away +
					(
						away_tmp
						*
						(
							_SocialForcesParams.sf_agent_a
							*
							exp(
								(
									(
										(
											this->radius()
											+
											tmp_agent->radius()
										)
										-
										(
											this->position()
											-
											tmp_agent->position()
										).length()
									)
									/
									_SocialForcesParams.sf_agent_b
								)
							)


						)
						*
						dt
					);
			/*
			std::cout << "agent " << this->id() << " away this far " << away <<
					" distance " << exp(
							(
								(
									(
										radius()
										+
										tmp_agent->radius()
									)
									-
									(
										position()
										-
										tmp_agent->position()
									).length()
								)
								/
								_SocialForcesParams.sf_agent_b
							)
						) << std::endl;
						*/
		}
		else
		{
			// It is an obstacle
			tmp_ob = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbour);
			CircleObstacle * obs_cir = dynamic_cast<SteerLib::CircleObstacle *>(tmp_ob);
			if ( obs_cir != NULL && USE_CIRCLES)
			{
				// std::cout << "Found circle obstacle" << std::endl;
				Util::Vector away_tmp = normalize(position() - obs_cir->position());
				away = away +
						(
							away_tmp
							*
							(
									_SocialForcesParams.sf_wall_a
								*
								exp(
									(
										(
											(
												this->radius()
												+
												obs_cir->radius()
											)
											-
											(
												this->position()
												-
												obs_cir->position()
											).length()
										)
										/
										_SocialForcesParams.sf_wall_b
									)
								)


							)
							*
							dt
						);
			}
			else
			{
				Util::Vector wall_normal = calcWallNormal( tmp_ob );
				std::pair<Util::Point,Util::Point> line = calcWallPointsFromNormal(tmp_ob, wall_normal);
				// Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
					// 	(line.first.z+line.second.z)/2);
				std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
				// wall distance

				Util::Vector away_obs_tmp = normalize(position() - min_stuff.second);
				// std::cout << "away_obs_tmp vec" << away_obs_tmp << std::endl;
				// away_obs = away_obs + ( away_obs_tmp * ( radius() / ((position() - min_stuff.second).length() * B ) ) );
				away_obs = away_obs +
						(
							away_obs_tmp
							*
							(
								_SocialForcesParams.sf_wall_a
								*
								exp(
									(
										(
											(this->radius()) -
											(
												this->position()
												-
												min_stuff.second
											).length()
										)
										/
										_SocialForcesParams.sf_wall_b
									)
								)
							)
							*
							dt
						);
			}
		}

	}
	return away + away_obs;
}

Util::Vector SocialForcesAgent::calcRepulsionForce(float dt)
{
#ifdef _DEBUG_
	std::cout << "wall repulsion; " << calcWallRepulsionForce(dt) << " agent repulsion " <<
			(_SocialForcesParams.sf_agent_repulsion_importance * calcAgentRepulsionForce(dt)) << std::endl;
#endif
	return calcWallRepulsionForce(dt) + (_SocialForcesParams.sf_agent_repulsion_importance * calcAgentRepulsionForce(dt));
}

Util::Vector SocialForcesAgent::calcAgentRepulsionForce(float dt)
{

	Util::Vector agent_repulsion_force = Util::Vector(0,0,0);

	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
		getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
				_position.x-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.x+(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z+(this->_radius + _SocialForcesParams.sf_query_radius),
				(this));

	SteerLib::AgentInterface * tmp_agent;

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin();  neighbour != _neighbors.end();  neighbour++)
	// for (int a =0; a < tmp_agents.size(); a++)
	{
		if ( (*neighbour)->isAgent() )
		{
			tmp_agent = dynamic_cast<SteerLib::AgentInterface *>(*neighbour);
		}
		else
		{
			continue;
		}
		if ( ( id() != tmp_agent->id() ) &&
				(tmp_agent->computePenetration(this->position(), this->radius()) > 0.000001)
			)
		{
		agent_repulsion_force = agent_repulsion_force +
			( tmp_agent->computePenetration(this->position(), this->radius()) * _SocialForcesParams.sf_agent_body_force * dt) *
			normalize(position() - tmp_agent->position());
			// normalized tangential force
		/*
			agent_repulsion_force = agent_repulsion_force +
					(
						(
							(-1*position()) - tmp_agent->position()
						)
						/
						(
							(-1*position()) - tmp_agent->position()
						).length()

					)*0.2;
					*/
			//TODO this can have some funny behaviour is velocity == 0
			Util::Vector tangent = cross(cross(tmp_agent->position() - position(), velocity()),
					tmp_agent->position() - position());
			tangent = tangent /  tangent.length();
			float  tanget_v_diff = dot(tmp_agent->velocity() - velocity(),  tangent);
			// std::cout << "Velocity diff is " << tanget_v_diff << " tangent is " << tangent <<
				//	" velocity is " << velocity() << std::endl;
			agent_repulsion_force = agent_repulsion_force +
			(_SocialForcesParams.sf_sliding_friction_force * dt *
				(
					tmp_agent->computePenetration(this->position(), this->radius())
				) * tangent * tanget_v_diff

			);
		}

	}
	return agent_repulsion_force;
}

Util::Vector SocialForcesAgent::calcWallRepulsionForce(float dt)
{

	Util::Vector wall_repulsion_force = Util::Vector(0,0,0);


	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
		getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
				_position.x-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.x+(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z-(this->_radius + _SocialForcesParams.sf_query_radius),
				_position.z+(this->_radius + _SocialForcesParams.sf_query_radius),
				dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SteerLib::ObstacleInterface * tmp_ob;

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin();  neighbour != _neighbors.end();  neighbour++)
	// for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = _neighbors.begin();  tmp_o != _neighbors.end();  tmp_o++)
	{
		if ( !(*neighbour)->isAgent() )
		{
			tmp_ob = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbour);
		}
		else
		{
			continue;
		}
		if ( tmp_ob->computePenetration(this->position(), this->radius()) > 0.000001 )
		{
			CircleObstacle * cir_obs = dynamic_cast<SteerLib::CircleObstacle *>(tmp_ob);
			if ( cir_obs != NULL && USE_CIRCLES )
			{
				// std::cout << "Intersected circle obstacle" << std::endl;
				Util::Vector wall_normal = position() - cir_obs->position();
				// wall distance
				float distance = wall_normal.length() - cir_obs->radius();

				wall_normal = normalize(wall_normal);
				wall_repulsion_force = wall_repulsion_force +
					((
						(
							(
									wall_normal
							)
							*
							(
								radius() +
								_SocialForcesParams.sf_personal_space_threshold -
								(
									distance
								)
							)
						)
						/
						distance
					)* _SocialForcesParams.sf_body_force * dt);

				// tangential force
				// std::cout << "wall tangent " << rightSideInXZPlane(wall_normal) <<
					// 	" dot is " << dot(forward(),  rightSideInXZPlane(wall_normal)) <<
						// std::endl;
				wall_repulsion_force = wall_repulsion_force +
				(
					dot(forward(),  rightSideInXZPlane(wall_normal))
					*
					rightSideInXZPlane(wall_normal)
					*
					cir_obs->computePenetration(this->position(), this->radius())
				)* _SocialForcesParams.sf_sliding_friction_force * dt;

			}
			else
			{
				Util::Vector wall_normal = calcWallNormal( tmp_ob );
				std::pair<Util::Point,Util::Point> line = calcWallPointsFromNormal(tmp_ob, wall_normal);
				// Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
					// 	(line.first.z+line.second.z)/2);
				std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
				// wall distance
				wall_repulsion_force = wall_repulsion_force +
					((
						(
							(
									wall_normal
							)
							*
							(
								radius() +
								_SocialForcesParams.sf_personal_space_threshold -
								(
									min_stuff.first
								)
							)
						)
						/
						min_stuff.first
					)* _SocialForcesParams.sf_body_force * dt);
				// tangential force
				// std::cout << "wall tangent " << rightSideInXZPlane(wall_normal) <<
					// 	" dot is " << dot(forward(),  rightSideInXZPlane(wall_normal)) <<
						// std::endl;
				wall_repulsion_force = wall_repulsion_force +
				(
					dot(forward(),  rightSideInXZPlane(wall_normal))
					*
					rightSideInXZPlane(wall_normal)
					*
					tmp_ob->computePenetration(this->position(), this->radius())
				)* _SocialForcesParams.sf_sliding_friction_force * dt;
			}
		}

	}
	return wall_repulsion_force;
}

std::pair<Util::Point, Util::Point> SocialForcesAgent::calcWallPointsFromNormal(SteerLib::ObstacleInterface* obs, Util::Vector normal)
{
	Util::AxisAlignedBox box = obs->getBounds();
	if ( normal.z == 1)
	{
		return std::make_pair(Util::Point(box.xmin,0,box.zmax), Util::Point(box.xmax,0,box.zmax));
		// Ended here;
	}
	else if ( normal.z == -1 )
	{
		return std::make_pair(Util::Point(box.xmin,0,box.zmin), Util::Point(box.xmax,0,box.zmin));
	}
	else if ( normal.x == 1)
	{
		return std::make_pair(Util::Point(box.xmax,0,box.zmin), Util::Point(box.xmax,0,box.zmax));
	}
	else // normal.x == -1
	{
		return std::make_pair(Util::Point(box.xmin,0,box.zmin), Util::Point(box.xmin,0,box.zmax));
	}
}

/**
 * Basically What side of the obstacle is the agent on use that as the normal
 * DOES NOT SUPPORT non-axis-aligned boxes
 *
 *
 * 			   \		   /
 * 				\		  /
 * 				 \	 a	 /
 *				  \		/
 * 					 _
 * 			a		| |       a
 * 					 -
 * 				  /     \
 * 				 /   a   \
 * 				/	      \
 * 			   /	       \
 *
 *
 */
Util::Vector SocialForcesAgent::calcWallNormal(SteerLib::ObstacleInterface* obs)
{
	Util::AxisAlignedBox box = obs->getBounds();
	if ( position().x > box.xmax )
	{
		if ( position().z > box.zmax)
		{
			if ( abs(position().z - box.zmax ) >
				abs( position().x - box.xmax) )
			{
				return Util::Vector(0, 0, 1);
			}
			else
			{
				return Util::Vector(1, 0, 0);
			}

		}
		else if ( position().z < box.zmin )
		{
			if ( abs(position().z - box.zmin ) >
				abs( position().x - box.xmax) )
			{
				return Util::Vector(0, 0, -1);
			}
			else
			{
				return Util::Vector(1, 0, 0);
			}

		}
		else
		{ // in between zmin and zmax
			return Util::Vector(1, 0, 0);
		}

	}
	else if ( position().x < box.xmin )
	{
		if ( position().z > box.zmax )
		{
			if ( abs(position().z - box.zmax ) >
				abs( position().x - box.xmin) )
			{
				return Util::Vector(0, 0, 1);
			}
			else
			{
				return Util::Vector(-1, 0, 0);
			}

		}
		else if ( position().z < box.zmin )
		{
			if ( abs(position().z - box.zmin ) >
				abs( position().x - box.xmin) )
			{
				return Util::Vector(0, 0, -1);
			}
			else
			{
				return Util::Vector(-1, 0, 0);
			}

		}
		else
		{ // in between zmin and zmax
			return Util::Vector(-1, 0, 0);
		}
	}
	else // between xmin and xmax
	{
		if ( position().z > box.zmax )
		{
			return Util::Vector(0, 0, 1);
		}
		else if ( position().z < box.zmin)
		{
			return Util::Vector(0, 0, -1);
		}
		else
		{ // What do we do if the agent is inside the wall?? Lazy Normal
			return calcObsNormal( obs );
		}
	}

}

/**
 * Treats Obstacles as a circle and calculates normal
 */
Util::Vector SocialForcesAgent::calcObsNormal(SteerLib::ObstacleInterface* obs)
{
	Util::AxisAlignedBox box = obs->getBounds();
	Util::Point obs_centre = Util::Point((box.xmax+box.xmin)/2, (box.ymax+box.ymin)/2,
			(box.zmax+box.zmin)/2);
	return normalize(position() - obs_centre);
}


void SocialForcesAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
	// std::cout << "_SocialForcesParams.rvo_max_speed " << _SocialForcesParams._SocialForcesParams.rvo_max_speed << std::endl;
	if (gShowStats)
		Util::AutomaticFunctionProfiler profileThisFunction( &SocialForcesGlobals::gPhaseProfilers->aiProfiler );
	if (!enabled())
	{
		return;
	}

    // Store old bounds
    Util::AxisAlignedBox oldBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);

	_last_position = position();

	if (frameNumber == 1)
	{
#ifdef _DEBUG_1
	std::cout << "Starting in frameNumber 1\n";
#endif
		runLongTermPlanningAStarLite();
		runMidTermPlanningAStarLite();

		// Find next visibile waypoint in midterm path and set that index and corresponding local target
		FindAndSetFurthestVisibleLocalTarget();
#ifdef _DEBUG_1
	std::cout << "Done in frameNumber 1\n";
	std::cout << "number of neighbors is " << _neighbors.size() << "\n";
#endif
	}

	// If we lose sight of the current visibile local target we have been pushed or made an off path avoidance maneuver 
	if (!this->hasLineOfSightTo(_localTargetLocation))
    {
    	// In some cases there is a frame to frame flip-flop between seeing and not seeing the next target
    	// This adds an almost faux behavioural action to losing sight of a target, ie it takes a moment to decide to replan
    	lostSightOfTargetFrames++;
    	if(lostSightOfTargetFrames > 5) {
    		// To get out of concavities after pushing we really need to update our long term plan to handle all edge cases
	        runLongTermPlanningAStarLite();
			runMidTermPlanningAStarLite();

			// Find next visibile waypoint in midterm path and set that index and corresponding local target
			FindAndSetFurthestVisibleLocalTarget();

			lostSightOfTargetFrames = 0;
		}
    }

    SteerLib::AgentGoalInfo goalInfo = _goalQueue.front();

	// Update midterm path if we are at end
	if(_midTermPathSize - _currentWaypointIndex - 1 == 0) 
	{	
		if (_gEngine->isAgentSelected(this))
		{
			std::cout << "updateAI:\tUpdate midterm path if we are at end\n";
			std::cout << "updateAI:\t_midTermPathSize:\t"<< _midTermPathSize << "\n";
			std::cout << "updateAI:\t_currentWaypointIndex:\t"<< _currentWaypointIndex << "\n";
		} 
		runLongTermPlanningAStarLite();
		runMidTermPlanningAStarLite();
	} 
	// Handle normal midterm path following behaviour
	else if ( ! _midTermPath.empty() && (!this->hasLineOfSightTo(goalInfo.targetLocation)) )
	{
		// UPdate target along midtermpath if we reach the latest
		// move midterm index to next one
		// Then move along the midterm path to find the last actully visible one
		// Set local target to the furthest visibile taget in the midterm plan
		if ((position() - _localTargetLocation).lengthSquared() <= (_radius * _radius)) 
		{
			// Increment waypoint index
			_currentWaypointIndex++;
			
			// Find next visibile waypoint in midterm path and set that index and corresponding local target
			FindAndSetFurthestVisibleLocalTarget();

			if (_gEngine->isAgentSelected(this))
			{
				std::cout << "updateAI:\tNew local Target: " << _localTargetLocation << "\n";
			} 
		}
	}
	// If we can see the current goal just head for it
	else if (this->hasLineOfSightTo(goalInfo.targetLocation))
	{
		if (_gEngine->isAgentSelected(this))
		{
			std::cout << "updateAI:\tCan see goal, New local Target: " << goalInfo.targetLocation << "\n";
		} 

		_localTargetLocation = goalInfo.targetLocation;
	}

	Util::Vector goalDirection;
    goalDirection = normalize(_localTargetLocation - position());

    // the normalize function returns (NaN, NaN, NaN) if the input vector is (0,0,0), handle this here
    if (goalDirection.x != goalDirection.x) 
    {
    	if (_gEngine->isAgentSelected(this))
		{
    		std::cout << "goalDirection is NaN.  Most likely _localTargetLocation and position() are zero vectors\n";
    	}
    	goalDirection = _localTargetLocation - position();	
    }

	// _prefVelocity = goalDirection * PERFERED_SPEED;
	Util::Vector prefForce = (((goalDirection * PERFERED_SPEED) - velocity()) / (_SocialForcesParams.sf_acceleration/dt)); //assumption here
	if (getSimulationEngine()->isAgentSelected(this))
	{
		std::cout << id() << ": _localTargetLocation: " << _localTargetLocation << "\n";
		std::cout << id() << ": position(): " << position() << "\n";
		std::cout << id() << ": goalDirection: " << goalDirection << "\n";
		std::cout << id() << ": PERFERED_SPEED: " << PERFERED_SPEED << "\n";
		std::cout << id() << ": velocity(): " << velocity() << "\n";
		std::cout << id() << ": _SocialForcesParams.sf_acceleration: " << _SocialForcesParams.sf_acceleration << "\n";
		std::cout << id() << ": dt: " << dt << "\n";
	}

	prefForce = prefForce + velocity();
	// _velocity = prefForce;

	Util::Vector repulsionForce = calcRepulsionForce(dt);
	if ( repulsionForce.x != repulsionForce.x)
	{
		if (_gEngine->isAgentSelected(this))
		{
			std::cout << "Found some nan" << std::endl;
		}
		repulsionForce = velocity();
		// throw GenericException("SocialForces numerical issue");
	}
	Util::Vector proximityForce = calcProximityForce(dt);
// #define _DEBUG_ 1
#ifdef _DEBUG_
	std::cout << "agent" << id() << " repulsion force " << repulsionForce << std::endl;
	std::cout << "agent" << id() << " proximity force " << proximityForce << std::endl;
	std::cout << "agent" << id() << " pref force " << prefForce << std::endl;
#endif
	// _velocity = _newVelocity;
	int alpha=1;
	if ( repulsionForce.length() > 0.0)
	{
		alpha=0;
	}

	if (getSimulationEngine()->isAgentSelected(this))
	{
		std::cout << id() << ": prefForce: " << prefForce << "\n";
		std::cout << id() << ": repulsionForce: " << repulsionForce << "\n";
		std::cout << id() << ": proximityForce: " << proximityForce << "\n";
	}

	_velocity = (prefForce) + repulsionForce + proximityForce;


	// _velocity = (prefForce);
	// _velocity = velocity() + repulsionForce + proximityForce;
	
	if (getSimulationEngine()->isAgentSelected(this))
	{
		std::cout << id() << ": _velocity before clamp: " << _velocity << "\n";
		std::cout << id() << ": velocity() before clam: " << velocity() << "\n";
	}

	_velocity = clamp(velocity(), _SocialForcesParams.sf_max_speed);
	_velocity.y=0.0f;
#ifdef _DEBUG_
	std::cout << "agent" << id() << " speed is " << velocity().length() << std::endl;
#endif

	if (getSimulationEngine()->isAgentSelected(this))
	{
		std::cout << id() << ": _position: " << _position << "\n";
		std::cout << id() << ": velocity(): " << velocity() << "\n";
		std::cout << id() << ": dt: " << dt << "\n";
	}

	_position = position() + (velocity() * dt);
	// A grid database update should always be done right after the new position of the agent is calculated
	/*
	 * Or when the agent is removed for example its true location will not reflect its location in the grid database.
	 * Not only that but this error will appear random depending on how well the agent lines up with the grid database
	 * boundaries when removed.
	 */
	// std::cout << "Updating agent" << this->id() << " at " << this->position() << std::endl;
	Util::AxisAlignedBox newBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	if (getSimulationEngine()->isAgentSelected(this))
	{
		std::cout << id() << ": _position.x: " << _position.x << "\n";
		std::cout << id() << ": _radius: " << _radius << "\n";
		std::cout << id() << ": newBounds: " << newBounds << "\n";
	}

	getSimulationEngine()->getSpatialDatabase()->updateObject( this, oldBounds, newBounds);

/*
	if ( ( !_waypoints.empty() ) && (_waypoints.front() - position()).length() < radius()*WAYPOINT_THRESHOLD_MULTIPLIER)
	{
		_waypoints.erase(_waypoints.begin());
	}
	*/
	/*
	 * Now do the conversion from SocialForcesAgent into the SteerSuite coordinates
	 */
	// _velocity.y = 0.0f;

	if ((goalInfo.targetLocation - position()).length() < radius()*GOAL_THRESHOLD_MULTIPLIER ||
			(goalInfo.goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL &&
					Util::boxOverlapsCircle2D(goalInfo.targetRegion.xmin, goalInfo.targetRegion.xmax,
							goalInfo.targetRegion.zmin, goalInfo.targetRegion.zmax, this->position(), this->radius())))
	{
		_goalQueue.pop();
		// std::cout << "Made it to a goal" << std::endl;
		if (_goalQueue.size() != 0)
		{
			// in this case, there are still more goals, so start steering to the next goal.
			goalDirection = _goalQueue.front().targetLocation - _position;
			_prefVelocity = Util::Vector(goalDirection.x, 0.0f, goalDirection.z);
		}
		else
		{
			// in this case, there are no more goals, so disable the agent and remove it from the spatial database.
			disable();
			return;
		}
	}

	// Hear the 2D solution from RVO is converted into the 3D used by SteerSuite
	// _velocity = Vector(velocity().x, 0.0f, velocity().z);
	if ( velocity().lengthSquared() > 0.0 )
	{
		// Only assign forward direction if agent is moving
		// Otherwise keep last forward
		_forward = normalize(_velocity);
	}
	// _position = _position + (_velocity * dt);

}


void SocialForcesAgent::draw()
{
#ifdef ENABLE_GUI
	AgentInterface::draw();
	// if the agent is selected, do some annotations just for demonstration

	if (getSimulationEngine()->isAgentSelected(this))
	{
		// draw goal
		DrawLib::drawFlag( _localTargetLocation, gBlack, 4.0 );

		for (int nn = 0; nn < _midTermPathSize - 1 && _midTermPathSize > 1; nn++)
		{
			Util::Point waypointStart;
			Util::Point waypointEnd;
			gSpatialDatabase->getLocationFromIndex(_midTermPath[nn],waypointStart);
			gSpatialDatabase->getLocationFromIndex(_midTermPath[nn+1],waypointEnd);
			DrawLib::drawLine(waypointStart, waypointEnd, gRed);
		}
	}

#ifdef DRAW_COLLISIONS
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors, _position.x-(this->_radius * 3), _position.x+(this->_radius * 3),
			_position.z-(this->_radius * 3), _position.z+(this->_radius * 3), dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = _neighbors.begin();  neighbor != _neighbors.end();  neighbor++)
	{
		if ( (*neighbor)->isAgent() && (*neighbor)->computePenetration(this->position(), this->_radius) > 0.00001f)
		{
			Util::DrawLib::drawStar(
					this->position()
					+
					(
						(
							dynamic_cast<AgentInterface*>(*neighbor)->position()
							-
							this->position()
						)
					/2), Util::Vector(1,0,0), 0.8f, gRed);
			// Util::DrawLib::drawStar(this->position(), Util::Vector(1,0,0), 1.14f, gRed);
		}
	}
#endif
#ifdef DRAW_HISTORIES
	__oldPositions.push_back(position());
	int points = 0;
	float mostPoints = 100.0f;
	while ( __oldPositions.size() > mostPoints )
	{
		__oldPositions.pop_front();
	}
	for (int q = __oldPositions.size()-1 ; q > 0 && __oldPositions.size() > 1; q--)
	{
		DrawLib::drawLineAlpha(__oldPositions.at(q), __oldPositions.at(q-1),gBlack, q/(float)__oldPositions.size());
	}

#endif

#ifdef DRAW_ANNOTATIONS
/*
	for (int i=0; ( _waypoints.size() > 1 ) && (i < (_waypoints.size() - 1)); i++)
	{
		if ( _gEngine->isAgentSelected(this) )
		{
			DrawLib::drawLine(_waypoints.at(i), _waypoints.at(i+1), gYellow);
		}
		else
		{
			//DrawLib::drawLine(_waypoints.at(i), _waypoints.at(i+1), gBlack);
		}
	}

	for (int i=0; i < (_waypoints.size()); i++)
	{
		DrawLib::drawStar(_waypoints.at(i), Util::Vector(1,0,0), 0.34f, gBlue);
	}

	for (int i=0; ( _midTermPath.size() > 1 ) && (i < (_midTermPath.size() - 1)); i++)
	{
		if ( _gEngine->isAgentSelected(this) )
		{
			DrawLib::drawLine(_midTermPath.at(i), _midTermPath.at(i+1), gMagenta);
		}
		else
		{
			// DrawLib::drawLine(_midTermPath.at(i), _midTermPath.at(i+1), gCyan);
		}
	}
*/

	// Draw midtermPath
		

	DrawLib::drawLine(position(), _localTargetLocation, gGray10);
	DrawLib::drawStar(_localTargetLocation+Util::Vector(0,0.001,0), Util::Vector(1,0,0), 0.24f, gGray10);

	/*
	// draw normals and closest points on walls
	std::set<SteerLib::ObstacleInterface * > tmp_obs = gEngine->getObstacles();

	for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = tmp_obs.begin();  tmp_o != tmp_obs.end();  tmp_o++)
	{
		Util::Vector normal = calcWallNormal( *tmp_o );
		std::pair<Util::Point,Util::Point> line = calcWallPointsFromNormal(* tmp_o, normal);
		Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
				(line.first.z+line.second.z)/2);
		DrawLib::drawLine(midpoint, midpoint+normal, gGreen);

		// Draw the closes point as well
		std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
		DrawLib::drawStar(min_stuff.second, Util::Vector(1,0,0), 0.34f, gGreen);
	}
	*/

#endif

#endif
}

void SocialForcesAgent::runLongTermPlanningAStarLite()
{
	AStarLite longTermAStar;

	if (!_enabled) return;

	//==========================================================================
	int myIndexPosition = getSimulationEngine()->getSpatialDatabase()->getCellIndexFromLocation(_position);
	int goalIndex = getSimulationEngine()->getSpatialDatabase()->getCellIndexFromLocation(_currentGoal.targetLocation);

	if (myIndexPosition != -1) {
		// run the main a-star search here
		longTermAStar.findPath((*gEnvironmentForAStar), myIndexPosition, goalIndex);

		// set up the waypoints along this path.
		// if there was no path, then just make one waypoint that is the landmark target.
		_waypoints.clear();
		int longTermAStarLength = (int) longTermAStar.getPath().size();
		if (longTermAStarLength > 2)
		{
			// note the >2 condition: if the astar path is not at least this large, then there will be a behavior bug in the AI
			// when it tries to create waypoints.  in this case, the right thing to do is create only one waypoint that is at the landmark target.
			// remember the astar lib produces "backwards" paths that start at [pathLengh-1] and end at [0].
			int nextWaypointIndex = longTermAStarLength - 1 - FURTHEST_LOCAL_TARGET_DISTANCE;
			while (nextWaypointIndex > 0) {
				Util::Point waypoint;
				getSimulationEngine()->getSpatialDatabase()->getLocationFromIndex(longTermAStar.getPath()[nextWaypointIndex],waypoint);
				_waypoints.push_back(waypoint);
				nextWaypointIndex -= FURTHEST_LOCAL_TARGET_DISTANCE;
			}
			_waypoints.push_back(_currentGoal.targetLocation);
		}
		else {
			_waypoints.push_back(_currentGoal.targetLocation);
		}

		// since we just computed a new long-term path, set the character to steer towards the first waypoint.
		_currentWaypointIndex = 0; 
	}
	else {
		// can't do A-star if we are outside the database.
		// this happens rarely, if ever, but still needs to be robustly handled... this seems like a reasonable decision to make in the extreme case.
		_waypoints.push_back(_currentGoal.targetLocation);
	}
}

void SocialForcesAgent::runMidTermPlanningAStarLite()
{

#ifndef USE_ANNOTATIONS
	// if not using annotations, then declare things local here.
	AStarLite midTermAStar;
#endif

#ifdef IGNORE_PLANNING
	return;
#endif

	if (!_enabled) return;

	// if we reached the final goal, then schedule long-term planning to run
	// long-term planning will call cognitive
	if (_reachedCurrentGoal()) {
		runLongTermPlanningAStarLite();
		if (!_enabled) return;
	}

	// compute a local a-star from your current location to the waypoint.
	int myIndexPosition = getSimulationEngine()->getSpatialDatabase()->getCellIndexFromLocation(_position.x, _position.z);
	int waypointIndexPosition = getSimulationEngine()->getSpatialDatabase()->getCellIndexFromLocation(_waypoints[_currentWaypointIndex].x, _waypoints[_currentWaypointIndex].z);
	if (!midTermAStar.findPath((*gEnvironmentForAStar), myIndexPosition, waypointIndexPosition))
	{
		std::cout << "Error! Midterm path not found " << std::endl;
	}

	// copy the local AStar path to your array
	// keeping the convention that the beginning of the path starts at the end of the array

	/// Store the output from the mid term Astar get path
	_midTermPath = midTermAStar.getPath();
	_midTermPathSize = _midTermPath.size();

#ifdef _DEBUG_1
	std::cout << "In _runMidTermPlanningPhase, Mid term path size: " << _midTermPathSize << "\n";
#endif

	// sanity checks
	if (_midTermPathSize != midTermAStar.getPath().size()) {
		std::cerr << "ERROR!!!  _midTermPathSize does not equal (midTermAStar.getPath()).size(). exiting ungracefully.\n";
		assert(false);
	}
}

void SocialForcesAgent::FindAndSetFurthestVisibleLocalTarget()
{
	Util::Point midtermPathStartPoint;

	for (int i = _currentWaypointIndex; _midTermPathSize - i - 1 > 0; i++) 
	{
		Util::Point tempPathStartPoint;
		getSimulationEngine()->getSpatialDatabase()->getLocationFromIndex(_midTermPath[_midTermPathSize - i - 1],tempPathStartPoint);
		if (this->hasLineOfSightTo(tempPathStartPoint))
		{
			_currentWaypointIndex = i;	
		} else
		{
			break;
		}
	}

	getSimulationEngine()->getSpatialDatabase()->getLocationFromIndex(_midTermPath[_midTermPathSize - _currentWaypointIndex - 1],midtermPathStartPoint);
	_localTargetLocation = midtermPathStartPoint;
}

bool SocialForcesAgent::_reachedCurrentWaypoint()
{
	return ( (_waypoints[_currentWaypointIndex]-position()).lengthSquared() < (_radius * _radius) );
}

bool SocialForcesAgent::_reachedLocalTarget()
{
	return ( (_localTargetLocation-_position).lengthSquared() < (_radius * _radius) );
}

bool SocialForcesAgent::_LocalTargetIsFinalGoal()
{
	return ( (_localTargetLocation-_currentGoal.targetLocation).lengthSquared() < (_radius * _radius) );

}

bool SocialForcesAgent::_reachedCurrentGoal()
{
	SteerLib::AgentGoalInfo goalInfo = _goalQueue.front();
	return ( (_currentGoal.targetLocation-_position).lengthSquared() < (_radius * _radius) ||
			(goalInfo.goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL &&
								Util::boxOverlapsCircle2D(goalInfo.targetRegion.xmin, goalInfo.targetRegion.xmax,
										goalInfo.targetRegion.zmin, goalInfo.targetRegion.zmax, this->position(), this->radius())
										));
}