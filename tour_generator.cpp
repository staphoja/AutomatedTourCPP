#include "tour_generator.h"

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router) 
	: m_geodb(geodb), m_router(router) {

}

TourGenerator::~TourGenerator() {

}

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const {
	std::string stopName, talkingPoint;
	GeoPoint stopPoint;
	std::vector<TourCommand> tourCommands;

	for (int i = 0; i < stops.size(); i++) {
		stops.get_poi_data(i, stopName, talkingPoint);
		m_geodb.get_poi_location(stopName, stopPoint);
		TourCommand curr;
		curr.init_commentary(stopName, talkingPoint);
		tourCommands.push_back(curr);

		if (i < stops.size() - 1) {
			std::string nextStopName, nextTalkingPoint;
			GeoPoint nextStopPoint;
			stops.get_poi_data(i + 1, nextStopName, nextTalkingPoint);
			m_geodb.get_poi_location(nextStopName, nextStopPoint);
			std::vector<GeoPoint> wayToNextStop;
			wayToNextStop = m_router.route(stopPoint, nextStopPoint);


			for (int i = 1; i < wayToNextStop.size(); i++) {
				curr.init_proceed(getTurnCategory(angle_of_line(wayToNextStop[i - 1], wayToNextStop[i])),
					m_geodb.get_street_name(wayToNextStop[i - 1], wayToNextStop[i]),
					distance_earth_miles(wayToNextStop[i - 1], wayToNextStop[i]), 
					wayToNextStop[i - 1], wayToNextStop[i]);

				tourCommands.push_back(curr);
			
				if (i < stops.size() - 1) {
					std::string thirdStopName, thirdTalkingPoint;
					GeoPoint thirdStopPoint;
					std::string firstSegmentName = m_geodb.get_street_name(wayToNextStop[i - 1], wayToNextStop[i]);
					std::string secondSegmentName = m_geodb.get_street_name(wayToNextStop[i], wayToNextStop[i + 1]);
					double turnAngle = angle_of_turn(wayToNextStop[i - 1], wayToNextStop[i], wayToNextStop[i + 1]);
					std::string turnCategory;
					if (firstSegmentName != secondSegmentName && turnAngle != 0) {
						turnCategory = getLeftOrRight(turnAngle);
						curr.init_turn(turnCategory, secondSegmentName);
						tourCommands.push_back(curr);
					}
				}
			}
		}
	}
	return tourCommands;
}


std::string TourGenerator::getTurnCategory(double turnAngle) const {
	if (0 < turnAngle && turnAngle < 22.5)
		return "east";
	else if (0 < turnAngle && turnAngle < 67.5)
		return "northeast";
	else if (0 <= turnAngle && turnAngle < 112.5)
		return "north";
	else if (0 <= turnAngle && turnAngle < 157.5)
		return "northwest";
	else if (0 <= turnAngle && turnAngle < 202.5)
		return "west";
	else if (0 <= turnAngle && turnAngle < 247.5)
		return "southwest";
	else if (0 <= turnAngle && turnAngle < 292.5)
		return "south";
	else if (0 <= turnAngle && turnAngle < 337.5)
		return "southeast";
	else
		return "east";
}

std::string TourGenerator::getLeftOrRight(double turnAngle) const {
	if (0 < turnAngle && turnAngle < 180)
		return "left";
	else
		return "right";
}