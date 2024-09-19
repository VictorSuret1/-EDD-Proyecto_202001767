#include "nuevapublicacion.h"
#include "ui_nuevapublicacion.h"
#include <QFileDialog>
#include <QMessageBox>
NuevaPublicacion::NuevaPublicacion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NuevaPublicacion)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &NuevaPublicacion::seleccionarImagen);

    // Conectar el botón Aceptar
    connect(ui->pushButton_2, &QPushButton::clicked, this, &NuevaPublicacion::aceptarPublicacion);

    // Conectar el botón Cancelar
    connect(ui->pushButton_3, &QPushButton::clicked, this, &NuevaPublicacion::reject);


}

NuevaPublicacion::~NuevaPublicacion()
{
    delete ui;
}

QString NuevaPublicacion::getTextoPublicacion() const {
    return ui->plainTextEdit->toPlainText();
}

QString NuevaPublicacion::getRutaImagen() const {
    return ui->label->text();
}

void NuevaPublicacion::seleccionarImagen() {
    QString rutaImagen = QFileDialog::getOpenFileName(this, "Seleccionar Imagen", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!rutaImagen.isEmpty()) {
        ui->label->setText(rutaImagen);
    }
}

void NuevaPublicacion::aceptarPublicacion() {
    // Verifica si el campo de texto no está vacío
    if (!getTextoPublicacion().isEmpty()) {
        accept();  // Cierra el diálogo con un resultado de aceptación
    } else {
        QMessageBox::warning(this, "Advertencia", "El campo de texto de la publicación no puede estar vacío.");
    }
}
