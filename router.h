#ifndef ROUTER_H
#define ROUTER_H

#include "base_classes.h"
#include "hashmap.h"
#include "geotools.h"
#include <iostream>
#include <queue>
#include <vector>
#include <iostream>
#include <string>


class Router : public RouterBase {
public:
	/// <summary>
	/// Constructor initializes geodatabase member variable to the inputted parameter.
	/// </summary>
	/// <param name="geo_db"></param>
	Router(const GeoDatabaseBase& geo_db);

	/// <summary>
	/// Destructor does nothing.
	/// </summary>
	virtual ~Router();

	/// <summary>
	/// Route function uses A* algorithm to determine the shortest path to the inputted destination pt2.
	/// A minheap data structure is used to place the GeoPoint with the lowest distance rating at the top of the 
	/// queue. Hash table determines if a GeoPoint has been visited (by determining if it is in the hash table).
	/// Hash table also keeps track of path from start to finish to be returned.
	/// </summary>
	/// <param name="pt1"></param>
	/// <param name="pt2"></param>
	/// <returns></returns>
	virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;

private:
	/// <summary>
	/// AGeoPoint structure derives from Geopoint. Adds pointer to parent GeoPoint, and doubles g, h, and f to determine
	/// distance rating & priority.
	/// </summary>
	struct AGeoPoint : public GeoPoint {
		AGeoPoint(const GeoPoint& pt) : GeoPoint(pt), parent(nullptr), g(0), h(0), f(0) {

		}
		bool operator>(const AGeoPoint& rhs) const {
			return f > rhs.f;
		}
		bool operator<(const AGeoPoint& rhs) const {
			return f < rhs.f;
		}

		AGeoPoint* parent;
		double g;
		double h;
		double f;
	};

	/// <summary>
	/// Function determines if two GeoPoints are equal, by examining latitude & longitude strings.
	/// </summary>
	/// <param name="pt1"></param>
	/// <param name="pt2"></param>
	/// <returns></returns>
	bool isSameGeoPoint(const GeoPoint& pt1, const GeoPoint& pt2) const;

	/// <summary>
	/// Function computes heuristic statistic by determining Euclidean distance to destination from
	/// current GeoPoint.
	/// </summary>
	/// <param name="successor"></param>
	/// <param name="destination"></param>
	/// <returns></returns>
	double computeHeuristic(const GeoPoint& successor, const GeoPoint& destination) const;

	/// <summary>
	/// Function concatenates GeoPoint latitude and longitude strings into one string with a single
	/// space separating them.
	/// </summary>
	/// <param name="pt"></param>
	/// <returns></returns>
	std::string GeoPointToString(const GeoPoint& pt) const;

	/// <summary>
	/// Function concatenates AGeoPoint latitude and longitude strings into one string with a single
	/// space separating them.
	/// </summary>
	/// <param name="pt"></param>
	/// <returns></returns>
	std::string AGeoPointToString(const AGeoPoint& pt) const;

	const GeoDatabaseBase& m_geodb;

	/// <summary>
	/// Function takes in a hash table and traces back GeoPoints to the start point. Points are 
	/// returned in a GeoPoint vector in order from the first points to the last.
	/// </summary>
	/// <param name="start"></param>
	/// <param name="dest"></param>
	/// <param name="locationOfPreviousWayPoint"></param>
	/// <returns></returns>
	std::vector<GeoPoint> retraceSteps(const GeoPoint& start, const GeoPoint& dest, const HashMap<GeoPoint>& locationOfPreviousWayPoint) const;
};

#endif // !ROUTER_H