#ifndef COPTER_WAY_H
#define COPTER_WAY_H

#include "iostream"
#include <GeoDistance.h>
#include <RouteTS.h>

class Copter
{
    private:

        double focus; //расстояние до здания
        double distance;
        double matrix_hor; //угол камеры
        double matrix_ver;
        double overlap_hor; //горизонтальное перекрытие в процентах
        double overlap_ver; //вертикальное
        double length_moving_hor;
        double length_moving_ver;
        double building_high;

        RouteTS::GeoPoint3DList building_coordinates;

        RouteTS::GeoPoint3D copter_coordinates_p;
        RouteTS::GeoPoint3DList copter_coordinates;

        struct Coefficients
        {
            Coefficients() = default;
            Coefficients(double a, double b, double c) : A(a), B(b), C(c) {}

            double A = 0.0;
            double B = 0.0;
            double C = 0.0;
        };

        using CoefficientsList = QList<Coefficients>;

        Coefficients coefficients_of_straights_p;
        CoefficientsList coefficients_of_straights; //коэффициенты уравнения Ax+By+C=0 между каждыми двумя соседними точками (контур)

        RouteTS::GeoPoint3D first_point; //опорная точка расчёта прямых
        RouteTS::GeoPoint3D intersection_point;

        void CalculateFirstPoint (uint32_t point_1, uint32_t point_2); //находим точку, с которой начнёт коптер: выбираем сами
        void CalculateIntersectionPoint(uint32_t straight_1, uint32_t straight_2);
        void CalculateCoefficientsOfStraights (); //уравнения прямых здания, за которые коптер не может залететь
        void CalculateHorizonCopterCoordinates(); //координаты коптера по горизонтали

    public:

        Copter (double focus, double matrix_ver, double matrix_hor, double distance, double overlap_ver, double overlap_hor, double building_high);
        void SetCoordinates (RouteTS::GeoPoint3DList app_coord); //устанавливаем координаты здания
        double Range (double x1, double y1, double x2, double y2); //между произвольными точками
        RouteTS::GeoPoint3DList GetCopterCoordinates(); //возвращает координаты коптера
};

#endif // COPTER_WAY_H
