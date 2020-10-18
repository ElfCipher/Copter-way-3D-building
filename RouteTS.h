#pragma once

#include <QByteArray>
#include <QPointF>
#include <QList>

class RouteTS
{
public:
    //RouteTS() = delete;

	struct GeoPoint3D {
		GeoPoint3D() = default;
		GeoPoint3D(double lon, double lat, double alt) :
			longitude(lon), latitude(lat), altitude(alt) {}

		double longitude = 0.0;
		double latitude = 0.0;
		double altitude = 0.0;
	};

	using GeoPoint3DList = QList<GeoPoint3D>;

	static QByteArray serializeRoute(const GeoPoint3DList& points);
	static GeoPoint3DList deserializeRoute(const QByteArray& ba);

private:
	static QByteArray serializePoint(const RouteTS::GeoPoint3D& point, const QByteArray& flags);
	static GeoPoint3D deserializePoint(const char* serialPoint);
};
