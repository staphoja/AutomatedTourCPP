#ifndef TOUR_GENERATOR_H
#define TOUR_GENERATOR_H

#include "base_classes.h"
#include "geodb.h"
#include "router.h"


class TourGenerator : public TourGeneratorBase {
public:
	/// <summary>
	/// Initializes member database and router variables.
	/// </summary>
	/// <param name="geodb"></param>
	/// <param name="router"></param>
	TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router);

	/// <summary>
	/// Does nothing.
	/// </summary>
	virtual ~TourGenerator();

	/// <summary>
	/// Function determines GeoPoints between stops of a tour. At each stop, commentary is printed
	/// based on the location. As long as there are more stops in the tour, function
	/// tracks distances traveled on roads and turns onto different streets in another direction.
	/// </summary>
	/// <param name="stops"></param>
	/// <returns></returns>
	virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;

private:
	/// <summary>
	/// Categorizes a turn angle into north, south, northeast, southwest, etc. 
	/// </summary>
	/// <param name="turnAngle"></param>
	/// <returns></returns>
	std::string getTurnCategory(double turnAngle) const;

	/// <summary>
	/// Categorizes a turn angle into left and right.
	/// </summary>
	/// <param name="turnAngle"></param>
	/// <returns></returns>
	std::string getLeftOrRight(double turnAngle) const;

	const GeoDatabaseBase& m_geodb;
	const RouterBase& m_router;
};

#endif