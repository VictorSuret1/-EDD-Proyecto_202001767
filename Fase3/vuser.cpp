#include "vuser.h"
#include "ui_vuser.h"
#include "mainwindow.h"
#include "arbolavl.h"
#include <iostream>
#import <QMessageBox>
#import "matrix.h"
#import "nodoavl.h"
#include <QHBoxLayout>
#include "arbolabb.h"
#include "nuevapublicacion.h"
#include "listapublicaciones.h"
#include "comentar.h"
#include "vercomentarios.h"
#include "arbolb.h"
#include <unordered_set>
#include <QProcess>
#include <fstream>
#include "grafoamistades.h"
#include "blockchain.h"
#include "merkletree.h"
#include "huffman.h"
#include "sha256.h"
extern Huffman huffman;
extern MerkleTree merkleTree;
extern Blockchain blockchain;
extern GrafoAmistades grafo;
extern ArbolAVL arbol;
extern Matrix matrizAmistades;
extern ArbolABB arbolabb;
extern ListaPublicaciones lista;
extern ArbolB arbolb;

vUser::vUser(const Usuario& usuario, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::vUser),
    usuarioActual(usuario)
{
    ui->setupUi(this);
    ui->label_3->setText(usuarioActual.nombres);
    ui->lineEdit_6->setText(usuarioActual.nombres);
    ui->lineEdit_7->setText(usuarioActual.apellidos);
    ui->lineEdit_8->setText(usuarioActual.correo);
    ui->lineEdit_9->setText(usuarioActual.contrasena);

    QDate fecha = QDate::fromString(usuarioActual.fechaNacimiento, "dd/MM/yyyy");
    ui->dateEdit->setDate(fecha);

    llenarTablaDesdeArbol(arbol.raiz);
    llenarTablaSolicitudes();
    llenarTablaSolicitudesEnviadas();

    mostrarPublicacionesUsuario(ui->scrollArea,&usuarioActual,arbolabb);

    connect(ui->pushButton_5, &QPushButton::clicked, this, &vUser::agregarNuevaPublicacion);

    connect(ui->pushButton_6, &QPushButton::clicked, this, &vUser::mostrarPublicacionesPorFecha);

    poblarComboBoxFechas();
    poblarComboBoxABB();
    mostrarSugerenciasAmistad(usuario.correo);
    grafo.graficarSugerenciasAmistad(usuario.correo,"sugerencias_amistad");
    mostrarImagenEnGroupBox(ui->groupBox_13,"sugerencias_amistad.png");
}

vUser::~vUser()
{
    delete ui;
}


Usuario* obtenerUsuario(const std::string& correo) {
    NodoAVL* nodo = arbol.buscar(QString::fromStdString(correo));
    if (nodo) {
        return &nodo->usuario;
    }
    return nullptr;
}


QString prepararDatosParaCompresion(const GrafoAmistades& grafoAmistades) {
    QString datos;

    // Obtener información de los usuarios
    datos += "Usuarios:\n";
    QList<Usuario> usuarios = arbol.obtenerRecorrido("InOrden");
    for (const Usuario& usuario : usuarios) {
        datos += "Usuario: " + usuario.nombres + ", Correo: " + usuario.correo + ", Contraseña: " + usuario.contrasena + "\n";
    }

    // Obtener información de las amistades
    datos += "\nAmigos:\n";
    for (const Usuario& usuario : usuarios) {
        QString correo = usuario.correo;
        QList<QString> amigos = grafoAmistades.obtenerAmigos(correo);
        datos += "Usuario: " + correo + " tiene amigos: " + amigos.join(", ") + "\n";
    }

    // Obtener información de las solicitudes enviadas y recibidas
    datos += "\nSolicitudes Enviadas:\n";
    for (const Usuario& usuario : usuarios) {
        for (const Solicitud* solicitud : usuario.solicitudesEnviadas) {
            datos += "Emisor: " + solicitud->correoEmisor + " -> Receptor: " + solicitud->correoReceptor + ", Estado: " + solicitud->estado + "\n";
        }
    }

    datos += "\nSolicitudes Recibidas:\n";
    for (const Usuario& usuario : usuarios) {
        for (const Solicitud* solicitud : usuario.solicitudesRecibidas) {
            datos += "Receptor: " + solicitud->correoReceptor + " <- Emisor: " + solicitud->correoEmisor + ", Estado: " + solicitud->estado + "\n";
        }
    }

    return datos;
}

void vUser::on_pushButton_clicked()
{



    QString datos = prepararDatosParaCompresion(grafo);
    huffman.compress(datos, "backup.edd");
    std::cout << "Datos comprimidos correctamente al cerrar sesión." << std::endl;

    MainWindow *loginWindow = new MainWindow();
    loginWindow->show();

    this->close();
}


void vUser::on_pushButton_2_clicked()
{

    QString correo = ui->lineEdit->text();

    NodoAVL* usuario = arbol.buscar(correo);

    // Verificar si el usuario fue encontrado
    if (usuario != nullptr) {
        ui->lineEdit_2->setText(usuario->usuario.nombres);
        ui->lineEdit_3->setText(usuario->usuario.apellidos);
        ui->lineEdit_4->setText(usuario->usuario.correo);
        ui->lineEdit_5->setText(usuario->usuario.fechaNacimiento);
    } else {
        QMessageBox::warning(this, "Usuario no encontrado", "No se encontró un usuario con el correo proporcionado.");
    }
}


void vUser::on_pushButton_3_clicked() {
    QString nuevosNombres = ui->lineEdit_6->text();
    QString nuevosApellidos = ui->lineEdit_7->text();
    QString nuevoCorreo = ui->lineEdit_8->text();
    QString nuevaContra = ui->lineEdit_9->text();
    QString nuevaFechaNacimiento = ui->dateEdit->date().toString("dd/MM/yyyy");

    // Hash the new password
    std::string contrasenaOriginal = nuevaContra.toStdString();
    std::string contrasenaHashed = sha256(contrasenaOriginal);
    QString nuevaContraHashed = QString::fromStdString(contrasenaHashed);

    // Verify if the email has changed
    if (nuevoCorreo != usuarioActual.correo) {
        // Remove the old node from the AVL tree
        arbol.eliminar(usuarioActual.correo);

        // Update the logged-in user's data
        usuarioActual.nombres = nuevosNombres;
        usuarioActual.apellidos = nuevosApellidos;
        usuarioActual.correo = nuevoCorreo;
        usuarioActual.contrasena = nuevaContraHashed;
        usuarioActual.fechaNacimiento = nuevaFechaNacimiento;

        // Insert the updated user back into the AVL tree
        arbol.insertar(usuarioActual);
    } else {
        // If the email hasn't changed, simply update the data in the existing node
        NodoAVL* nodoUsuario = arbol.buscar(usuarioActual.correo);
        if (nodoUsuario != nullptr) {
            nodoUsuario->usuario.nombres = nuevosNombres;
            nodoUsuario->usuario.apellidos = nuevosApellidos;
            nodoUsuario->usuario.contrasena = nuevaContraHashed;
            nodoUsuario->usuario.fechaNacimiento = nuevaFechaNacimiento;
        }
    }

    QMessageBox::information(this, "Modificación exitosa", "Los datos del usuario han sido actualizados.");
}


void vUser::llenarTablaSolicitudes() {
    ui->tableWidget_2->setRowCount(0);

    NodoAVL* nodoUsuario = arbol.buscar(usuarioActual.correo);

    if (!nodoUsuario) {
        QMessageBox::warning(this, "Error", "Usuario logeado no encontrado.");
        return;
    }

    // Recorrer las solicitudes recibidas del usuario
    for (Solicitud* solicitud : nodoUsuario->usuario.solicitudesRecibidas) {
        // Insertar una nueva fila en la tabla
        int row = ui->tableWidget_2->rowCount();
        ui->tableWidget_2->insertRow(row);

        // Colocar el correo en la primera columna
        QTableWidgetItem* itemCorreo = new QTableWidgetItem(solicitud->correoEmisor);
        ui->tableWidget_2->setItem(row, 0, itemCorreo);

        // Crear botones para aceptar y rechazar
        QPushButton* btnAceptar = new QPushButton("Aceptar");
        QPushButton* btnRechazar = new QPushButton("Rechazar");

        // Conectar los botones
        connect(btnAceptar, &QPushButton::clicked, [this, solicitud, nodoUsuario]() {
            // Aceptar la solicitud y actualizar la tabla
            arbol.aceptarSolicitud(solicitud->correoEmisor, solicitud->correoReceptor, grafo);

            // Eliminar la solicitud aceptada de la lista del usuario
            nodoUsuario->usuario.solicitudesRecibidas.removeOne(solicitud);

            // Actualizar la tabla
            llenarTablaSolicitudes();

        });

        connect(btnRechazar, &QPushButton::clicked, [this, solicitud, nodoUsuario]() {
            // Rechazar la solicitud y actualizar la tabla
            arbol.rechazarSolicitud(solicitud->correoEmisor, solicitud->correoReceptor);

            nodoUsuario->usuario.solicitudesRecibidas.removeOne(solicitud);

            llenarTablaSolicitudes();
        });

        QWidget* widgetAcciones = new QWidget();
        QHBoxLayout* layoutAcciones = new QHBoxLayout(widgetAcciones);
        layoutAcciones->addWidget(btnAceptar);
        layoutAcciones->addWidget(btnRechazar);
        layoutAcciones->setContentsMargins(0, 0, 0, 0); // Para ajustar los márgenes

        ui->tableWidget_2->setCellWidget(row, 1, widgetAcciones);
    }
}


void vUser::on_tableWidget_2_cellActivated(int row, int column)
{

}

bool solicitudYaEnviada(const QList<Solicitud*>& solicitudes, const QString& correoReceptor) {
    for (Solicitud* solicitud : solicitudes) {
        if (solicitud->correoReceptor == correoReceptor) {
            return true;
        }
    }
    return false;
}

void vUser::llenarTablaDesdeArbol(NodoAVL* nodo) {
    if (!nodo) return;

    llenarTablaDesdeArbol(nodo->izquierdo);

    if (solicitudYaEnviada(usuarioActual.solicitudesEnviadas, nodo->usuario.correo)) {
        // Si ya se envió una solicitud a este usuario, no lo muestra en la tabla
        llenarTablaDesdeArbol(nodo->derecho);
        return;
    }

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QTableWidgetItem* itemNombres = new QTableWidgetItem(nodo->usuario.nombres);
    QTableWidgetItem* itemApellidos = new QTableWidgetItem(nodo->usuario.apellidos);
    QTableWidgetItem* itemCorreo = new QTableWidgetItem(nodo->usuario.correo);
    QTableWidgetItem* itemFechaNacimiento = new QTableWidgetItem(nodo->usuario.fechaNacimiento);

    ui->tableWidget->setItem(row, 0, itemNombres);
    ui->tableWidget->setItem(row, 1, itemApellidos);
    ui->tableWidget->setItem(row, 2, itemCorreo);
    ui->tableWidget->setItem(row, 3, itemFechaNacimiento);

    QPushButton* btnEnviarSolicitud = new QPushButton("Enviar Solicitud");

    // Conectar el botón a la función para enviar solicitud
    connect(btnEnviarSolicitud, &QPushButton::clicked, [this, nodo]() {
        arbol.enviarSolicitud(usuarioActual.correo, nodo->usuario.correo);
        llenarTablaSolicitudesEnviadas();


    });

    // Añadir el botón a la última columna
    QWidget* widgetAccion = new QWidget();
    QHBoxLayout* layoutAccion = new QHBoxLayout(widgetAccion);
    layoutAccion->addWidget(btnEnviarSolicitud);
    layoutAccion->setContentsMargins(0, 0, 0, 0);
    ui->tableWidget->setCellWidget(row, 4, widgetAccion);

    // Llenar información de los usuarios del subárbol derecho
    llenarTablaDesdeArbol(nodo->derecho);
}

void vUser::on_tableWidget_cellActivated(int row, int column)
{

}


void vUser::llenarTablaSolicitudesEnviadas() {
    ui->tableWidget_3->setRowCount(0);
    NodoAVL* nodoUsuario = arbol.buscar(usuarioActual.correo);
    if (!nodoUsuario) {
        QMessageBox::warning(this, "Error", "Usuario logeado no encontrado.");
        return;
    }

    // Iterar sobre las solicitudes enviadas del usuario logeado
    for (Solicitud* solicitud : nodoUsuario->usuario.solicitudesEnviadas) {
        int row = ui->tableWidget_3->rowCount();
        ui->tableWidget_3->insertRow(row);

        QTableWidgetItem* itemCorreo = new QTableWidgetItem(solicitud->correoReceptor);
        ui->tableWidget_3->setItem(row, 0, itemCorreo);

        QPushButton* btnCancelar = new QPushButton("Cancelar");

        connect(btnCancelar, &QPushButton::clicked, [this, solicitud]() {
            arbol.eliminarSolicitud(solicitud->correoEmisor,solicitud->correoReceptor);
            llenarTablaSolicitudesEnviadas();
            llenarTablaSolicitudes();
        });

        // Añadir el botón a la segunda columna
        QWidget* widgetAccion = new QWidget();
        QHBoxLayout* layoutAccion = new QHBoxLayout(widgetAccion);
        layoutAccion->addWidget(btnCancelar);
        layoutAccion->setContentsMargins(0, 0, 0, 0); // Ajustar márgenes
        ui->tableWidget_3->setCellWidget(row, 1, widgetAccion);

    }
}



void mostrarFechasDesdeArbol(NodoABB* nodo) {
    if (!nodo) return;  // Caso base: si el nodo es nulo, termina la recursión

    // Recorrer el subárbol izquierdo
    mostrarFechasDesdeArbol(nodo->izquierda);

    // Recorrer el subárbol derecho
    mostrarFechasDesdeArbol(nodo->derecha);
}


void eliminarPublicacion(Publicacion* publicacion) {
    QDate fechaPublicacion = publicacion->fechaHora.date();
    NodoABB* nodo = arbolabb.buscar(fechaPublicacion);  // Encuentra el nodo del ABB con la fecha de la publicación

    if (nodo != nullptr) {
        // Eliminar la publicación de la lista enlazada en el nodo correspondiente
        Publicacion* actual = nodo->listaPublicaciones;
        Publicacion* anterior = nullptr;

        while (actual != nullptr) {
            if (actual == publicacion) {
                // Si es la primera publicación en la lista
                if (anterior == nullptr) {
                    nodo->listaPublicaciones = actual->siguiente;
                    if (actual->siguiente != nullptr) {
                        actual->siguiente->anterior = nullptr;
                    }
                } else {
                    anterior->siguiente = actual->siguiente;
                    if (actual->siguiente != nullptr) {
                        actual->siguiente->anterior = anterior;
                    }
                }
                delete actual;  // Liberar memoria
                break;
            }
            anterior = actual;
            actual = actual->siguiente;
        }

        // Si la lista de publicaciones en ese nodo queda vacía, eliminar el nodo del ABB
        if (nodo->listaPublicaciones == nullptr) {
            arbolabb.raiz = arbolabb.eliminarNodo(arbolabb.raiz, fechaPublicacion);
        }
    }
}


void vUser::mostrarPublicacionesEnScrollArea(QScrollArea* scrollArea, const QList<Publicacion*>& publicaciones) {
    QWidget* contenedor = new QWidget();
    QVBoxLayout* layoutContenedor = new QVBoxLayout(contenedor);


    for (Publicacion* publicacion : publicaciones) {
        QGroupBox* groupBox = new QGroupBox();
        QVBoxLayout* layoutGroupBox = new QVBoxLayout(groupBox);


        QLabel* labelUsuario = new QLabel(publicacion->correoUsuario);
        layoutGroupBox->addWidget(labelUsuario);

        mostrarFechasDesdeArbol(arbolabb.obtenerRaiz());

        QLabel* labelFecha = new QLabel("Fecha: " + publicacion->fechaHora.toString("dd/MM/yyyy hh:mm"));
        layoutGroupBox->addWidget(labelFecha);

        QLabel* labelTexto = new QLabel(publicacion->contenido);
        layoutGroupBox->addWidget(labelTexto);

        if (!publicacion->rutaImagen.isEmpty()) {
            QLabel* labelImagen = new QLabel();
            QPixmap imagen(publicacion->rutaImagen);
            labelImagen->setPixmap(imagen.scaled(200, 200, Qt::KeepAspectRatio));
            layoutGroupBox->addWidget(labelImagen);
        }

        QHBoxLayout* layoutBotones = new QHBoxLayout();
        QPushButton* btnComentar = new QPushButton("Comentar");
        QPushButton* btnVerComentarios = new QPushButton("Ver comentarios");
        QPushButton* btnVerArbolComentarios = new QPushButton("Ver árbol de comentarios");
        QPushButton* btnEliminar = new QPushButton("Eliminar");

        // Conectar el botón de comentar
        connect(btnComentar, &QPushButton::clicked, [this, publicacion]() {
            Comentar dialogoComentario;
            if (dialogoComentario.exec() == QDialog::Accepted) {
                QString comentario = dialogoComentario.getComentario();
                if (!comentario.isEmpty()) {
                    QDateTime fechaHoraComentario = QDateTime::currentDateTime(); // Usa la fecha y hora actual para el comentario
                    publicacion->arbolComentarios.insertarComentario(usuarioActual.correo, comentario, fechaHoraComentario);
                    QMessageBox::information(this, "Comentario agregado", "Tu comentario ha sido agregado correctamente.");
                }
            }
        });

        connect(btnVerComentarios, &QPushButton::clicked, [this, publicacion]() {
            VerComentarios VerComentarios(&publicacion->arbolComentarios, this);
            VerComentarios.exec();
        });

        // Conectar el botón de ver árbol de comentarios
        connect(btnVerArbolComentarios, &QPushButton::clicked, [this, publicacion]() {
            // Graficar el árbol de comentarios
            QString filename = QString("arbol_comentarios_%1").arg(publicacion->fechaHora.toString("yyyyMMdd_hhmmss"));
            arbolb.graficarArbolB(&publicacion->arbolComentarios, filename);

            // Mostrar la imagen generada en una nueva pestaña
            QLabel* labelImagen = new QLabel();
            QPixmap imagen(filename + ".png");
            labelImagen->setPixmap(imagen.scaled(800, 600, Qt::KeepAspectRatio));

            QScrollArea* scrollAreaImagen = new QScrollArea();
            scrollAreaImagen->setWidget(labelImagen);
            scrollAreaImagen->setWindowTitle("Árbol de Comentarios");
            scrollAreaImagen->show();
        });


        // Conectar el botón de eliminar
        connect(btnEliminar, &QPushButton::clicked, [this, publicacion]() {
            // Confirmación de eliminación
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Eliminar Publicación", "¿Estás seguro de eliminar esta publicación?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                // Eliminar la publicación del árbol
                eliminarPublicacion(publicacion);

                QMessageBox::information(this, "Publicación Eliminada", "La publicación ha sido eliminada correctamente.");

                // Actualizar el feed de publicaciones en el QScrollArea
                mostrarPublicacionesUsuario(ui->scrollArea, &usuarioActual, arbolabb);
            }
        });


        layoutBotones->addWidget(btnComentar);
        layoutBotones->addWidget(btnVerComentarios);
        layoutBotones->addWidget(btnVerArbolComentarios);
        layoutGroupBox->addLayout(layoutBotones);
        layoutBotones->addWidget(btnEliminar);

        layoutContenedor->addWidget(groupBox);
    }

    contenedor->setLayout(layoutContenedor);
    contenedor->adjustSize();
    scrollArea->setWidget(contenedor);
    scrollArea->setWidgetResizable(true);
}


void vUser::mostrarPublicacionesUsuario(QScrollArea* scrollArea, Usuario* usuarioLogeado, ArbolABB& arbolPublicaciones) {
    QList<Publicacion*> listaPublicaciones;

    // Extraer publicaciones desde el árbol
    extraerPublicaciones(arbolPublicaciones, listaPublicaciones);

    QList<Publicacion*> publicacionesParaMostrar;

    // Filtrar las publicaciones propias y las de los amigos
    for (Publicacion* publicacion : listaPublicaciones) {
        if (publicacion->correoUsuario == usuarioLogeado->correo ||  grafo.sonAmigos(usuarioLogeado->correo ,publicacion->correoUsuario)) {
            publicacionesParaMostrar.append(publicacion);
        }
    }

    // Llamar a la función para mostrar las publicaciones en el QScrollArea
    mostrarPublicacionesEnScrollArea(scrollArea, publicacionesParaMostrar);
}

void vUser::extraerPublicaciones(ArbolABB& arbolPublicaciones, QList<Publicacion*>& listaPublicaciones) {
    std::function<void(NodoABB*)> agregarPublicaciones = [&](NodoABB* nodo) {
        if (nodo) {
            Publicacion* actual = nodo->listaPublicaciones;
            while (actual) {
                listaPublicaciones.append(actual);  // Asegúrate de que `actual` tiene fecha y hora correctas
                actual = actual->siguiente;
            }
            agregarPublicaciones(nodo->izquierda);
            agregarPublicaciones(nodo->derecha);
        }
    };
    agregarPublicaciones(arbolPublicaciones.obtenerRaiz());
}


void vUser::agregarNuevaPublicacion() {
    NuevaPublicacion dialogo(this);

    if (dialogo.exec() == QDialog::Accepted) {
        QString texto = dialogo.getTextoPublicacion();
        QString rutaImagen = dialogo.getRutaImagen();

        if (!texto.isEmpty()) {
            try {
                // Crear un QDateTime a partir de la fecha actual
                QDateTime fechaHora = QDateTime::currentDateTime();

                // Insertar la publicación en el árbol ABB y la lista doblemente enlazada de publicaciones
                arbolabb.insertarPublicacion(usuarioActual.correo, texto, rutaImagen, fechaHora.date());
                lista.agregarPublicacion(usuarioActual.correo, texto, rutaImagen, fechaHora);

                // Actualizar el feed de publicaciones en el QScrollArea
                mostrarPublicacionesUsuario(ui->scrollArea, &usuarioActual, arbolabb);

                // Crear el JSON que contiene todas las publicaciones
                json publicacionesJson;
                Publicacion* actual = lista.getCabeza();
                while (actual != nullptr) {
                    try {
                        std::string publicacionJson = actual->toJson();
                        publicacionesJson.push_back(json::parse(publicacionJson));
                    } catch (const std::exception& e) {
                        qDebug() << "Error al procesar publicación: " << e.what();
                        return;
                    }
                    actual = actual->siguiente;
                }

                // Serializar el JSON de todas las publicaciones
                std::string dataJson = publicacionesJson.dump();

                // Crear un árbol de Merkle y agregar el JSON de las publicaciones
                MerkleTree merkleTree;
                try {
                    merkleTree.insert(dataJson);  // Usar el JSON completo para generar el Merkle Tree
                } catch (const std::exception& e) {
                    qDebug() << "Error al insertar en el Merkle Tree: " << e.what();
                    return;
                }

                std::string rootMerkle = merkleTree.getRootHash();  // Obtener el hash del Merkle Tree

                // Agregar un nuevo bloque al Blockchain con el JSON de las publicaciones y el rootMerkle
                try {
                    blockchain.insert(dataJson);  // Pasar el JSON de las publicaciones como dataJson
                } catch (const std::exception& e) {
                    qDebug() << "Error al insertar bloque en el blockchain: " << e.what();
                    return;
                }

                // (Opcional) Imprimir detalles del bloque añadido para fines de depuración
                blockchain.print();

            } catch (const std::exception& e) {
                qDebug() << "Error general en el proceso de agregar publicación: " << e.what();
            }
        } else {
            qDebug() << "Error: El texto de la publicación está vacío.";
        }
    }
}


void vUser::poblarComboBoxFechas() {
    ui->comboBox->clear();
    QList<QDate> fechas;
    obtenerFechasDesdeArbol(arbolabb.obtenerRaiz(), fechas, usuarioActual);

    for (const QDate& fecha : fechas) {
        ui->comboBox->addItem(fecha.toString("dd/MM/yyyy"));
    }
}

void vUser::poblarComboBoxABB() {
    ui->comboBox_3->clear();
    QList<QDate> fechas;
    obtenerFechasDesdeArbol(arbolabb.obtenerRaiz(), fechas, usuarioActual);

    for (const QDate& fecha : fechas) {
        ui->comboBox_3->addItem(fecha.toString("dd/MM/yyyy"));
    }
}



void vUser::graficarArbolABB(NodoABB* nodo) {
    std::ofstream archivo("arbol_publicaciones.dot");

    if (!archivo.is_open()) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo DOT para graficar el árbol.");
        return;
    }

    archivo << "digraph G {" << std::endl;
    archivo << "node [shape=ellipse];" << std::endl;

    std::function<void(NodoABB*)> graficarNodo = [&](NodoABB* nodo) {
        if (!nodo) return;

        // Graficar la fecha del nodo y sus publicaciones
        std::string nodoID = "nodo" + nodo->fecha.toString("yyyyMMdd").toStdString();
        archivo << nodoID << " [label=\"Fecha: " << nodo->fecha.toString("dd/MM/yyyy").toStdString() << "\"];" << std::endl;

        // Graficar publicaciones de la fecha seleccionada
        Publicacion* publicacion = nodo->listaPublicaciones;
        int contador = 0;
        while (publicacion) {
            std::string pubID = nodoID + "_pub" + std::to_string(contador);
            archivo << pubID << " [label=\"{" << publicacion->correoUsuario.toStdString()
                    << " | " << publicacion->contenido.toStdString() << "}\"];" << std::endl;
            archivo << nodoID << " -> " << pubID << ";" << std::endl;
            publicacion = publicacion->siguiente;
            contador++;
        }

        // Recursivamente graficar los nodos izquierdo y derecho
        if (nodo->izquierda) {
            graficarNodo(nodo->izquierda);
            archivo << nodoID << " -> " << "nodo" + nodo->izquierda->fecha.toString("yyyyMMdd").toStdString() << ";" << std::endl;
        }
        if (nodo->derecha) {
            graficarNodo(nodo->derecha);
            archivo << nodoID << " -> " << "nodo" + nodo->derecha->fecha.toString("yyyyMMdd").toStdString() << ";" << std::endl;
        }
    };

    graficarNodo(nodo);

    archivo << "}" << std::endl;
    archivo.close();

    // Generar la imagen usando Graphviz
    QProcess proceso;
    proceso.start("dot", QStringList() << "-Tpng" << "arbol_publicaciones.dot" << "-o" << "arbol_publicaciones.png");
    proceso.waitForFinished();

    if (proceso.exitStatus() != QProcess::NormalExit || proceso.exitCode() != 0) {
        QMessageBox::critical(this, "Error", "No se pudo generar la imagen del árbol.");
    }
}



void vUser::obtenerFechasDesdeArbol(NodoABB* nodo, QList<QDate>& fechas, Usuario& usuarioLogeado) {
    if (!nodo) return;

    obtenerFechasDesdeArbol(nodo->izquierda, fechas, usuarioLogeado);

    // Verificar si este nodo tiene publicaciones del usuario o sus amigos
    Publicacion* actual = nodo->listaPublicaciones;
    bool tienePublicacionRelevante = false;

    while (actual) {
        if (actual->correoUsuario == usuarioLogeado.correo ||  grafo.sonAmigos(usuarioLogeado.correo ,actual->correoUsuario) ) {
            tienePublicacionRelevante = true;
            break; // No es necesario seguir buscando en este nodo
        }
        actual = actual->siguiente;
    }

    // Si hay al menos una publicación relevante, agregamos la fecha
    if (tienePublicacionRelevante) {
        fechas.append(nodo->fecha);
    }

    obtenerFechasDesdeArbol(nodo->derecha, fechas, usuarioLogeado);
}


void vUser::mostrarPublicacionesPorFecha() {
    QString fechaSeleccionada = ui->comboBox->currentText();

    // Convertir la fecha seleccionada a QDate
    QDate fecha = QDate::fromString(fechaSeleccionada, "dd/MM/yyyy");

    // Buscar el nodo correspondiente a la fecha en el árbol
    NodoABB* nodo = arbolabb.buscar(fecha);
    if (!nodo) {
        QMessageBox::warning(this, "Fecha no encontrada", "No se encontraron publicaciones para la fecha seleccionada.");
        return;
    }

    // Filtrar y mostrar solo las publicaciones de la fecha seleccionada
    QList<Publicacion*> publicacionesParaMostrar;
    Publicacion* actual = nodo->listaPublicaciones;
    while (actual) {
        publicacionesParaMostrar.append(actual);
        actual = actual->siguiente;
    }

    mostrarPublicacionesEnScrollArea(ui->scrollArea, publicacionesParaMostrar);
}

void vUser::on_pushButton_6_clicked()
{

}


void vUser::on_pushButton_7_clicked()
{
    QString ordenSeleccionado = ui->comboBox_2->currentText();
    int cantidad = ui->lineEdit_10->text().toInt();
    std::vector<Publicacion*> publicaciones;

    if (ordenSeleccionado == "PreOrden") {
        arbolabb.preOrden(arbolabb.obtenerRaiz(), publicaciones);
    } else if (ordenSeleccionado == "PostOrden") {
        arbolabb.postOrden(arbolabb.obtenerRaiz(), publicaciones);
    } else if (ordenSeleccionado == "InOrden") {
        arbolabb.inOrden(arbolabb.obtenerRaiz(), publicaciones);
    }

    mostrarPublicacionesEnScrollArea2(ui->scrollArea,publicaciones, cantidad);
}

void vUser::mostrarPublicacionesEnScrollArea2(QScrollArea* scrollArea, const std::vector<Publicacion*>& publicaciones, int limite) {
    QWidget* contenedor = new QWidget();
    QVBoxLayout* layoutContenedor = new QVBoxLayout(contenedor);

    int mostrar = std::min(limite, static_cast<int>(publicaciones.size()));

    for (int i = 0; i < mostrar; ++i) {
        Publicacion* publicacion = publicaciones[i];

        QGroupBox* groupBox = new QGroupBox();
        QVBoxLayout* layoutGroupBox = new QVBoxLayout(groupBox);

        QLabel* labelUsuario = new QLabel("Usuario: " + publicacion->correoUsuario);
        layoutGroupBox->addWidget(labelUsuario);

        QLabel* labelFecha = new QLabel("Fecha: " + publicacion->fechaHora.toString("dd/MM/yyyy hh:mm"));
        layoutGroupBox->addWidget(labelFecha);

        QLabel* labelTexto = new QLabel(publicacion->contenido);
        layoutGroupBox->addWidget(labelTexto);

        if (!publicacion->rutaImagen.isEmpty()) {
            QLabel* labelImagen = new QLabel();
            QPixmap imagen(publicacion->rutaImagen);
            labelImagen->setPixmap(imagen.scaled(200, 200, Qt::KeepAspectRatio));  // Escalar la imagen
            layoutGroupBox->addWidget(labelImagen);
        }

        QHBoxLayout* layoutBotones = new QHBoxLayout();
        QPushButton* btnComentar = new QPushButton("Comentar");
        QPushButton* btnVerComentarios = new QPushButton("Ver comentarios");
        QPushButton* btnVerArbolComentarios = new QPushButton("Ver árbol de comentarios");

        layoutBotones->addWidget(btnComentar);
        layoutBotones->addWidget(btnVerComentarios);
        layoutBotones->addWidget(btnVerArbolComentarios);

        layoutGroupBox->addLayout(layoutBotones);

        layoutContenedor->addWidget(groupBox);
    }

    contenedor->setLayout(layoutContenedor);
    contenedor->adjustSize();

    scrollArea->setWidget(contenedor);
    scrollArea->setWidgetResizable(true);
}

void vUser::llenarTablaFechasYPublicaciones(QTableWidget* tabla, ArbolABB& arbolPublicaciones) {

    tabla->setRowCount(0);
    tabla->setColumnCount(2);

    QStringList headers;
    headers << "Fecha" << "Cantidad de Publicaciones";
    tabla->setHorizontalHeaderLabels(headers);

    // Vector para almacenar fechas y cantidades de publicaciones
    std::vector<std::pair<QDate, int>> fechasPublicaciones;

    // Función lambda para recorrer el árbol y llenar el vector
    std::function<void(NodoABB*)> recorrerArbol = [&](NodoABB* nodo) {
        if (!nodo) return;

        // Recorrer el subárbol izquierdo
        recorrerArbol(nodo->izquierda);

        // Contar las publicaciones en la lista del nodo actual
        int cantidadPublicaciones = 0;
        Publicacion* actual = nodo->listaPublicaciones;
        while (actual) {
            cantidadPublicaciones++;
            actual = actual->siguiente;
        }

        // Almacenar la fecha y la cantidad de publicaciones en el vector
        fechasPublicaciones.push_back(std::make_pair(nodo->fecha, cantidadPublicaciones));

        // Recorrer el subárbol derecho
        recorrerArbol(nodo->derecha);
    };

    // Llamar a la función para recorrer desde la raíz
    recorrerArbol(arbolPublicaciones.obtenerRaiz());

    // Ordenar las fechas por la cantidad de publicaciones en orden descendente
    std::sort(fechasPublicaciones.begin(), fechasPublicaciones.end(),
              [](const std::pair<QDate, int>& a, const std::pair<QDate, int>& b) {
                  return a.second > b.second; // Ordenar por cantidad de publicaciones, descendente
              });

    // Insertar las primeras 3 fechas con más publicaciones en la tabla
    for (int i = 0; i < std::min(3, static_cast<int>(fechasPublicaciones.size())); ++i) {
        const auto& [fecha, cantidad] = fechasPublicaciones[i];
        int fila = tabla->rowCount();
        tabla->insertRow(fila);
        tabla->setItem(fila, 0, new QTableWidgetItem(fecha.toString("dd/MM/yyyy")));
        tabla->setItem(fila, 1, new QTableWidgetItem(QString::number(cantidad)));
    }
}


namespace std {
template <>
struct hash<QDate> {
    std::size_t operator()(const QDate& date) const noexcept {
        // Usar año, mes y día para calcular el hash
        return std::hash<int>()(date.year()) ^ std::hash<int>()(date.month()) << 1 ^ std::hash<int>()(date.day()) << 2;
    }
};
}

struct PairHash {
    std::size_t operator()(const std::pair<QDate, QString>& pair) const {
        return std::hash<QString>()(pair.second) ^ std::hash<int>()(pair.first.toJulianDay());
    }
};


void vUser::llenarTablaPublicacionesConComentarios(QTableWidget* tabla, ArbolABB& arbolPublicaciones) {
    tabla->setRowCount(0);
    tabla->setColumnCount(3);

    QStringList headers;
    headers << "Fecha" << "Usuario" << "Cantidad de Comentarios";
    tabla->setHorizontalHeaderLabels(headers);

    std::unordered_map<QDate, Publicacion*> publicacionConMasComentariosPorFecha;

    std::function<void(NodoABB*)> recorrerArbol = [&](NodoABB* nodo) {
        if (!nodo) return;

        // Recorrer el subárbol izquierdo
        recorrerArbol(nodo->izquierda);

        // Procesar publicaciones en el nodo actual
        Publicacion* actual = nodo->listaPublicaciones;
        while (actual) {
            int cantidadComentarios = 0;

            // Contar comentarios en la publicación actual
            actual->arbolComentarios.recorrerComentarios([&](const Comentario&) {
                cantidadComentarios++;
            });

            // Si la fecha ya está en el mapa, comparar el número de comentarios
            if (publicacionConMasComentariosPorFecha.count(nodo->fecha)) {
                Publicacion* publicacionExistente = publicacionConMasComentariosPorFecha[nodo->fecha];
                int cantidadComentariosExistente = 0;

                publicacionExistente->arbolComentarios.recorrerComentarios([&](const Comentario&) {
                    cantidadComentariosExistente++;
                });
                if (cantidadComentarios > cantidadComentariosExistente) {
                    publicacionConMasComentariosPorFecha[nodo->fecha] = actual;
                }
            } else {
                publicacionConMasComentariosPorFecha[nodo->fecha] = actual;
            }

            actual = actual->siguiente;
        }

        // Recorrer el subárbol derecho
        recorrerArbol(nodo->derecha);
    };

    // Llamar a la función para recorrer desde la raíz
    recorrerArbol(arbolPublicaciones.obtenerRaiz());

    // Vector de pares para ordenar por la cantidad de comentarios por fecha
    std::vector<std::pair<QDate, int>> fechasConComentarios;
    for (const auto& [fecha, publicacion] : publicacionConMasComentariosPorFecha) {
        int cantidadComentarios = 0;

        // Contar los comentarios en la publicación con más comentarios para esa fecha
        publicacion->arbolComentarios.recorrerComentarios([&](const Comentario&) {
            cantidadComentarios++;
        });

        fechasConComentarios.emplace_back(fecha, cantidadComentarios);
    }

    // Ordenar por la cantidad de comentarios en orden descendente
    std::sort(fechasConComentarios.begin(), fechasConComentarios.end(),
              [](const std::pair<QDate, int>& a, const std::pair<QDate, int>& b) {
                  return a.second > b.second; // Ordenar por cantidad de comentarios, descendente
              });

    // Mantener solo las 3 fechas con más comentarios
    if (fechasConComentarios.size() > 3) {
        fechasConComentarios.resize(3);
    }

    // Conjunto de las fechas seleccionadas
    std::unordered_set<QDate> fechasSeleccionadas;
    for (const auto& [fecha, _] : fechasConComentarios) {
        fechasSeleccionadas.insert(fecha);
    }

    // Mapa para contar los comentarios por usuario y fecha seleccionada
    std::unordered_map<std::pair<QDate, QString>, int, PairHash> conteoComentariosPorUsuario;

    // Filtrar y contar los comentarios de las publicaciones seleccionadas
    for (const auto& [fecha, publicacion] : publicacionConMasComentariosPorFecha) {
        if (fechasSeleccionadas.count(fecha)) {
            publicacion->arbolComentarios.recorrerComentarios([&](const Comentario& comentario) {
                conteoComentariosPorUsuario[std::make_pair(fecha, comentario.correoUsuario)]++;
            });
        }
    }

    // Insertar en la tabla los datos agrupados por usuario
    for (const auto& [fechaUsuario, cantidad] : conteoComentariosPorUsuario) {
        const auto& [fecha, usuario] = fechaUsuario;
        int fila = tabla->rowCount();
        tabla->insertRow(fila);
        tabla->setItem(fila, 0, new QTableWidgetItem(fecha.toString("dd/MM/yyyy")));
        tabla->setItem(fila, 1, new QTableWidgetItem(usuario));
        tabla->setItem(fila, 2, new QTableWidgetItem(QString::number(cantidad)));
    }
}

void vUser::on_pushButton_8_clicked()
{
    llenarTablaFechasYPublicaciones(ui->tableWidget_4,arbolabb);
    llenarTablaPublicacionesConComentarios(ui->tableWidget_5,arbolabb);

}



void vUser::mostrarImagenEnGroupBox(QGroupBox* groupBox, const QString& rutaImagen) {
    // Limpiar cualquier layout existente en el groupBox
    QLayout* layoutExistente = groupBox->layout();
    if (layoutExistente) {
        QLayoutItem* item;
        while ((item = layoutExistente->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layoutExistente;
    }

    QLabel* label = new QLabel(groupBox);
    QPixmap pixmap(rutaImagen);

    if (!pixmap.isNull()) {
        label->setPixmap(pixmap);
        label->setScaledContents(true);

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(label);
        groupBox->setLayout(layout);
    } else {
        qDebug() << "No se pudo cargar la imagen en el QGroupBox.";
        QLabel* errorLabel = new QLabel("No se pudo cargar la imagen.", groupBox);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(errorLabel);
        groupBox->setLayout(layout);
    }
}



void graficarArbolPublicaciones(ArbolABB& arbol, const QDate& fechaSeleccionada, const QString& outputFile) {
    std::ofstream archivo(outputFile.toStdString());

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
        return;
    }

    archivo << "digraph G {" << std::endl;
    archivo << "    node [shape=ellipse, style=filled, fillcolor=white];" << std::endl;

    std::function<void(NodoABB*)> graficarNodo = [&](NodoABB* nodo) {
        if (!nodo) return;

        // Graficar el nodo de la fecha
        archivo << "    \"" << nodo->fecha.toString("dd/MM/yyyy").toStdString() << "\"";

        // Si la fecha es la seleccionada, cambiar el color y agregar publicaciones alineadas verticalmente
        if (nodo->fecha == fechaSeleccionada) {
            archivo << " [fillcolor=green, style=filled];" << std::endl;

            Publicacion* actual = nodo->listaPublicaciones;
            int pubIndex = 0;
            std::string prevNode;

            // Agregar publicaciones una debajo de otra sin usar un subgrafo
            while (actual != nullptr) {
                std::string pubNode = nodo->fecha.toString("dd/MM/yyyy").toStdString() + "_pub" + std::to_string(pubIndex);
                archivo << "    \"" << pubNode << "\" [label=\"Publicacion\\nFecha y hora: "
                        << actual->fechaHora.toString("dd/MM/yyyy hh:mm").toStdString()
                        << "\\nCorreo: " << actual->correoUsuario.toStdString()
                        << "\\nContenido: " << actual->contenido.toStdString() << "\", shape=box];" << std::endl;

                if (!prevNode.empty()) {
                    // Conectar las publicaciones verticalmente sin un cuadro alrededor
                    archivo << "    \"" << prevNode << "\" -> \"" << pubNode << "\" [dir=none];" << std::endl;
                }
                prevNode = pubNode;

                actual = actual->siguiente;
                pubIndex++;
            }

            // Conectar el nodo de fecha con la primera publicación
            if (!prevNode.empty()) {
                archivo << "    \"" << nodo->fecha.toString("dd/MM/yyyy").toStdString()
                << "\" -> \"" << nodo->fecha.toString("dd/MM/yyyy").toStdString() << "_pub0\";" << std::endl;
            }

        } else {
            archivo << ";" << std::endl;
        }

        // Graficar subárboles izquierdo y derecho
        if (nodo->izquierda) {
            archivo << "    \"" << nodo->fecha.toString("dd/MM/yyyy").toStdString() << "\" -> \""
                    << nodo->izquierda->fecha.toString("dd/MM/yyyy").toStdString() << "\";" << std::endl;
            graficarNodo(nodo->izquierda);
        }

        if (nodo->derecha) {
            archivo << "    \"" << nodo->fecha.toString("dd/MM/yyyy").toStdString() << "\" -> \""
                    << nodo->derecha->fecha.toString("dd/MM/yyyy").toStdString() << "\";" << std::endl;
            graficarNodo(nodo->derecha);
        }
    };

    graficarNodo(arbol.obtenerRaiz());
    archivo << "}" << std::endl;
    archivo.close();

    std::cout << "Archivo DOT generado: " << outputFile.toStdString() << std::endl;
}

void vUser::on_pushButton_9_clicked()
{
    QDate  fechaSeleccionada = QDate::fromString(ui->comboBox_3->currentText(), "dd/MM/yyyy");
    if (!fechaSeleccionada.isValid()) {
        QMessageBox::warning(this, "Error", "Fecha no válida seleccionada.");
        return;
    }
    graficarArbolPublicaciones(arbolabb, fechaSeleccionada, "arbol_publicaciones.dot");

    // Luego, con Graphviz, conviertes el .dot a una imagen
    system("dot -Tpng arbol_publicaciones.dot -o arbol_publicaciones.png");
    mostrarImagenEnGroupBox(ui->groupBox_11, "arbol_publicaciones.png");
}


void vUser::on_pushButton_4_clicked()
{
    arbol.eliminar(usuarioActual.correo);
    QMessageBox::information(this, "Eliminar", "Usuario eliminado correctamente.");
    this->close();
}




void vUser::on_pushButton_10_clicked()
{
    grafo.sugerenciasAmistad(usuarioActual.correo);
}



void vUser::mostrarSugerenciasAmistad(const QString& usuario) {
    // Crear un QTableWidget para mostrar las sugerencias de amistad
    QTableWidget* tableWidget = new QTableWidget(ui->groupBox_12);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Usuario Sugerido" << "Amigos en Común");

    // Obtener las sugerencias de amistad del usuario
    QList<QPair<QString, int>> sugerencias = grafo.sugerenciasAmistad(usuario);

    // Establecer el número de filas de la tabla
    tableWidget->setRowCount(sugerencias.size());

    // Rellenar la tabla con las sugerencias de amistad
    for (int i = 0; i < sugerencias.size(); ++i) {
        QTableWidgetItem* usuarioItem = new QTableWidgetItem(sugerencias[i].first);
        QTableWidgetItem* amigosEnComunItem = new QTableWidgetItem(QString::number(sugerencias[i].second));

        tableWidget->setItem(i, 0, usuarioItem);
        tableWidget->setItem(i, 1, amigosEnComunItem);
    }

    // Configurar la disposición de la tabla
    tableWidget->setGeometry(ui->groupBox_12->rect()); // Ajustar la tabla al tamaño del groupBox
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // No permitir la edición de celdas
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection); // No permitir la selección
    tableWidget->horizontalHeader()->setStretchLastSection(true); // Ajustar las columnas para que ocupen todo el espacio disponible
    tableWidget->show();
}

