#include "form.h"
#include "ui_form.h"
#include <QMessageBox>
#include <QFile>
#include <RouteTS.h>
#include <copter_way.h>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->add_pointButton->setEnabled(false);
    ui->calcButton->setEnabled(false);
    ui->settingsButton->setEnabled(false);

    QRegExp exp("[0-9]{1,3}[.]{1}[0-9]{1,7}");
    ui->longitude->setValidator(new QRegExpValidator(exp, this));
    ui->latitude->setValidator(new QRegExpValidator(exp, this));
    ui->altitude->setValidator(new QRegExpValidator(exp, this));

    QRegExp capture("[0-9]{1,2}");
    ui->hor_overlap->setValidator(new QRegExpValidator(capture, this));
    ui->ver_overlap->setValidator(new QRegExpValidator(capture, this));

    QRegExp mm("[0-9]{1,2}[.]{0,1}[0-9]{0,1}");
    ui->focus->setValidator(new QRegExpValidator(mm, this));
    ui->hor_matrix->setValidator(new QRegExpValidator(mm, this));
    ui->ver_matrix->setValidator(new QRegExpValidator(mm, this));
    ui->build_high->setValidator(new QRegExpValidator(mm, this));
    ui->distance->setValidator(new QRegExpValidator(mm, this));

    connect(ui->longitude, SIGNAL(textChanged(QString)), this, SLOT(AddB_Enabled()));
    connect(ui->latitude, SIGNAL(textChanged(QString)), this, SLOT(AddB_Enabled()));
    connect(ui->altitude, SIGNAL(textChanged(QString)), this, SLOT(AddB_Enabled()));

    connect(ui->add_pointButton, SIGNAL(clicked(bool)), this, SLOT(CalcB_Enabled()));

    connect(ui->focus, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->ver_matrix, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->hor_matrix, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->distance, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->ver_overlap, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->hor_overlap, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
    connect(ui->build_high, SIGNAL(textChanged(QString)), this, SLOT(SettB_Enabled()));
}

void Form::CalcB_Enabled()
{
    if( (number_of_points>2) && set_enabled ) ui->calcButton->setEnabled(true);
}

void Form::AddB_Enabled()
{
    ui->add_pointButton->setEnabled( (ui->longitude->hasAcceptableInput()) && (ui->latitude->hasAcceptableInput())
                                     && (ui->altitude->hasAcceptableInput()) );
}

void Form::SettB_Enabled()
{
    if ( (ui->distance->hasAcceptableInput()) && (ui->hor_matrix->hasAcceptableInput()) &&
         (ui->ver_matrix->hasAcceptableInput()) && (ui->hor_overlap->hasAcceptableInput()) &&
         (ui->ver_overlap->hasAcceptableInput()) && (ui->build_high->hasAcceptableInput()) &&
         (ui->focus->hasAcceptableInput()) )
    {
        ui->settingsButton->setEnabled(true);
        set_enabled = true;
    }
}

Form::~Form()
{
    delete ui;
}

void Form::on_add_pointButton_clicked()
{
    QString longit = ui->longitude->text();
    QString latit = ui->latitude->text();
    QString altit = ui->altitude->text();

    add = add + "Longitude: " + longit + "\nLatitude: " + latit + "\nAltitude: " + altit + "\n\n";

    ui->textBrowser->setText(add);

    double longit_d = longit.toDouble();
    double latit_d = latit.toDouble();
    double altit_d = altit.toDouble();

    app_coord_build_p.longitude = longit_d;
    app_coord_build_p.latitude  = latit_d;
    app_coord_build_p.altitude  = altit_d;
    app_coord_build.append(app_coord_build_p);

    ui->longitude->clear();
    ui->latitude->clear();
    ui->altitude->clear();

    number_of_points++;
}

void Form::on_calcButton_clicked()
{
       RouteTS::GeoPoint3DList coordList;

       Copter *m_copter = new Copter(focus_f, matrix_ver_f, matrix_hor_f, distance_f, overlap_ver_f, overlap_hor_f, building_high_f);
       m_copter->SetCoordinates(app_coord_build);
       coordList = m_copter->GetCopterCoordinates();

       RouteTS *Box = new RouteTS();
       QByteArray byte_coordList = Box->serializeRoute(coordList);

       char file_name[100] = "Kvadrokopter_coordinates_0.rts";
       QFile file(file_name);
       file.open(QIODevice::WriteOnly);
       file.write(byte_coordList);
       file.close();

       QMessageBox end;

       end.setText("Done");
       end.show();
       end.exec();
}

void Form::on_settingsButton_clicked()
{
    QString Qfocus = ui->focus->text();
    QString Qver_mat = ui->ver_matrix->text();
    QString Qhor_mat = ui->hor_matrix->text();
    QString Qver_over = ui->ver_overlap->text();
    QString Qhor_over = ui->hor_overlap->text();
    QString Qdistance = ui->distance->text();
    QString Qhigh = ui->build_high->text();

    focus_f = Qfocus.toDouble();
    matrix_ver_f = Qver_mat.toDouble();
    matrix_hor_f = Qhor_mat.toDouble();
    overlap_ver_f = Qver_over.toDouble();
    overlap_hor_f = Qhor_over.toDouble();
    building_high_f = Qhigh.toDouble();
    distance_f = Qdistance.toDouble();

    ui->settingsButton->setEnabled(false);
    ui->focus->setEnabled(false);
    ui->ver_matrix->setEnabled(false);
    ui->hor_matrix->setEnabled(false);
    ui->distance->setEnabled(false);
    ui->ver_overlap->setEnabled(false);
    ui->hor_overlap->setEnabled(false);
    ui->build_high->setEnabled(false);
}
