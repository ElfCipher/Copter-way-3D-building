#include "RouteTS.h"

QByteArray RouteTS::serializeRoute(const RouteTS::GeoPoint3DList& points)
{
	QByteArray flags = QByteArray::fromHex("0100000069006e00");
	QByteArray resultBa;
	resultBa.reserve(points.count() * 4 * sizeof(double));

	for (auto& point : points) {
		resultBa.append(serializePoint(point, flags));
	}

    return resultBa;
}

RouteTS::GeoPoint3DList RouteTS::deserializeRoute(const QByteArray& ba)
{
	GeoPoint3DList resultList;

	const int pointSize = 4 * sizeof(double);

	for (int i = 0; i <= ba.size() - pointSize; i += pointSize) {
		resultList.append(deserializePoint(ba.data() + i));
	}

	return resultList;
}

QByteArray RouteTS::serializePoint(const RouteTS::GeoPoint3D& point, const QByteArray& flags)
{
	QByteArray ba;
	ba.append(reinterpret_cast<const char*>(&point.latitude), sizeof(point.latitude));
	ba.append(reinterpret_cast<const char*>(&point.longitude), sizeof(point.longitude));
	ba.append(reinterpret_cast<const char*>(&point.altitude), sizeof(point.altitude));
	ba.append(flags);

	return ba;
}

RouteTS::GeoPoint3D RouteTS::deserializePoint(const char* serialPoint)
{
	GeoPoint3D retPoint;

	retPoint.latitude = *(double*)(serialPoint);
	retPoint.longitude = *(double*)(serialPoint + sizeof(double));
	retPoint.altitude = *(double*)(serialPoint + 2 * sizeof(double));

	return retPoint;
}
