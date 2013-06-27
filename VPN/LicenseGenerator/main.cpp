#include <QApplication>
#include <QFont>
#include <QPushButton>

#include "Generator.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Generator *gen = new Generator();
    gen->show();

    return app.exec();
}
