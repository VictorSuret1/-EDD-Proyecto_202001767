#include "comentar.h"
#include "ui_comentar.h"

Comentar::Comentar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Comentar)
{
    ui->setupUi(this);
}

Comentar::~Comentar()
{
    delete ui;
}

void Comentar::on_pushButton_clicked()
{
    comentario = ui->textEdit->toPlainText();
    accept();
}


void Comentar::on_pushButton_2_clicked()
{
    reject();
}

QString Comentar::getComentario() const
{
    return comentario;
}

