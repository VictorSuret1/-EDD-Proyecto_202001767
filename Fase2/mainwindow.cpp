#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadmin.h"
#include <QMessageBox>
#include "arbolavl.h"
#include "vuser.h"
#include "matrix.h"
#include "listapublicaciones.h"
#include "arbolb.h"
#include "arbolabb.h"
#include "listapublicaciones.h"
#include "registrarse.h"
ListaPublicaciones lista;
Matrix matrizAmistades;
ArbolAVL arbol;
ArbolB arbolb;
ArbolABB arbolabb;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString correo = ui->lineEdit->text();
    QString contrasena = ui->lineEdit_2->text();

    if (correo == "admin@gmail.com" && contrasena == "EDD2S2024") {
        vAdmin *adminWindow = new vAdmin();
        adminWindow->show();
        this->close();
        return;
    }


    NodoAVL* usuario = arbol.buscar(correo);

    // Depuración para ver los valores
    qDebug() << "Correo ingresado: " << correo;
    qDebug() << "Contraseña ingresada: " << contrasena;

    if (usuario != nullptr) {
        qDebug() << "Usuario encontrado: " << usuario->usuario.correo << " - " << usuario->usuario.contrasena;
        if (usuario->usuario.contrasena == contrasena) {
            vUser *usuarioWindow = new vUser(usuario->usuario);
            usuarioWindow->show();
            this->close();
        } else {
            QMessageBox::warning(this, "Error", "Contraseña incorrecta.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Correo no encontrado.");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    Registrarse *ventanaRegistro = new Registrarse(this);
    ventanaRegistro->show();
}

