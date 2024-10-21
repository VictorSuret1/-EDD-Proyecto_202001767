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
#include "grafoamistades.h"
#include "adjacencylist.h"
#include "blockchain.h"
#include "merkletree.h"
#include "huffman.h"
#include "sha256.h"
Huffman huffman;

Blockchain blockchain;
MerkleTree merkletree;

AdjacencyList adlist;
GrafoAmistades grafo;
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

    if (arbol.raiz == nullptr) {
        QString datosDescomprimidos = huffman.decompress("backup.edd");
        if (!datosDescomprimidos.isEmpty()) {
            cargarDatosDesdeTexto(datosDescomprimidos);
            std::cout << "Datos descomprimidos y cargados correctamente." << std::endl;
        }

        blockchain.cargarBlockchain();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString correo = ui->lineEdit->text();
    QString contrasena = ui->lineEdit_2->text();

    // Hash the entered password
    std::string contrasenaIngresadaHashed = sha256(contrasena.toStdString());
    QString contrasenaIngresadaHashedQString = QString::fromStdString(contrasenaIngresadaHashed);

    if (correo == "admin@gmail.com" && contrasena == "EDD2S2024") {
        vAdmin *adminWindow = new vAdmin();
        adminWindow->show();
        this->close();
        return;
    }

    NodoAVL* usuario = arbol.buscar(correo);

    // Debugging
    qDebug() << "Correo ingresado: " << correo;
    qDebug() << "Contraseña ingresada (hashed): " << contrasenaIngresadaHashedQString;

    if (usuario != nullptr) {
        qDebug() << "Usuario encontrado: " << usuario->usuario.correo << " - " << usuario->usuario.contrasena;

        // Compare the hashed entered password with the stored hashed password
        if (usuario->usuario.contrasena == contrasenaIngresadaHashedQString) {
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

void MainWindow::cargarDatosDesdeTexto(const QString& datosDescomprimidos) {
    QStringList lineas = datosDescomprimidos.split("\n");
    QString seccionActual;

    for (const QString& linea : lineas) {
        if (linea.isEmpty()) {
            continue;
        }

        if (linea.startsWith("Usuarios:")) {
            seccionActual = "Usuarios";
            continue;
        } else if (linea.startsWith("Amigos:")) {
            seccionActual = "Amigos";
            continue;
        } else if (linea.startsWith("Solicitudes Enviadas:")) {
            seccionActual = "SolicitudesEnviadas";
            continue;
        } else if (linea.startsWith("Solicitudes Recibidas:")) {
            seccionActual = "SolicitudesRecibidas";
            continue;
        }

        if (seccionActual == "Usuarios") {
            // Procesar datos de usuario
            if (linea.startsWith("Usuario: ")) {
                QStringList partes = linea.split(", ");
                if (partes.size() == 3) {
                    QString nombres = partes[0].mid(9);  // Remover "Usuario: "
                    QString correo = partes[1].mid(8);   // Remover "Correo: "
                    QString contrasena = partes[2].mid(12); // Remover "Contraseña: "

                    Usuario usuario;
                    usuario.nombres = nombres;
                    usuario.correo = correo;
                    usuario.contrasena = contrasena;

                    arbol.insertar(usuario);
                } else {
                    std::cerr << "Formato inválido en la línea de usuario: " << linea.toStdString() << std::endl;
                }
            }
        } else if (seccionActual == "Amigos") {
            // Procesar datos de amigos
            if (linea.startsWith("Usuario: ")) {
                QStringList partes = linea.split(" tiene amigos: ");
                if (partes.size() == 2) {
                    QString usuario = partes[0].mid(9);  // Remover "Usuario: "
                    QStringList amigos = partes[1].split(", ");

                    for (const QString& amigo : amigos) {
                        grafo.agregarAmistad(usuario, amigo);
                    }
                } else {
                    std::cerr << "Formato inválido en la línea de amigos: " << linea.toStdString() << std::endl;
                }
            }
        }
    }
}
