#include "vadmin.h"
#include "ui_vadmin.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "arbolavl.h"
#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "matrix.h"
#include "arbolb.h"
#include "arbolabb.h"
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QProcess>
#include "listapublicaciones.h"

using json = nlohmann::json;

extern ArbolAVL arbol;
extern Matrix matrizAmistades;
extern ArbolB arbolb;
extern ArbolABB arbolabb;
extern ListaPublicaciones lista;

vAdmin::vAdmin(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::vAdmin)
{
    ui->setupUi(this);
    llenarTablaUsuarios(arbol.raiz);
}

vAdmin::~vAdmin()
{
    delete ui;
}



void cargarUsuariosDesdeJSON(const std::string& path, ArbolAVL& arbol) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        QMessageBox::warning(nullptr, "Error", "No se pudo abrir el archivo: " + QString::fromStdString(path));
        return;
    }

    json root;
    archivo >> root;

    QString datosCargados;  // Cadena para acumular los datos a mostrar

    for (const auto& usuarioData : root) {
        if (usuarioData.contains("nombres") &&
            usuarioData.contains("apellidos") &&
            usuarioData.contains("fecha_de_nacimiento") &&
            usuarioData.contains("correo") &&
            usuarioData.contains("contraseña")) {

            Usuario nuevoUsuario;
            nuevoUsuario.nombres = QString::fromStdString(usuarioData["nombres"]);
            nuevoUsuario.apellidos = QString::fromStdString(usuarioData["apellidos"]);
            nuevoUsuario.fechaNacimiento = QString::fromStdString(usuarioData["fecha_de_nacimiento"]);
            nuevoUsuario.correo = QString::fromStdString(usuarioData["correo"]);
            nuevoUsuario.contrasena = QString::fromStdString(usuarioData["contraseña"]);

            arbol.insertar(nuevoUsuario);

            qDebug() << "Usuario cargado: " << nuevoUsuario.correo << " - " << nuevoUsuario.contrasena;

            // Agregar los datos del usuario a la cadena
            datosCargados += "Nombres: " + nuevoUsuario.nombres + "\n";
            datosCargados += "Apellidos: " + nuevoUsuario.apellidos + "\n";
            datosCargados += "Fecha de Nacimiento: " + nuevoUsuario.fechaNacimiento + "\n";
            datosCargados += "Correo: " + nuevoUsuario.correo + "\n";
            datosCargados += "----------------------------------------\n";
        } else {
            QMessageBox::warning(nullptr, "Error", "JSON malformado. Faltan campos requeridos para un usuario.");
        }
    }



    QMessageBox::information(nullptr, "Éxito", "Carga masiva de usuarios completada.");

}


void cargarSolicitudesDesdeJSON(const std::string& path) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        QMessageBox::warning(nullptr, "Error", "No se pudo abrir el archivo: " + QString::fromStdString(path));
        return;
    }

    json root;
    archivo >> root;

    for (const auto& solicitudData : root) {

        if (solicitudData.contains("emisor") &&
            solicitudData.contains("receptor") &&
            solicitudData.contains("estado")) {

            std::string correoEmisor = solicitudData["emisor"];
            std::string correoReceptor = solicitudData["receptor"];
            std::string estado = solicitudData["estado"];

            NodoAVL* nodoEmisor = arbol.buscar(QString::fromStdString(correoEmisor));
            NodoAVL* nodoReceptor = arbol.buscar(QString::fromStdString(correoReceptor));

            if (!nodoEmisor || !nodoReceptor) {
                QMessageBox::warning(nullptr, "Error"," Emisor o receptor no encontrados para la solicitud ");
                continue;
            }

            if (estado == "PENDIENTE") {
                Solicitud* nuevaSolicitud = new Solicitud;
                nuevaSolicitud->correoEmisor = QString::fromStdString(correoEmisor);
                nuevaSolicitud->correoReceptor = QString::fromStdString(correoReceptor);

                nodoEmisor->usuario.solicitudesEnviadas.append(nuevaSolicitud);

                nodoReceptor->usuario.solicitudesRecibidas.push_back(nuevaSolicitud);

            } else if (estado == "ACEPTADA") {

                matrizAmistades.insertarAmistad(correoEmisor, correoReceptor);
            }
        } else {
            QMessageBox::warning(nullptr, "Error", "JSON malformado. Faltan campos requeridos para un usuario.");
        }
    }

    QMessageBox::information(nullptr, "Éxito", "Carga masiva de usuarios completada.");
}




void cargarComentariosDesdeJSON(const std::string& path) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << path << "\n";
        return;
    }

    json root;
    archivo >> root;

    for (const auto& publicacionData : root) {
        if (publicacionData.contains("correo") &&
            publicacionData.contains("contenido") &&
            publicacionData.contains("fecha") &&
            publicacionData.contains("hora")) {

            QString correo = QString::fromStdString(publicacionData["correo"]);
            QString contenido = QString::fromStdString(publicacionData["contenido"]);
            QString fechaStr = QString::fromStdString(publicacionData["fecha"]);
            QString horaStr = QString::fromStdString(publicacionData["hora"]);
            QString rutaImagen = publicacionData.contains("imagen") ? QString::fromStdString(publicacionData["imagen"]) : QString();

            QDateTime fechaHora = QDateTime::fromString(fechaStr + " " + horaStr, "dd/MM/yyyy hh:mm");

            arbolabb.insertarPublicacion(correo, contenido, rutaImagen, fechaHora.date());

            NodoABB* nodoPublicacion = arbolabb.buscar(fechaHora.date());

            if (nodoPublicacion) {
                Publicacion* actualPublicacion = nodoPublicacion->listaPublicaciones;
                while (actualPublicacion && actualPublicacion->contenido != contenido) {
                    actualPublicacion = actualPublicacion->siguiente;
                }

                if (actualPublicacion) {
                    if (actualPublicacion->arbolComentarios.raiz == nullptr) {
                        actualPublicacion->arbolComentarios = ArbolB();
                    }

                    // Procesar los comentarios si existen
                    if (publicacionData.contains("comentarios")) {
                        for (const auto& comentarioData : publicacionData["comentarios"]) {
                            if (comentarioData.contains("correo") &&
                                comentarioData.contains("comentario") &&
                                comentarioData.contains("fecha") &&
                                comentarioData.contains("hora")) {

                                QString correoComentario = QString::fromStdString(comentarioData["correo"]);
                                QString textoComentario = QString::fromStdString(comentarioData["comentario"]);
                                QString fechaComentarioStr = QString::fromStdString(comentarioData["fecha"]);
                                QString horaComentarioStr = QString::fromStdString(comentarioData["hora"]);
                                QDateTime fechaHoraComentario = QDateTime::fromString(fechaComentarioStr + " " + horaComentarioStr, "dd/MM/yyyy hh:mm");

                                actualPublicacion->arbolComentarios.insertarComentario(correoComentario, textoComentario, fechaHoraComentario);


                            }
                        }
                    }

                    lista.agregarPublicacion(correo, contenido, rutaImagen, fechaHora);

                } else {
                    std::cout << "Error: No se encontró la publicación para el contenido especificado.\n";
                }
            } else {
                std::cout << "Error: No se encontró el nodo de la publicación para la fecha especificada.\n";
            }
        } else {
            std::cout << "Error: JSON malformado. Faltan campos requeridos para una publicación.\n";
        }
    }

    std::cout << "Carga masiva de publicaciones y comentarios completada.\n";
}



void vAdmin::on_pushButton_clicked()
{
    // Abrir el cuadro de diálogo para seleccionar un archivo
            QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir archivo"), "", tr("Archivos JSON (*.json);;Todos los archivos (*)"));

    // Verificar si se seleccionó un archivo
    if (!archivo.isEmpty()) {
        // Mostrar la ruta del archivo
        ui->lineEdit->setText(archivo);

        cargarUsuariosDesdeJSON(archivo.toStdString(),arbol);
        arbol.mostrarUsuarios();
        QMessageBox::information(this, tr("Carga Completada"), tr("Los usuarios han sido cargados correctamente."));
    } else {
        QMessageBox::information(this, tr("No se seleccionó archivo"), tr("Por favor seleccione un archivo para continuar."));
    }

    llenarTablaUsuarios(arbol.raiz);
}


void vAdmin::on_pushButton_4_clicked()
{

    MainWindow *loginWindow = new MainWindow();
    loginWindow->show();


    this->close();

}


void vAdmin::on_pushButton_2_clicked()
{
    QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir archivo"), "", tr("Archivos JSON (*.json);;Todos los archivos (*)"));

    if (!archivo.isEmpty()) {

        ui->lineEdit_2->setText(archivo);

        cargarSolicitudesDesdeJSON(archivo.toStdString());
        QMessageBox::information(this, tr("Carga Completada"), tr("Las solicitudes han sido cargados correctamente."));
    } else {
        QMessageBox::information(this, tr("No se seleccionó archivo"), tr("Por favor seleccione un archivo para continuar."));
    }
}


void vAdmin::on_pushButton_3_clicked()
{
    QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir archivo"), "", tr("Archivos JSON (*.json);;Todos los archivos (*)"));

    if (!archivo.isEmpty()) {
        ui->lineEdit_3->setText(archivo);

        cargarComentariosDesdeJSON(archivo.toStdString());
        arbolabb.mostrarPublicaciones();
        QMessageBox::information(this, tr("Carga Completada"), tr("Las publicaiones  han sido cargados correctamente."));
    } else {
        QMessageBox::information(this, tr("No se seleccionó archivo"), tr("Por favor seleccione un archivo para continuar."));
    }
}

void vAdmin::llenarTablaUsuarios(NodoAVL* nodo) {
    if (!nodo) return;

    // Recorrer el árbol inorden para llenar la tabla ordenadamente
    llenarTablaUsuarios(nodo->izquierdo);

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    // Columna de Nombre
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(nodo->usuario.nombres));

    // Columna de Apellidos
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(nodo->usuario.apellidos));

    // Columna de Correo
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(nodo->usuario.correo));

    // Columna de Fecha de Nacimiento
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(nodo->usuario.fechaNacimiento));

    // Botón Modificar
    QPushButton* btnModificar = new QPushButton("Modificar");
    connect(btnModificar, &QPushButton::clicked, this, [=]() { modificarUsuario(nodo->usuario.correo); });
    ui->tableWidget->setCellWidget(row, 4, btnModificar);

    // Botón Eliminar
    QPushButton* btnEliminar = new QPushButton("Eliminar");
    connect(btnEliminar, &QPushButton::clicked, this, [=]() { eliminarUsuario(nodo->usuario.correo); });
    ui->tableWidget->setCellWidget(row, 5, btnEliminar);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);


    llenarTablaUsuarios(nodo->derecho);
}



void vAdmin::modificarUsuario(const QString& correo) {
    // Buscar la fila correspondiente al correo
    int row = -1;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (ui->tableWidget->item(i, 2)->text() == correo) {
            row = i;
            break;
        }
    }

    if (row == -1) {
        QMessageBox::warning(this, "Error", "No se encontró el usuario para modificar.");
        return;
    }

    // Obtener los nuevos datos del usuario desde la tabla
    QString nuevoNombre = ui->tableWidget->item(row, 0)->text();
    QString nuevoApellido = ui->tableWidget->item(row, 1)->text();
    QString nuevoCorreo = ui->tableWidget->item(row, 2)->text();
    QString nuevaFechaNacimiento = ui->tableWidget->item(row, 3)->text();

    // Verificar si los datos han cambiado
    NodoAVL* nodo = arbol.buscar(correo);
    if (!nodo) {
        QMessageBox::warning(this, "Error", "No se encontró el nodo en el árbol.");
        return;
    }

    bool cambiado = false;
    if (nodo->usuario.nombres != nuevoNombre) {
        nodo->usuario.nombres = nuevoNombre;
        cambiado = true;
    }
    if (nodo->usuario.apellidos != nuevoApellido) {
        nodo->usuario.apellidos = nuevoApellido;
        cambiado = true;
    }
    if (nodo->usuario.correo != nuevoCorreo) {
        // Si el correo cambia, eliminar e insertar el nodo nuevamente
        arbol.eliminar(correo);  // Eliminar el nodo con el correo viejo
        nodo->usuario.correo = nuevoCorreo;
        arbol.insertar(nodo->usuario);  // Insertar el nodo con el nuevo correo
        cambiado = true;
    }
    if (nodo->usuario.fechaNacimiento != nuevaFechaNacimiento) {
        nodo->usuario.fechaNacimiento = nuevaFechaNacimiento;
        cambiado = true;
    }

    if (cambiado) {
        QMessageBox::information(this, "Modificación", "Usuario modificado correctamente.");
        actualizarTabla(); // Actualizar la tabla
    } else {
        QMessageBox::information(this, "Modificación", "No se realizaron cambios.");
    }
}

void vAdmin::eliminarUsuario(const QString& correo) {
    arbol.eliminar(correo);
    actualizarTabla();
    QMessageBox::information(this, "Eliminar", "Usuario eliminado correctamente.");
}

void vAdmin::actualizarTabla() {
    // Limpiar la tabla y volver a llenarla
    ui->tableWidget->setRowCount(0);
    llenarTablaUsuarios(arbol.obtenerRaiz());
}

void vAdmin::on_pushButton_5_clicked()
{
    QString correo = ui->lineEdit_4->text();

    NodoAVL* nodoUsuario = arbol.buscar(correo);

    if (nodoUsuario != nullptr) {
        ui->tableWidget->setRowCount(0);
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(nodoUsuario->usuario.nombres));

        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(nodoUsuario->usuario.apellidos));

        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(nodoUsuario->usuario.correo));

        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(nodoUsuario->usuario.fechaNacimiento));

        QPushButton* btnModificar = new QPushButton("Modificar");
        connect(btnModificar, &QPushButton::clicked, this, [=]() { modificarUsuario(nodoUsuario->usuario.correo); });
        ui->tableWidget->setCellWidget(row, 4, btnModificar);

        QPushButton* btnEliminar = new QPushButton("Eliminar");
        connect(btnEliminar, &QPushButton::clicked, this, [=]() { eliminarUsuario(nodoUsuario->usuario.correo); });
        ui->tableWidget->setCellWidget(row, 5, btnEliminar);
    } else {
        QMessageBox::warning(this, "Usuario no encontrado", "No se encontró un usuario con el correo proporcionado.");
    }
}


void vAdmin::on_pushButton_6_clicked()
{
    // Obtener el tipo de recorrido seleccionado del ComboBox
    QString tipoRecorrido = ui->comboBox->currentText();

    // Obtener la lista de usuarios según el recorrido seleccionado
    QList<Usuario> usuarios = arbol.obtenerRecorrido(tipoRecorrido);

    // Limpiar la tabla antes de agregar nuevos datos
    ui->tableWidget->setRowCount(0);

    // Llenar la tabla con los usuarios obtenidos
    int row = 0;
    for (const Usuario& usuario : usuarios) {
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(usuario.nombres));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(usuario.apellidos));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(usuario.correo));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(usuario.fechaNacimiento));

        // Añadir botones Modificar y Eliminar en la tabla
        QPushButton* modificarButton = new QPushButton("Modificar");
        QPushButton* eliminarButton = new QPushButton("Eliminar");


        ui->tableWidget->setCellWidget(row, 4, modificarButton);
        ui->tableWidget->setCellWidget(row, 5, eliminarButton);

        row++;
    }

}


void mostrarImagenEnGroupBox(QGroupBox* groupBox, const QString& rutaImagen) {
    QPixmap pixmap(rutaImagen);

    if (!pixmap.isNull()) {
        // Verificar si el groupBox ya tiene un layout
        QLayout* layout = groupBox->layout();
        if (!layout) {
            // Si no tiene un layout, crear uno nuevo y asignarlo
            layout = new QVBoxLayout(groupBox);
            groupBox->setLayout(layout);
        } else {
            // Si ya tiene un layout, limpiar los widgets existentes
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget(); // Eliminar los widgets del layout
                delete item;           // Eliminar el item del layout
            }
        }

        // Crear un nuevo QLabel con la imagen
        QLabel* label = new QLabel(groupBox);
        label->setPixmap(pixmap);
        label->setScaledContents(true);

        // Añadir el QLabel al layout existente
        layout->addWidget(label);
    } else {
        qDebug() << "No se pudo cargar la imagen en el QGroupBox.";
    }
}

void generarImagenDesdeDot(const QString& rutaDot, const QString& rutaImagen) {
    QString comando = "dot";
    QStringList argumentos;
    argumentos << "-Tpng" << rutaDot << "-o" << rutaImagen;

    QProcess proceso;
    proceso.setWorkingDirectory(QDir::currentPath());
    proceso.start(comando, argumentos);
    proceso.waitForFinished();

    if (proceso.exitStatus() == QProcess::NormalExit) {
        qDebug() << "Imagen generada exitosamente.";
    } else {
        qDebug() << "Error al generar la imagen: " << proceso.errorString();
    }
}


void graficarListaPublicaciones(ListaPublicaciones& lista) {
    std::ofstream archivo("ListaPublicaciones.dot");

    if (archivo.is_open()) {
        archivo << "digraph G {" << std::endl;
        archivo << "node [shape=record];" << std::endl;

        Publicacion* actual = lista.getCabeza();
        int i = 0;

        // Crear nodos y enlaces
        while (actual != nullptr) {
            archivo << "nodo" << i << " [label=\"{Correo: "
                    << actual->correoUsuario.toStdString()
                    << " | Contenido: " << actual->contenido.toStdString()
                    << " | Fecha: " << actual->fechaHora.toString("dd/MM/yyyy hh:mm:ss").toStdString()
                    << " | Imagen: " << actual->rutaImagen.toStdString() << "}\"];" << std::endl;

            if (actual->siguiente != nullptr) {
                archivo << "nodo" << i << " -> nodo" << i + 1 << " [dir=both];" << std::endl;
            }

            actual = actual->siguiente;
            i++;
        }

        archivo << "}" << std::endl;
        archivo.close();

        // Verificar si el archivo se generó correctamente
        qDebug() << "Archivo DOT generado correctamente: ListaPublicaciones.dot";

    } else {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
    }
}


void convertirDotAImagen(const std::string& nombreArchivoDot, const std::string& nombreArchivoImagen) {
    std::string comando = "dot -Tpng " + nombreArchivoDot + " -o " + nombreArchivoImagen;
    int resultado = system(comando.c_str());

    if (resultado == 0) {
        std::cout << "Imagen generada exitosamente: " << nombreArchivoImagen << std::endl;
    } else {
        std::cerr << "Error al generar la imagen desde el archivo DOT." << std::endl;
    }
}


void vAdmin::on_pushButton_7_clicked()
{
    QString rutaDot = "arbolAVL.dot";
    QString rutaImagen = "arbolAVL.png";

    QString ruta2 = "ListaPublicaciones.png";

    arbol.exportarArbolAVLaDot(arbol, rutaDot.toStdString());

    generarImagenDesdeDot(rutaDot, rutaImagen);

    mostrarImagenEnGroupBox(ui->groupBox_4, rutaImagen);

    graficarListaPublicaciones(lista);
    convertirDotAImagen("ListaPublicaciones.dot","ListaPublicaciones.png" );
    mostrarImagenEnGroupBox(ui->groupBox_5,ruta2);

    lista.mostrarPublicaciones();

}

