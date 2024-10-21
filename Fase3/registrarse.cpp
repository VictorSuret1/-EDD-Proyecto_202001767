#include "registrarse.h"
#include "ui_registrarse.h"
#include "arbolavl.h"
#include <QMessageBox>
#include "sha256.h"
extern ArbolAVL arbol;
Registrarse::Registrarse(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Registrarse)
{
    ui->setupUi(this);
}

Registrarse::~Registrarse()
{
    delete ui;
}

void Registrarse::on_pushButton_clicked()
{
    QString nuevosNombres = ui->lineEdit->text();
    QString nuevosApellidos = ui->lineEdit_2->text();
    QString nuevoCorreo = ui->lineEdit_4->text();
    QString nuevaContra = ui->lineEdit_5->text();
    QString nuevaFechaNacimiento = ui->dateEdit->date().toString("dd/MM/yyyy");

    if (nuevosNombres.isEmpty() || nuevosApellidos.isEmpty() || nuevoCorreo.isEmpty() || nuevaContra.isEmpty()) {
        QMessageBox::warning(this, "Campos Vacíos", "Por favor llena todos los campos.");
        return;
    }

    if (arbol.buscar(nuevoCorreo) != nullptr) {
        QMessageBox::warning(this, "Correo Ya Registrado", "El correo ingresado ya está registrado. Por favor usa otro.");
        return;
    }

    Usuario nuevoUsuario;
    nuevoUsuario.nombres = nuevosNombres;
    nuevoUsuario.apellidos = nuevosApellidos;
    nuevoUsuario.correo = nuevoCorreo;
    nuevoUsuario.fechaNacimiento = nuevaFechaNacimiento;

    std::string contrasenaOriginal = nuevaContra.toStdString();
    std::string contrasenaHashed = sha256(contrasenaOriginal);
    nuevoUsuario.contrasena = QString::fromStdString(contrasenaHashed);

    arbol.insertar(nuevoUsuario);

    QMessageBox::information(this, "Registro Exitoso", "El usuario ha sido registrado correctamente.");

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    this->close();
}

