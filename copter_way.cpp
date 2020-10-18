#include <copter_way.h>
#include <GeoDistance.h>
#include <RouteTS.h>
#include "iostream"
#include "cmath"

Copter::Copter(double m_focus, double m_matrix_ver, double m_matrix_hor, double m_distance, double m_overlap_ver, double m_overlap_hor, double m_building_high)
{
    focus                = m_focus;
    matrix_ver           = m_matrix_ver;
    matrix_hor           = m_matrix_hor;
    distance             = m_distance;
    overlap_hor          = m_overlap_hor;
    overlap_ver          = m_overlap_ver;
    building_high        = m_building_high;

    double angle_of_capture_ver = 180/M_PI * 2*atan(matrix_ver/2/focus);     //угол камеры
    double angle_of_capture_hor = 180/M_PI * 2*atan(matrix_hor/2/focus);

    length_moving_hor    = distance * tan(angle_of_capture_hor/2*M_PI/180) * (1 - overlap_hor/100)*2;
    length_moving_ver    = distance * tan(angle_of_capture_ver/2*M_PI/180) * (1 - overlap_ver/100)*2;
}

void Copter::SetCoordinates(RouteTS::GeoPoint3DList app_coord)
{
    building_coordinates = app_coord;
}

void Copter::CalculateFirstPoint(uint32_t point_1, uint32_t point_2)
{
    double azimuth = GeoDistance::getAzimuthOnSphere(building_coordinates[point_1].longitude,
                                                     building_coordinates[point_1].latitude,
                                                     building_coordinates[point_2].longitude,
                                                     building_coordinates[point_2].latitude);
    QPointF first_point_p;

    first_point_p = GeoDistance::getPointByDistanceOnSphere(building_coordinates[point_1].longitude,
                                                            building_coordinates[point_1].latitude,
                                                            distance, azimuth + 90);

    first_point.longitude = first_point_p.x();
    first_point.latitude = first_point_p.y();

    double d = (first_point.longitude - building_coordinates[point_1].longitude) *
               (building_coordinates[point_2].latitude - building_coordinates[point_1].latitude) -
               (first_point.latitude - building_coordinates[point_1].latitude) *
               (building_coordinates[point_2].longitude - building_coordinates[point_1].longitude); //(х3 - х1)  (у2 - у1) - (у3 - у1)  (х2 - х1)

    if (d < 0)
    {
        first_point_p = GeoDistance::getPointByDistanceOnSphere(building_coordinates[point_1].longitude,
                                                                building_coordinates[point_1].latitude,
                                                                  distance, azimuth - 90);

        first_point.longitude = first_point_p.x();
        first_point.latitude  = first_point_p.y();
    }

}

void Copter::CalculateCoefficientsOfStraights()
{
    uint32_t i;
    uint32_t l = building_coordinates.size();

    for (i = 0; i < l-1 ; i++)
    {
        CalculateFirstPoint(i, i+1);

        coefficients_of_straights_p.A = building_coordinates[i].latitude - building_coordinates[i+1].latitude;
        coefficients_of_straights_p.B = building_coordinates[i+1].longitude - building_coordinates[i].longitude;

        //теперь найдём 3й коэффициент, соответствующий уравнению прямой коптера, учитывая, что они параллельны

        coefficients_of_straights_p.C = - coefficients_of_straights_p.A*first_point.longitude - coefficients_of_straights_p.B*first_point.latitude;
        coefficients_of_straights.append(coefficients_of_straights_p);
    }

    //теперь между первой и последней точками

    CalculateFirstPoint(i, 0);

    coefficients_of_straights_p.A = building_coordinates[i].latitude - building_coordinates[0].latitude;
    coefficients_of_straights_p.B = building_coordinates[0].longitude - building_coordinates[i].longitude;
    coefficients_of_straights_p.C = - coefficients_of_straights_p.A*first_point.longitude - coefficients_of_straights_p.B*first_point.latitude;
    coefficients_of_straights.append(coefficients_of_straights_p);
}

void Copter::CalculateIntersectionPoint(uint32_t straight_1, uint32_t straight_2)
{
    intersection_point.longitude = (coefficients_of_straights[straight_2].B * coefficients_of_straights[straight_1].C -
                                    coefficients_of_straights[straight_1].B * coefficients_of_straights[straight_2].C) /
                                   (coefficients_of_straights[straight_2].A * coefficients_of_straights[straight_1].B -
                                    coefficients_of_straights[straight_1].A * coefficients_of_straights[straight_2].B);

    intersection_point.latitude = -(coefficients_of_straights[straight_1].A * intersection_point.longitude +
                                    coefficients_of_straights[straight_1].C) / coefficients_of_straights[straight_1].B;
}

void Copter::CalculateHorizonCopterCoordinates()
{
    QPointF buffer;
    double length_between;
    double azimuth;
    uint32_t i = 0, j = 0;

    uint32_t l = coefficients_of_straights.size();

    //точка пересечения двух прямых, от которой будет двигаться коптер.
    CalculateIntersectionPoint(0, 1);

    copter_coordinates_p.longitude = intersection_point.longitude;
    copter_coordinates_p.latitude  = intersection_point.latitude;
    copter_coordinates_p.altitude  = length_moving_ver + building_coordinates[0].altitude;
    copter_coordinates.append(copter_coordinates_p);

    for (i = 1; i <= l; i++)
    {
        //точка пересечения следующих прямых, до которой будет двигаться коптер
        if (i == l-1)
            CalculateIntersectionPoint(i, 0);
        else if (i == l)
        {
            intersection_point.longitude = copter_coordinates[0].longitude;
            intersection_point.latitude  = copter_coordinates[0].latitude;
        }
        else
            CalculateIntersectionPoint(i, i+1);

        azimuth = GeoDistance::getAzimuthOnSphere(copter_coordinates[j].longitude, copter_coordinates[j].latitude,
                                                  intersection_point.longitude, intersection_point.latitude);
        azimuth = azimuth * 180/M_PI;
        length_between = GeoDistance::getDistanceOnSphere(copter_coordinates[j].longitude, copter_coordinates[j].latitude,
                                                          intersection_point.longitude, intersection_point.latitude);

        for(j = j + 1; length_between > length_moving_hor; j++, length_between -= length_moving_hor)
        {
            buffer = GeoDistance::getPointByDistanceOnSphere(copter_coordinates[j-1].longitude, copter_coordinates[j-1].latitude,
                                                             length_moving_hor, azimuth);

            copter_coordinates_p.longitude = buffer.x();
            copter_coordinates_p.latitude  = buffer.y();
            copter_coordinates_p.altitude  = copter_coordinates[0].altitude;
            copter_coordinates.append(copter_coordinates_p);
        }

        copter_coordinates_p.longitude = intersection_point.longitude;
        copter_coordinates_p.latitude  = intersection_point.latitude;
        copter_coordinates_p.altitude  = copter_coordinates[0].altitude;
        copter_coordinates.append(copter_coordinates_p);

    }

}

 RouteTS::GeoPoint3DList Copter::GetCopterCoordinates()
{
    uint32_t number_of_points_ver = (uint32_t)(building_high / length_moving_ver);

    CalculateCoefficientsOfStraights();
    CalculateHorizonCopterCoordinates();

    uint32_t l = copter_coordinates.size();

    for(uint32_t i = 1; i <= number_of_points_ver; i++)
    {
        for (uint32_t j = 0; j < l; j++)
        {
            copter_coordinates_p.longitude = copter_coordinates[j].longitude;
            copter_coordinates_p.latitude  = copter_coordinates[j].latitude;
            copter_coordinates_p.altitude  = i*length_moving_ver + copter_coordinates[0].altitude;
            copter_coordinates.append(copter_coordinates_p);
        }
    }

    return copter_coordinates;
}








