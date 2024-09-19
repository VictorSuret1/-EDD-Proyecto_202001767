#include "verarbolcomentarios.h"
#include "ui_verarbolcomentarios.h"

VerArbolComentarios::VerArbolComentarios(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerArbolComentarios)
{
    ui->setupUi(this);
}

VerArbolComentarios::~VerArbolComentarios()
{
    delete ui;
}
