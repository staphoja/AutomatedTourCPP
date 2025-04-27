#include "router.h"


Router::Router(const GeoDatabaseBase& geo_db) : m_geodb(geo_db) {

}

Router::~Router() {

}

std::vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const {
	if (isSameGeoPoint(pt1, pt2))
		return std::vector<GeoPoint>{pt2};

	HashMap<GeoPoint> locationOfPreviousWayPoint;
	std::priority_queue<AGeoPoint, std::vector<AGeoPoint>, std::greater<AGeoPoint>> minheap;

	AGeoPoint apt1(pt1);
	locationOfPreviousWayPoint.insert(AGeoPointToString(apt1), apt1);
	minheap.push(apt1);

	while (!minheap.empty()) {
		AGeoPoint curr = minheap.top();
		minheap.pop();
		if (isSameGeoPoint(curr, pt2))
			return retraceSteps(pt1, pt2, locationOfPreviousWayPoint);
		std::vector<GeoPoint> successors = m_geodb.get_connected_points(curr);
		std::vector<AGeoPoint> asuccessors;
		for (int i = 0; i < successors.size(); i++) {
			asuccessors.push_back(AGeoPoint(successors[i]));
			asuccessors[i].g = curr.g + distance_earth_km(curr, asuccessors[i]);
			asuccessors[i].h = distance_earth_km(asuccessors[i], pt2);
			asuccessors[i].f = asuccessors[i].g + asuccessors[i].h;
			asuccessors[i].parent = &curr;
			if (locationOfPreviousWayPoint.find(AGeoPointToString(asuccessors[i])) != nullptr)
				continue;
			locationOfPreviousWayPoint.insert(AGeoPointToString(asuccessors[i]), curr);
			minheap.push(asuccessors[i]);
		}
	}
	return std::vector<GeoPoint>{};
}


bool Router::isSameGeoPoint(const GeoPoint& pt1, const GeoPoint& pt2) const {
	if (pt1.sLatitude == pt2.sLatitude && pt1.sLongitude == pt2.sLongitude)
		return true;
	return false;
}

double Router::computeHeuristic(const GeoPoint& successor, const GeoPoint& destination) const {
	return distance_earth_km(successor, destination);
}

std::string Router::GeoPointToString(const GeoPoint& pt) const {
	return pt.sLatitude + " " + pt.sLongitude;
}

std::string Router::AGeoPointToString(const AGeoPoint& pt) const {
	return pt.sLatitude + " " + pt.sLongitude;
}

std::vector<GeoPoint> Router::retraceSteps(const GeoPoint& start, const GeoPoint& dest, const HashMap<GeoPoint>& locationOfPreviousWayPoint) const {
	GeoPoint curr = dest;
	std::vector<GeoPoint> steps;
	while (!isSameGeoPoint(curr, start)) {
		steps.insert(steps.begin(), curr);
		curr = *locationOfPreviousWayPoint.find(GeoPointToString(curr));
	}
	steps.insert(steps.begin(), curr);
	return steps;
}