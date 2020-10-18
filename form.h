#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <RouteTS.h>
#include <QString>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:

    explicit Form(QWidget *parent = 0);
    ~Form();

private:

    Ui::Form *ui;

    RouteTS::GeoPoint3D app_coord_build_p;
    RouteTS::GeoPoint3DList app_coord_build;

    uint32_t number_of_points = 0;
    bool set_enabled = false;

    QString add;

    double focus_f; //расстояние до здания
    double distance_f;
    double matrix_hor_f; //угол камеры
    double matrix_ver_f;
    double overlap_hor_f; //горизонтальное перекрытие в процентах
    double overlap_ver_f; //вертикальное
    double building_high_f;


private slots:

    void AddB_Enabled();
    void CalcB_Enabled();
    void SettB_Enabled();
    void on_add_pointButton_clicked();
    void on_calcButton_clicked();
    void on_settingsButton_clicked();
};

#endif // FORM_H
