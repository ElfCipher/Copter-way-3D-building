#include <QApplication>
#include <form.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Form *wnd = new Form();

    wnd->show();

    return a.exec();
}
