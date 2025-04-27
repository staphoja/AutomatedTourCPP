#include "geodb.h"


GeoDatabase::GeoDatabase() : m_nPOIs(0), m_nVertices(0), m_vertexCapacity(10) {
	m_stops.load("mapdata.txt");

	m_graph = new std::list<std::string>[m_vertexCapacity];
}

GeoDatabase::~GeoDatabase() {
	delete[] m_graph;
}

bool GeoDatabase::load(const std::string& map_data_file) {
	std::ifstream infile(map_data_file);
	if (!infile)
		return false;
	std::string geopoint_info;
	std::string last_street_name;
	GeoPoint last_geopoint_start, last_geopoint_end;
	std::string last_segment_string_start, last_segment_string_end;

	while (std::getline(infile, geopoint_info))
	{
		if (geopoint_info.empty())
			break;

		if (geopoint_info.size() <= 2 && isdigit(geopoint_info.front()) && isdigit(geopoint_info.back())) {
			if (stoi(geopoint_info) > 0) {
				int pointsOfInterest = std::stoi(geopoint_info);
				GeoPoint poiMidpoint = midpoint(last_geopoint_start, last_geopoint_end);
				std::string poiMidpointString = poiMidpoint.sLatitude + " " + poiMidpoint.sLongitude;

				m_vertexSlot.insert(poiMidpoint.sLatitude + " " + poiMidpoint.sLongitude, m_nVertices);
				m_nVertices++;

				increaseGraphSize(m_nVertices + pointsOfInterest + 1);
				m_graph[*m_vertexSlot.find(poiMidpointString)].push_back(last_segment_string_start);
				m_graph[*m_vertexSlot.find(poiMidpointString)].push_back(last_segment_string_end);
				m_graph[*m_vertexSlot.find(last_segment_string_start)].push_back(poiMidpointString);
				m_graph[*m_vertexSlot.find(last_segment_string_end)].push_back(poiMidpointString);

				m_segmentHash.insert(last_geopoint_start.sLatitude + " " + last_geopoint_start.sLongitude + " " + poiMidpoint.sLatitude + " " + poiMidpoint.sLongitude, last_street_name);
				m_segmentHash.insert(poiMidpoint.sLatitude + " " + poiMidpoint.sLongitude + " " + last_geopoint_end.sLatitude + " " + last_geopoint_end.sLongitude, last_street_name);

				for (int i = 0; i < pointsOfInterest; i++) {
					std::getline(infile, geopoint_info);
					std::vector<size_t> delim_pos;
					delim_pos.push_back(geopoint_info.find('|'));
					delim_pos.push_back(geopoint_info.rfind(' '));

					m_vertexSlot.insert(geopoint_info.substr(delim_pos.front() + 1), m_nVertices);
					m_nVertices++;
					m_graph[*m_vertexSlot.find(poiMidpointString)].push_back(geopoint_info.substr(delim_pos.front() + 1));
					m_graph[*m_vertexSlot.find(geopoint_info.substr(delim_pos.front() + 1))].push_back(poiMidpointString);

					m_segmentHash.insert(poiMidpoint.sLatitude + " " + poiMidpoint.sLongitude + " " + geopoint_info.substr(delim_pos.front() + 1), "a path");

					m_pois.insert(geopoint_info.substr(0, delim_pos.front()), m_nPOIs);
					m_geoPOIs.insert(geopoint_info.substr(0, delim_pos.front()), GeoPoint(geopoint_info.substr(delim_pos.front() + 1, delim_pos[1] - delim_pos[0] - 1), geopoint_info.substr(delim_pos[1] + 1)));
					m_nPOIs++;
				}
			}
		}
		else if (geopoint_info.find('.') == geopoint_info.rfind('.') && geopoint_info.find('|') == std::string::npos && geopoint_info.size() > 2) {
			last_street_name = geopoint_info;
		}
		else {
			std::vector<size_t> delim_pos;

			do {
				if (delim_pos.empty())
					delim_pos.push_back(geopoint_info.find(' '));
				else
					delim_pos.push_back(geopoint_info.find(' ', delim_pos.back() + 1));
			} while (delim_pos.back() != std::string::npos && geopoint_info.find(' ', delim_pos.back() + 1) != std::string::npos);

			if (delim_pos.front() == std::string::npos || delim_pos.size() != 3 ||
				geopoint_info.find(' ', delim_pos.back() + 1) != std::string::npos ||
				last_street_name == "")
				return false;

			if (m_vertexSlot.find(geopoint_info.substr(0, delim_pos[1])) == nullptr) {
				m_vertexSlot.insert(geopoint_info.substr(0, delim_pos[1]), m_nVertices);
				m_nVertices++;
			}
			if (m_vertexSlot.find(geopoint_info.substr(delim_pos[1] + 1)) == nullptr) {
				m_vertexSlot.insert(geopoint_info.substr(delim_pos[1] + 1), m_nVertices);
				m_nVertices++;
			}

			last_segment_string_start = geopoint_info.substr(0, delim_pos[1]);
			last_segment_string_end = geopoint_info.substr(delim_pos[1] + 1);

			increaseGraphSize(m_nVertices + 2);
			if (!graphSlotContains(m_graph[*m_vertexSlot.find(last_segment_string_start)], last_segment_string_end)) {
				m_graph[*m_vertexSlot.find(last_segment_string_start)].push_back(last_segment_string_end);
			}
			if (!graphSlotContains(m_graph[*m_vertexSlot.find(last_segment_string_end)], last_segment_string_start)) {
				m_graph[*m_vertexSlot.find(last_segment_string_end)].push_back(last_segment_string_start);
			}

			m_segmentHash.insert(geopoint_info, last_street_name);

			last_geopoint_start = GeoPoint(geopoint_info.substr(0, delim_pos[0]), geopoint_info.substr(delim_pos[0] + 1, delim_pos[1] - delim_pos[0] - 1));
			last_geopoint_end = GeoPoint(geopoint_info.substr(delim_pos[1] + 1, delim_pos[2] - delim_pos[1] - 1), geopoint_info.substr(delim_pos[2] + 1));
		}
	}
	return true;
}

bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const {
	std::string pointOfInterest = poi;
	if (m_pois.find(poi) != nullptr) {
		point = *m_geoPOIs.find(poi);
		return true;
	}
	return false;
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const {
	std::vector<GeoPoint> neighbors = {};
	std::string ptString = pt.sLatitude + " " + pt.sLongitude;
	int vertexSlot = *m_vertexSlot.find(ptString);
	for (std::list<std::string>::iterator iter = m_graph[vertexSlot].begin(); iter != m_graph[vertexSlot].end(); iter++) {
		neighbors.push_back(GeoPoint((*iter).substr(0, (*iter).find(" ")), (*iter).substr((*iter).find(" ") + 1)));
	}
	return neighbors;
}

std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const {
	const std::string* forwardptr = m_segmentHash.find(pt1.sLatitude + " " + pt1.sLongitude + " " + pt2.sLatitude + " " + pt2.sLongitude);
	const std::string* backwardptr = m_segmentHash.find(pt2.sLatitude + " " + pt2.sLongitude + " " + pt1.sLatitude + " " + pt1.sLongitude);

	if (forwardptr != nullptr) {
		return *forwardptr;
	}
	else if (backwardptr != nullptr) {
		return *backwardptr;
	}
	else {
		return "";
	}
}


bool GeoDatabase::increaseGraphSize(int newSize) {
	if (newSize >= m_vertexCapacity) {
		m_vertexCapacity *= 2;
		std::list<std::string>* tempGraph = new std::list<std::string>[m_vertexCapacity];

		for (int i = 0; i < m_vertexCapacity / 2; i++) {
			for (std::list<std::string>::iterator iter = m_graph[i].begin(); iter != m_graph[i].end(); iter++) {
				tempGraph[i].push_back(*iter);
			}
		}

		delete[] m_graph;
		m_graph = new std::list<std::string>[m_vertexCapacity];

		for (int i = 0; i < m_vertexCapacity; i++) {
			for (std::list<std::string>::iterator iter = tempGraph[i].begin(); iter != tempGraph[i].end(); iter++) {
				m_graph[i].push_back(*iter);
			}
		}

		delete[] tempGraph;
		return true;
	}
	return false;
}

bool GeoDatabase::graphSlotContains(std::list<std::string> vertices, std::string vertex) const {
	for (std::list<std::string>::iterator iter = vertices.begin(); iter != vertices.end(); iter++) {
		if (*iter == vertex)
			return true;
	}
	return false;
}