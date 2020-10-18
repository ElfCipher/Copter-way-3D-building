#include "GeoDistance.h"

#include <iostream>


GeoDistance::GeoDistance(QObject* parent)
	: QObject(parent)
{
}

GeoDistance::~GeoDistance()
{
}

double GeoDistance::getDistanceOnSphere(double lon1, double lat1,
										double lon2, double lat2,
										const double earthRadius)
{
	double retVal = ERROR_DISTANCE;

	if ((lon1 < -180.0) || (lon1 > +180.0) ||
			(lon2 < -180.0) || (lon2 > +180.0)) {
		return retVal;
	}

	if ((lat1 < -90.0) || (lat1 > +90.0) ||
			(lat2 < -90.0) || (lat2 > +90.0)) {
		return retVal;
	}

	double lonRad1 = lon1*M_PI/180.0;
	double latRad1 = lat1*M_PI/180.0;
	double lonRad2 = lon2*M_PI/180.0;
	double latRad2 = lat2*M_PI/180.0;

	double cosLat1 = cos(latRad1);
	double cosLat2 = cos(latRad2);
	double sinLat1 = sin(latRad1);
	double sinLat2 = sin(latRad2);
	double deltaLon = lonRad2 - lonRad1;
	double cosDelta = cos(deltaLon);
	double sinDelta = sin(deltaLon);

	double p1 = pow((cosLat2*sinDelta), 2.0);
	double p2 = pow(((cosLat1*sinLat2) - (sinLat1*cosLat2*cosDelta)), 2.0);
	double p3 = sqrt(p1 + p2);
	double p4 = sinLat1*sinLat2;
	double p5 = cosLat1*cosLat2*cosDelta;
	double p6 = p4 + p5;
	if (p6 == 0.0) {
		return retVal;
	}
	double p7 = p3/p6;
	double anglerad = atan(p7);


	//double sphereRadius = 6378137.0;
	double r = earthRadius;
	if (earthRadius < 0.0) r = getEarthRadius(lat1);

	retVal = anglerad*r;
	return retVal;
}

QPointF GeoDistance::getPointByDistance(double lon1, double lat1, double dist, double azimuth)
{
	lon1 = DegreeToRadian(lon1);
	lat1 = DegreeToRadian(lat1);
	azimuth = DegreeToRadian( getCorrectDegAngle(azimuth) );
	double angle = DegreeToRadian(90);

	double b = dist / getEarthRadius((lat1));
	double a = acos( cos(b)*cos(angle-lat1) + sin(angle-lat1)*sin(b)*cos(azimuth) );
	double B = asin( sin(b)*sin(azimuth)/sin(a) );

	return QPointF( RadianToDegree(B+lon1), RadianToDegree(angle-a) );
}

QPointF GeoDistance::getPointByDistanceOnSphere(double lon1, double lat1,
												double dist, double degAzimuth,
												const double earthRadius)
{
	double azimuth = DegreeToRadian( getCorrectDegAngle(degAzimuth) );

	//-----перехожу в локальную систему координат с центром в lon1,lat1
	double originLon = lon1;
	double originLat = lat1;
	double r = earthRadius;
	if (earthRadius < 0.0) r = getEarthRadius(originLat);

	//----------------
	double one_degree_lat = r*M_PI/180.0;
	double one_degree_lon = one_degree_lat*cos(originLat*M_PI/180.0);


	//----------------
	double lon2 = dist*sin(azimuth)/one_degree_lon + originLon;
	double lat2 = dist*cos(azimuth)/one_degree_lat + originLat;

	return QPointF(lon2, lat2);
}

double GeoDistance::getDistanceOnPlane(const double x_lon1, const double y_lat1,
									   const double x_lon2, const double y_lat2)
{
	double retVal = (x_lon2 - x_lon1)*(x_lon2 - x_lon1) +
			(y_lat2 - y_lat1)*(y_lat2 - y_lat1);
	return sqrt(retVal);
}

QPointF GeoDistance::getPointByDistancePlane(const double x_lon1, const double y_lat1,
											 const double dist, const double azimuth)
{
	QPointF retVal(x_lon1, y_lat1);

	if (dist <= 0.0) {
		return retVal;
	}
	if (azimuth < 0.0) {
		return retVal;
	}

	double azimuth1 = azimuth;

	if (azimuth1 > 360.0) azimuth1 -= 360.0;

	if (azimuth1 > 360.0) return retVal;

	double x2 = x_lon1;
	double y2 = y_lat1;
	double alpha;

	//log_debug(QString("x_lon1=%1, y_lat1=%2")
	//    .arg(QString::number(x_lon1, 'f', 1))
	//    .arg(QString::number(y_lat1, 'f', 1)));
	//log_debug(QString("dist=%1, azimuth1=%2").arg(dist).arg(azimuth1));

	if ((azimuth1 == 0.0) || (azimuth1 == 360.0)) {
		x2 = x_lon1;
		y2 = y_lat1 + dist;
	}
	else if (azimuth1 < 90.0) {
		alpha = DegreeToRadian(azimuth1);
		x2 = x_lon1 + dist*sin(alpha);
		y2 = y_lat1 + dist*cos(alpha);
	}
	else if (azimuth1 == 90.0) {
		x2 = x_lon1 + dist;
		y2 = y_lat1;
	}
	else if (azimuth1 < 180.0) {
		//log_debug("azimuth1 < 180.0");

		alpha = DegreeToRadian(azimuth1-90);
		x2 = x_lon1 + dist*cos(alpha);
		y2 = y_lat1 - dist*sin(alpha);
	}
	else if (azimuth1 == 180.0) {
		//log_debug("azimuth1 == 180.0");

		x2 = x_lon1;
		y2 = y_lat1 - dist;
	}
	else if (azimuth1 < 270.0) {
		//log_debug("azimuth1 < 270");

		alpha = DegreeToRadian(azimuth1-180);
		x2 = x_lon1 - dist*sin(alpha);
		y2 = y_lat1 - dist*cos(alpha);
	}
	else if (azimuth1 == 270.0) {
		//log_debug("azimuth1 == 270.0");

		x2 = x_lon1 - dist;
		y2 = y_lat1;
	}
	else if (azimuth1 < 360.0) {
		alpha = DegreeToRadian(azimuth1-270);
		x2 = x_lon1 - dist*cos(alpha);
		y2 = y_lat1 + dist*sin(alpha);
	}

	retVal.setX(x2);
	retVal.setY(y2);

	//log_debug(QString("retVal.x()=%1, retVal.y()=%2")
	//    .arg(QString::number(retVal.x(), 'f', 1))
	//    .arg(QString::number(retVal.y(), 'f', 1)));

	return retVal;
}

double GeoDistance::getEarthRadius(const double geodeticLat)
{
	//wgs84
	double a = 6378137.0; //meter
	double f = 1.0/298.257223563;
	double b = a*(1.0-f);

	double geocentricLat;
	if (geodeticLat == 90.0) {
		geocentricLat = M_PI/2.0;
	}
	else if (geodeticLat == -90.0) {
		geocentricLat = -1.0*M_PI/2.0;
	}
	else {
		geocentricLat = atan((b*b*tan(geodeticLat*M_PI/180.0))/(a*a));
	}

	//radius from the center of the earth to a point with the specified latitude
	double cosLat = cos(geocentricLat);
	double sinLat = sin(geocentricLat);
	double r = a*b/(sqrt(b*b*cosLat*cosLat + a*a*sinLat*sinLat));

	return r;
}

double GeoDistance::getAzimuthOnSphere(const double lon1, const double lat1,
									   const double lon2, const double lat2,
									   const double earthRadius)
{
	//-----перехожу в локальную систему координат с центром в lon1,lat1
	double originLon = lon1;
	double originLat = lat1;
	double r = earthRadius;
	if (earthRadius < 0.0) r = getEarthRadius(originLat);

	//----------------
	double one_degree_lat = r*M_PI/180.0;
	double one_degree_lon = one_degree_lat*cos(originLat*M_PI/180.0);

	//----------------
	double x1 = 0.0;
	double y1 = 0.0;

	//----------------
	double x2 = (lon2 - originLon)*one_degree_lon;
	double y2 = (lat2 - originLat)*one_degree_lat;

	//----------------
	return getAzimuth(x1, y1, x2, y2);
}

double GeoDistance::getAzimuth(const double x1, const double y1,
							   const double x2, const double y2)
{
	double x, y, tan_alpha, alpha;

	if ((x1 == x2) && (y1 == y2)) {
		return 0.0;
	}

	if ((x1 == x2) && (y1 < y2)) {
		return 0.0;
	}

	if ((x1 < x2) && (y1 < y2)) {
		y = y2 - y1;
		x = x2 - x1;
		tan_alpha = x/y;
		alpha = atan(tan_alpha);
		return alpha;
	}

	if ((x1 < x2) && (y1 == y2)) {
		return (M_PI/2.0);
	}

	if ((x1 < x2) && (y1 > y2)) {
		y = y1 - y2;
		x = x2 - x1;
		tan_alpha = y/x;
		alpha = atan(tan_alpha) + M_PI/2.0;
		return alpha;
	}

	if ((x1 == x2) && (y1 > y2)) {
		return M_PI;
	}

	if ((x1 > x2) && (y1 > y2)) {
		y = y1 - y2;
		x = x1 - x2;
		tan_alpha = x/y;
		alpha = atan(tan_alpha) + M_PI;
		return alpha;
	}

	if ((x1 > x2) && (y1 == y2)) {
		return (1.5*M_PI);
	}

	if ((x1 > x2) && (y1 < y2)) {
		y = y2 - y1;
		x = x1 - x2;
		tan_alpha = y/x;
		alpha = atan(tan_alpha) + 1.5*M_PI;
		return alpha;
	}

	return 0.0;
}

//учитываю переход через 0: 2Pi->0градусов
double GeoDistance::getDeltaAngle(const double rad_1, const double rad_2)
{
	double dA = fabs(rad_2 - rad_1);
	if (dA > M_PI) dA = 2.0*M_PI - dA;
	return dA;
}

double GeoDistance::getDeltaDegAngle(const double deg_1, const double deg_2)
{
	double dA = fabs(deg_2 - deg_1);
	if (dA > 180.0) dA = 360.0 - dA;
	return dA;
}

double GeoDistance::getShortRotDeg1ToDeg2(const double deg_1, const double deg_2)
{
	double dA = deg_2 - deg_1;

	if (dA < -180.0) {
		dA += 360.0;
		return dA;
	}

	if (dA > 180.0) {
		dA -= 360.0;
		return dA;
	}

	return dA;
}

//учитываю переход 2Pi и отрицательные значения
double GeoDistance::getCorrectRadAngle(const double rad_1)
{
	double retVal = rad_1;
	if (rad_1 < 0.0) {
		retVal = rad_1 + 2.0*M_PI;
	}
	else if (rad_1 > 2.0*M_PI) {
		retVal = rad_1 - 2.0*M_PI;
	}
	return retVal;
}

double GeoDistance::getCorrectDegAngle(const double deg_1)
{
	double retVal = deg_1;
	if (deg_1 < 0.0) {
		retVal = deg_1 + 360.0;
	}
	else if (deg_1 > 360.0) {
		retVal = deg_1 - 360.0;
	}
	return retVal;
}

double GeoDistance::DegreeToRadian(double angle)
{
	return M_PI * angle / 180.0;
}

double GeoDistance::RadianToDegree(double angle)
{
	return 180.0 * angle / M_PI;
}

QPointF GeoDistance::getLineCross(const double x1, const double y1,
								  const double x2, const double y2,
								  const double x3, const double y3,
								  const double x4, const double y4)
{
	QPointF retVal(GEO_ERROR_COORDINATE, GEO_ERROR_COORDINATE);

	double r1 = (x4-x3)*(y1-y3)-(y4-y3)*(x1-x3);
	double r2 = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);

	if (r2 == 0) {
		return retVal;
	}
	double k = r1/r2;

	//Подстановка любого из этих значений в
	//соответствующее уравнение прямой даст
	//точку пересечения:
	double x = x1 + k*(x2 - x1);
	double y = y1 + k*(y2 - y1);

	retVal.setX(x);
	retVal.setY(y);

	return retVal;
}

QPointF GeoDistance::getSegmentCross(const double x1, const double y1,
									 const double x2, const double y2,
									 const double x3, const double y3,
									 const double x4, const double y4)
{
	QPointF retVal = getLineCross(x1, y1,
								  x2, y2,
								  x3, y3,
								  x4, y4);

	//определяю принадлежит ли точка пересечения отрезкам
	double xMin, yMin, xMax, yMax;

	//первый отрезок
	if (x1 < x2) {
		xMin = x1;
		xMax = x2;
	}
	else {
		xMin = x2;
		xMax = x1;
	}
	if (y1 < y2) {
		yMin = y1;
		yMax = y2;
	}
	else {
		yMin = y2;
		yMax = y1;
	}
	if ((retVal.x() < xMin) || (retVal.x() > xMax) ||
		(retVal.y() < yMin) || (retVal.y() > yMax)) {
		//точка пересечения не принадлежит первому отрезку

		retVal.setX(GEO_ERROR_COORDINATE);
		retVal.setY(GEO_ERROR_COORDINATE);
		return retVal;
	}

	//и второй отрезок
	if (x3 < x4) {
		xMin = x3;
		xMax = x4;
	}
	else {
		xMin = x4;
		xMax = x3;
	}
	if (y3 < y4) {
		yMin = y3;
		yMax = y4;
	}
	else {
		yMin = y4;
		yMax = y3;
	}
	if ((retVal.x() < xMin) || (retVal.x() > xMax) ||
		(retVal.y() < yMin) || (retVal.y() > yMax)) {

		//точка пересечения не принадлежит второму отрезку
		retVal.setX(GEO_ERROR_COORDINATE);
		retVal.setY(GEO_ERROR_COORDINATE);
		return retVal;
	}

	//два отрезка пересекаются
	return retVal;
}

bool GeoDistance::isCorrectDouble(const double val)
{
	if (val > (GEO_ERROR_COORDINATE-1)) return false;
	return true;
}

bool GeoDistance::isCorrectInt(const int val)
{
	return isCorrectDouble((double)val);
}

bool GeoDistance::isCorrectGeoCoord(const double lon, const double lat)
{
	return isCorrectDouble(lon) && isCorrectDouble(lat);
}

bool GeoDistance::isSegmentCross(const double x1, const double y1,
								 const double x2, const double y2,
								 const double x3, const double y3,
								 const double x4, const double y4)
{
	QPointF cross_point = getSegmentCross(x1, y1,
										  x2, y2,
										  x3, y3,
										  x4, y4);

	if (!isCorrectDouble(cross_point.x())) return false;
	if (!isCorrectDouble(cross_point.y())) return false;

	return true;
}
