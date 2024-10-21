#include "listapublicaciones.h"
#include <iostream>

ListaPublicaciones::ListaPublicaciones() : cabeza(nullptr), cola(nullptr) {}

ListaPublicaciones::~ListaPublicaciones() {
    limpiar();
}

void ListaPublicaciones::agregarPublicacion(const QString& correo, const QString& contenido, const QString& rutaImagen, const QDateTime& fechaHora) {
    Publicacion* nuevaPublicacion = new Publicacion(correo, contenido, rutaImagen, fechaHora);

    // Depuración
    qDebug() << "Agregando publicación: " << correo << ", " << contenido << ", " << fechaHora;

    if (cabeza == nullptr) {
        cabeza = cola = nuevaPublicacion;
    } else {
        cola->siguiente = nuevaPublicacion;
        nuevaPublicacion->anterior = cola;
        cola = nuevaPublicacion;
    }

}


void ListaPublicaciones::mostrarPublicaciones() const {
    Publicacion* actual = cabeza;
    while (actual != nullptr) {
        std::cout << "Usuario: " << actual->correoUsuario.toStdString() << std::endl;
        std::cout << "Fecha y Hora: " << actual->fechaHora.toString("dd/MM/yyyy hh:mm:ss").toStdString() << std::endl;
        std::cout << "Contenido: " << actual->contenido.toStdString() << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        actual = actual->siguiente;
    }
}

bool ListaPublicaciones::estaVacia() const {
    return cabeza == nullptr;
}

Publicacion* ListaPublicaciones::getCabeza() const {
    return cabeza;
}

void ListaPublicaciones::limpiar() {
    Publicacion* actual = cabeza;
    while (actual != nullptr) {
        Publicacion* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }
    cabeza = cola = nullptr;
}

std::string ListaPublicaciones::toJson() const {
    json j = json::array();  // Crear un array JSON
    Publicacion* actual = cabeza;
    while (actual != nullptr) {
        j.push_back(json::parse(actual->toJson()));  // Añadir la publicación a la lista JSON
        actual = actual->siguiente;
    }
    return j.dump();
}
