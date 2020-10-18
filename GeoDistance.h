#pragma once

#include <QObject>
#include <QPointF>
#include <math.h>


const double ERROR_DISTANCE = -1.0;

//360 degree -> 2Пr = 2*3.14*6 378 137 = 40 075 016.69 meters
const double GEO_ERROR_COORDINATE = 1000000001.0;

//min degree longitude value: -180(-360) degree
//min degree latitude value: -90 degree
//mark degree value -> shift value
const double GEO_DEG_MARK_SHIFT = -2000.0;
//if degree value less `GEO_DEG_MARK_THRESHOLD` then
// this degree value is shifted
//reverse (return original): value -= GEO_DEG_MARK_SHIFT
const double GEO_DEG_MARK_THRESHOLD = -1000.0;


class GeoDistance : public QObject
{
	Q_OBJECT

public:
	GeoDistance(QObject* parent = 0);
	~GeoDistance();

	/**
	 * @brief getDistanceOnSphere - calculation of the distance between two points.
	 *        on the sphere (example Google R = 6 378 137 meters, EPSG:900913).
	 * @param lon1 - longitude in degrees of the first point.
	 * @param lat1 - latitude in degrees of the first point.
	 * @param lon2 - longitude in degrees of the second point.
	 * @param lat2 - latitude in degrees of the second point.
	 * @param earthRadius - радиус земли-сферы, если радиус равен -1.0,
	 *        то считаю радиус сферы равным радиусу эллипсоида на широте lat1
	 * @return distance between points in meters.
	 */
	static double getDistanceOnSphere(double lon1, double lat1,
									  double lon2, double lat2,
									  const double earthRadius = -1.0);

	static QPointF getPointByDistance(double lon1, double lat1, double dist, double azimuth);

	/**
	 * @brief getPointByDistanceOnSphere
	 * @param lon1 - географические координаты точки из которой наблюдаю (в градусах)
	 * @param lat1
	 * @param dist - дистанция в метрах
	 * @param degAzimuth - азимут в градусах относительно севера
	 * @param earthRadius - радиус земли-сферы, если радиус равен -1.0,
	 *        то считаю радиус сферы равным радиусу эллипсоида
	 *        на широте lat1, то есть на широте точки из которой наблюдаю
	 *
	 *
	 *        ----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------
	 *        если необходимо выполнить расчеты с постоянным радиусом сферы,
	 *        то необходимо его предварительно расcчитать
	 *
	 *        double getEarthRadius(const double geodeticLat)
	 *
	 *        и рассчитанное значение радиуса передавать как параметр
	 *        ----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------
	 *
	 * @return
	 */
	static QPointF getPointByDistanceOnSphere(double lon1, double lat1,
											  double dist, double degAzimuth,
											  const double earthRadius = -1.0);

	/**
	 * @brief getDistanceOnPlane - calculation of the distance between two points on the plane.
	 * @param x_lon1 - longitude in meters of the first point.
	 * @param y_lat1 - latitude in meters of the first point.
	 * @param x_lon2 - longitude in meters of the second point.
	 * @param y_lat2 - latitude in meters of the second point.
	 * @return distance between points in meters.
	 */
	static double getDistanceOnPlane(const double x_lon1, const double y_lat1,
									 const double x_lon2, const double y_lat2);

	static QPointF getPointByDistancePlane(const double x_lon1, const double y_lat1,
										   const double dist, const double azimuth);

	static double getEarthRadius(const double geodeticLat);

	/**
	 * @brief getAzimuthOnSphere
	 *                 lat
	 *                /|\
	 *                 0             lon2,lat2
	 *                 |              /|\
	 *                 |               |
	 *          270----|----90->lon    lon1,lat1
	 *                 |
	 *                 |
	 *                180
	 * @param lon1 - географические координаты точки из которой наблюдаю (в градусах)
	 * @param lat1
	 * @param lon2 - географические координаты точки на которую смотрю (в градусах)
	 * @param lat2
	 * @param earthRadius - радиус земли-сферы, если радиус равен -1.0,
	 *        то считаю радиус сферы равным радиусу эллипсоида
	 *        на широте lat1, то есть на широте точки из которой наблюдаю
	 *
	 *
	 *        ----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------
	 *        если необходимо выполнить расчеты с постоянным радиусом сферы,
	 *        то необходимо его предварительно расcчитать
	 *
	 *        double getEarthRadius(const double geodeticLat)
	 *
	 *        и рассчитанное значение радиуса передавать как параметр
	 *        ----------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------
	 *
	 *
	 * @return азимут по часовой стрелке в радианах относительно севера
	 *         (градусы=alpha*180.0/M_PI)
	 */
	static double getAzimuthOnSphere(const double lon1, const double lat1,
									 const double lon2, const double lat2,
									 const double earthRadius = -1.0);

	/**
	 * @brief getAzimuth
	 *                 y
	 *                /|\
	 *                 0             x2,y2
	 *                 |              /|\
	 *                 |               |
	 *          270----|----90->x    x1,y1
	 *                 |
	 *                 |
	 *                180
	 * @param x1 - декартовы координаты точки из которой наблюдаю (в метрах)
	 * @param y1
	 * @param x2 - декартовы координаты точки на которую смотрю (в метрах)
	 * @param y2
	 * @return азимут по часовой стрелке в радианах относительно севера
	 *         (градусы=alpha*180.0/M_PI)
	 */
	static double getAzimuth(const double x1, const double y1, const double x2, const double y2);

	//учитываю переход через 0: 2Pi->0градусов
	static double getDeltaAngle(const double rad_1, const double rad_2);
	static double getDeltaDegAngle(const double deg_1, const double deg_2);

	//кратчайший угол поворота (по модулю не превышает 180 градусов)
	//из deg1 в deg2 (результат может быть отрицательным -> поворот против часовой стрелки)
	//deg1 + result = deg2
	static double getShortRotDeg1ToDeg2(const double deg1, const double deg2);

	//учитываю переход 2Pi и отрицательные значения
	static double getCorrectRadAngle(const double rad_1);
	static double getCorrectDegAngle(const double deg_1);

	static double DegreeToRadian(double angle);
	static double RadianToDegree(double angle);

	static QPointF getLineCross(const double x1, const double y1,
								const double x2, const double y2,
								const double x3, const double y3,
								const double x4, const double y4);

	static QPointF getSegmentCross(const double x1, const double y1,
								   const double x2, const double y2,
								   const double x3, const double y3,
								   const double x4, const double y4);

	static bool isCorrectDouble(const double val);
	static bool isCorrectInt(const int val);

	static bool isCorrectGeoCoord(const double lon, const double lat);

	static bool isSegmentCross(const double x1, const double y1,
							   const double x2, const double y2,
							   const double x3, const double y3,
							   const double x4, const double y4);

};
