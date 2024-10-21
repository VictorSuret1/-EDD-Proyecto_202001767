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
#include "grafoamistades.h"
#include "adjacencylist.h"
#include "merkletree.h"
#include "blockchain.h"
#include "grafoamistades.h"
#include "sha256.h"

extern GrafoAmistades grafo;
extern Blockchain blockchain;
extern MerkleTree merkletree;
extern AdjacencyList adlist;
using json = nlohmann::json;

extern GrafoAmistades grafo;
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

            // Hash the password using SHA256
            std::string contrasenaOriginal = usuarioData["contraseña"];
            std::string contrasenaHashed = sha256(contrasenaOriginal);
            nuevoUsuario.contrasena = QString::fromStdString(contrasenaHashed);

            arbol.insertar(nuevoUsuario);
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

                // Convertir los correos de std::string a QString antes de usarlos
                QString emisorQString = QString::fromStdString(correoEmisor);
                QString receptorQString = QString::fromStdString(correoReceptor);

                matrizAmistades.insertarAmistad(correoEmisor, correoReceptor);
                grafo.agregarAmistad(emisorQString,receptorQString);


            }
        } else {
            QMessageBox::warning(nullptr, "Error", "JSON malformado. Faltan campos requeridos para un usuario.");
        }
    }
    // Mostrar el grafo después de cargar las solicitudes
    grafo.mostrarGrafo();
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

    // Lista de todas las publicaciones cargadas para el árbol de Merkle
    std::vector<std::string> listaPublicacionesJson;

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
            if (!fechaHora.isValid()) {
                std::cout << "Error: Fecha y hora inválidas en la publicación.\n";
                continue;
            }

            // Insertar publicación en ABB y lista
            arbolabb.insertarPublicacion(correo, contenido, rutaImagen, fechaHora.date());
            lista.agregarPublicacion(correo, contenido, rutaImagen, fechaHora);

            // Agregar publicación a la lista para generar árbol de Merkle más tarde
            json publicacionJson;
            publicacionJson["correo"] = publicacionData["correo"];
            publicacionJson["contenido"] = publicacionData["contenido"];
            publicacionJson["fecha"] = publicacionData["fecha"];
            publicacionJson["hora"] = publicacionData["hora"];
            if (publicacionData.contains("imagen")) {
                publicacionJson["imagen"] = publicacionData["imagen"];
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

                        // Insertar comentario en la publicación
                        NodoABB* nodoPublicacion = arbolabb.buscar(fechaHora.date());
                        if (nodoPublicacion) {
                            Publicacion* actualPublicacion = nodoPublicacion->listaPublicaciones;
                            while (actualPublicacion && actualPublicacion->contenido != contenido) {
                                actualPublicacion = actualPublicacion->siguiente;
                            }
                            if (actualPublicacion) {
                                actualPublicacion->arbolComentarios.insertarComentario(correoComentario, textoComentario, fechaHoraComentario);
                            }
                        }

                        // Agregar comentario al JSON de la publicación
                        json comentarioJson;
                        comentarioJson["correo"] = comentarioData["correo"];
                        comentarioJson["comentario"] = comentarioData["comentario"];
                        comentarioJson["fecha"] = comentarioData["fecha"];
                        comentarioJson["hora"] = comentarioData["hora"];

                        publicacionJson["comentarios"].push_back(comentarioJson);
                    }
                }
            }

            // Convertir publicación completa a JSON
            listaPublicacionesJson.push_back(publicacionJson.dump());
        } else {
            std::cout << "Error: JSON malformado. Faltan campos requeridos para una publicación.\n";
        }
    }

    // Mensaje de confirmación de carga
    std::cout << "Carga masiva de publicaciones y comentarios completada.\n";

    // Paso 2: Crear el Árbol de Merkle y Insertar en la Blockchain
    if (!listaPublicacionesJson.empty()) {
        MerkleTree merkleTree;
        for (const auto& publicacionJson : listaPublicacionesJson) {
            merkleTree.insert(publicacionJson);
        }

        std::string rootMerkle = merkleTree.getRootHash();

        // Crear un JSON que contenga todas las publicaciones
        json publicacionesCompletasJson = json::array();
        for (const auto& publicacionJson : listaPublicacionesJson) {
            publicacionesCompletasJson.push_back(json::parse(publicacionJson));
        }

        // Insertar en la blockchain el JSON completo de todas las publicaciones
        blockchain.insert(publicacionesCompletasJson.dump());
    }
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


}


void vAdmin::on_pushButton_8_clicked()
{
    grafo.graficarGrafo("amistades_grafo");
    grafo.graficarListaAdyacencia("lista_adyacencia");
    mostrarImagenEnGroupBox(ui->groupBox_6,"amistades_grafo.png");
    mostrarImagenEnGroupBox(ui->groupBox_7,"lista_adyacencia.png");

}




void vAdmin::generarMerkleTreeUltimoBloque() {
    Block* lastBlock = blockchain.getLastBlock();
    if (!lastBlock) {
        QMessageBox::warning(this, "Error", "No hay bloques en la blockchain.");
        return;
    }

    std::string dataJsonStr = lastBlock->data;

    json dataJson;
    try {
        dataJson = json::parse(dataJsonStr);
    } catch (json::exception& e) {
        QMessageBox::warning(this, "Error", "Error al parsear el DATA del bloque: " + QString::fromStdString(e.what()));
        return;
    }

    MerkleTree merkleTree;

    for (const auto& publicacionJson : dataJson) {
        std::string publicacionStr = publicacionJson.dump();

        merkleTree.insert(publicacionStr);
    }

    std::string filename = "arbolMerkle" + std::to_string(lastBlock->index);
    merkleTree.generateGraph(filename);

    QString imagePath = QString::fromStdString(filename) + ".png";
    mostrarImagenEnGroupBox(ui->groupBox_8, imagePath);
}

void vAdmin::on_pushButton_9_clicked()
{
    generarMerkleTreeUltimoBloque();

    mostrarImagenEnGroupBox(ui->groupBox_8,"arbolMerkle.png");

}



void vAdmin::mostrarBloquesEnScrollArea(QScrollArea* scrollArea, Blockchain& blockchain) {
    QWidget* oldWidget = scrollArea->takeWidget();
    if (oldWidget) {
        oldWidget->deleteLater();  // Usar deleteLater para eliminar de manera segura
    }

    QWidget* contenedor = new QWidget();
    QVBoxLayout* layoutContenedor = new QVBoxLayout(contenedor);

    Block* currentBlock = blockchain.first;
    int blockCounter = 0;

    while (currentBlock != nullptr) {
        blockCounter++;

        // Verificar si el bloque es válido
        if (currentBlock->hash.empty() || currentBlock->data.empty()) {
            qDebug() << "Bloque inválido en la posición: " << blockCounter;
            break;
        }

        qDebug() << "Procesando bloque número: " << blockCounter;

        QGroupBox* blockGroupBox = new QGroupBox();
        QVBoxLayout* layoutBlock = new QVBoxLayout(blockGroupBox);

        QLabel* labelBlock = new QLabel("Block: " + QString::number(currentBlock->index));
        QLabel* labelNonce = new QLabel("Nonce: " + QString::number(currentBlock->nonce));
        QLabel* labelData = new QLabel("Data: " + QString::fromStdString(currentBlock->data));
        QLabel* labelPrev = new QLabel("Prev: " + QString::fromStdString(currentBlock->previousHash));
        QLabel* labelHash = new QLabel("Hash: " + QString::fromStdString(currentBlock->hash));

        layoutBlock->addWidget(labelBlock);
        layoutBlock->addWidget(labelNonce);
        layoutBlock->addWidget(labelData);
        layoutBlock->addWidget(labelPrev);
        layoutBlock->addWidget(labelHash);

        // Verificar si el bloque ha sido modificado
        std::string recalculatedHash = currentBlock->generateHash();  // Recalcular el hash

        if (recalculatedHash != currentBlock->hash) {
            // El bloque ha sido modificado
            blockGroupBox->setStyleSheet("background-color: red;");
            qDebug() << "El bloque ha sido modificado. Mostrando en rojo.";
        } else {
            // El bloque es válido y no ha sido modificado
            blockGroupBox->setStyleSheet("background-color: green;");
            qDebug() << "El bloque es válido. Mostrando en verde.";
        }

        layoutContenedor->addWidget(blockGroupBox);

        // Verificar si el siguiente bloque es válido antes de asignar
        if (currentBlock->next == nullptr) {
            qDebug() << "No hay más bloques para procesar después del bloque número: " << blockCounter;
            break;  // No hay más bloques, salir del ciclo
        }

        // Pasar al siguiente bloque
        currentBlock = currentBlock->next;

        // Salir después de procesar hasta 3 bloques para evitar sobrecargar el scroll
        if (blockCounter > 3) {
            qDebug() << "Limite de bloques alcanzado, saliendo.";
            break;
        }
    }

    qDebug() << "Número total de bloques procesados: " << blockCounter;

    // Añadir contenedor al scroll area
    contenedor->setLayout(layoutContenedor);
    scrollArea->setWidget(contenedor);
    scrollArea->setWidgetResizable(true);
    contenedor->adjustSize();
}

void vAdmin::on_pushButton_10_clicked()
{
    blockchain.cargarBlockchain();
    if (blockchain.getSize() == 0) {
        qDebug() << "Blockchain está vacío.";
        return;
    }
    mostrarBloquesEnScrollArea(ui->scrollArea,blockchain);

}

