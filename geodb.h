#ifndef GEODB_H
#define GEODB_H

#include "base_classes.h"
#include "geotools.h"
#include "hashmap.h"
#include "geopoint.h"
#include "geotools.h"
#include <cctype>
#include <list>
#include <cassert>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>


class GeoDatabase : public GeoDatabaseBase {
	public:
		/// <summary>
		/// Constructor initializes counts to 0 and sets the initial capacity of the graph structure
		/// to 10 points.
		/// </summary>
		GeoDatabase();

		/// <summary>
		/// Destructor clears dynamically allocated memory in graph data structure.
		/// </summary>
		virtual ~GeoDatabase();

		/// <summary>
		/// Load function reads mapdata.txt file. Points of interest are stored in an open hash table
		/// m_geoPOIs with a counter variable m_nPOIs. Hash table m_pois keeps track of ordering of POIs
		/// to access the m_stops member. Names of road segments are stored in a hash table m_segmentHash
		/// accessable with a string representing endpoints of a segment. If read-in line 
		/// is the name & address of a point of interest, function creates a midpoint, 
		/// and connects each point of interest to the midpoint.
		/// </summary>
		/// <param name="map_data_file"></param>
		/// <returns></returns>
		virtual bool load(const std::string& map_data_file);

		/// <summary>
		/// If a point of interest exists, this function sets the point parameter equal to the point
		/// the location is at and returns true. Otherwise, the function returns false.
		/// </summary>
		/// <param name="poi"></param>
		/// <param name="point"></param>
		/// <returns></returns>
		virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;

		/// <summary>
		/// Function returns a vector of GeoPoints that are connected to the GeoPoint
		/// parameter. If none exist, an empty vector is returned.
		/// </summary>
		/// <param name="pt"></param>
		/// <returns></returns>
		virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;

		/// <summary>
		/// Function takes in two GeoPoints and returns a street name that corresponds. 
		/// Entries are filed based on a string representing their endpoints. Parameters can 
		/// be in either order to retrieve street name. Empty string is returned if no 
		/// street can be found.
		/// </summary>
		/// <param name="pt1"></param>
		/// <param name="pt2"></param>
		/// <returns></returns>
		virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;

	private:
		/// <summary>
		/// Function dynamically increases memory in the graph data structure if number of streets
		/// is greater than the capacity.
		/// </summary>
		/// <param name="newSize"></param>
		/// <returns></returns>
		bool increaseGraphSize(int newSize);

		/// <summary>
		/// Returns true if a list has a vertex determined by the vertex parameter.
		/// </summary>
		/// <param name="vertices"></param>
		/// <param name="vertex"></param>
		/// <returns></returns>
		bool graphSlotContains(std::list<std::string> vertices, std::string vertex) const;

		HashMap<int> m_vertexSlot;
		int m_nVertices;
		int m_vertexCapacity;
		std::list<std::string>* m_graph;

		HashMap<std::string> m_segmentHash;
		HashMap<GeoPoint> m_geoPOIs;
		HashMap<int> m_pois;
		unsigned int m_nPOIs;
		Stops m_stops;
};

#endif // !GEODB_H