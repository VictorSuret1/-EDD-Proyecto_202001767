#include "arbolabb.h"
#include <iostream>

ArbolABB::ArbolABB() : raiz(nullptr) {}

ArbolABB::~ArbolABB() {
    limpiar(raiz);
}

void ArbolABB::insertarPublicacion(const QString& correo, const QString& contenido, const QString& rutaImagen, const QDate& fecha) {
    raiz = insertarNodo(raiz, fecha);
    NodoABB* nodo = raiz;

    // Buscar el nodo con la fecha
    while (nodo->fecha != fecha) {
        if (fecha < nodo->fecha) {
            nodo = nodo->izquierda;
        } else {
            nodo = nodo->derecha;
        }
    }
    Publicacion* nuevaPublicacion = new Publicacion(correo, contenido, rutaImagen);
    insertarEnLista(nodo, nuevaPublicacion);
}

NodoABB* ArbolABB::insertarNodo(NodoABB* nodo, const QDate& fecha) {
    if (nodo == nullptr) {
        return new NodoABB(fecha);
    }

    if (fecha < nodo->fecha) {
        nodo->izquierda = insertarNodo(nodo->izquierda, fecha);
    } else if (fecha > nodo->fecha) {
        nodo->derecha = insertarNodo(nodo->derecha, fecha);
    }
    // Si la fecha ya existe, no inserta un nuevo nodo solo a la lista

    return nodo;
}

void ArbolABB::insertarEnLista(NodoABB* nodo, Publicacion* nuevaPublicacion) {
    if (nodo->listaPublicaciones == nullptr) {
        nodo->listaPublicaciones = nuevaPublicacion;
    } else {
        Publicacion* actual = nodo->listaPublicaciones;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevaPublicacion;
        nuevaPublicacion->anterior = actual;
    }
}

void ArbolABB::mostrarPublicacionesOrdenadas() const {
    mostrarEnOrden(raiz);
}

//fucion de prueba para mostrar en consola
void ArbolABB::mostrarEnOrden(NodoABB* nodo) const {
    if (nodo == nullptr) return;

    mostrarEnOrden(nodo->izquierda);

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        std::cout << "Fecha: " << nodo->fecha.toString("dd/MM/yyyy").toStdString() << std::endl;
        std::cout << "Usuario: " << actual->correoUsuario.toStdString() << std::endl;
        std::cout << "Contenido: " << actual->contenido.toStdString() << std::endl;
        if (!actual->rutaImagen.isEmpty()) {
            std::cout << "Imagen: " << actual->rutaImagen.toStdString() << std::endl;
        }
        std::cout << "--------------------------------------" << std::endl;
        actual = actual->siguiente;
    }

    mostrarEnOrden(nodo->derecha);
}

void ArbolABB::limpiar(NodoABB* nodo) {
    if (nodo == nullptr) return;

    limpiar(nodo->izquierda);
    limpiar(nodo->derecha);

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        Publicacion* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }

    delete nodo;
}


NodoABB* ArbolABB::buscar(const QDate& fecha) const {
    return buscarNodo(raiz, fecha);
}

NodoABB* ArbolABB::buscarNodo(NodoABB* nodo, const QDate& fecha) const {
    if (nodo == nullptr || nodo->fecha == fecha) {
        return nodo;
    }

    if (fecha < nodo->fecha) {
        return buscarNodo(nodo->izquierda, fecha);
    } else {
        return buscarNodo(nodo->derecha, fecha);
    }
}

void ArbolABB::mostrarPublicaciones() const {
    mostrarPublicacionesRecursivo(raiz);
}

void ArbolABB::mostrarPublicacionesRecursivo(NodoABB* nodo) const {
    if (!nodo) return;

    // Mostrar publicaciones en el sub izquierdo
    mostrarPublicacionesRecursivo(nodo->izquierda);

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        std::cout << "Fecha: " << nodo->fecha.toString("dd/MM/yyyy").toStdString() << std::endl;
        std::cout << "Usuario: " << actual->correoUsuario.toStdString() << std::endl;
        std::cout << "Contenido: " << actual->contenido.toStdString() << std::endl;

        if (!actual->rutaImagen.isEmpty()) {
            std::cout << "Imagen: " << actual->rutaImagen.toStdString() << std::endl;
        }

        std::cout << "Comentarios:" << std::endl;
        actual->arbolComentarios.mostrarComentarios();

        std::cout << "--------------------------------------" << std::endl;
        actual = actual->siguiente;
    }

    // Mostrar publicaciones en el sub derecho
    mostrarPublicacionesRecursivo(nodo->derecha);
}


void ArbolB::mostrarComentarios() const {
    mostrarComentariosRecursivo(raiz);
}

void ArbolB::mostrarComentariosRecursivo(NodoB* nodo) const {
    if (!nodo) return;

    // Mostrar comentarios en orden
    for (int i = 0; i < nodo->comentarios.size(); ++i) {
        if (!nodo->esHoja) {
            mostrarComentariosRecursivo(nodo->hijos[i]);
        }
        std::cout << "   - " << nodo->comentarios[i].fechaHora.toString("dd/MM/yyyy hh:mm").toStdString()
                  << " " << nodo->comentarios[i].correoUsuario.toStdString() << ": "
                  << nodo->comentarios[i].contenido.toStdString() << std::endl;
    }

    // Mostrar comentarios del último hijo si no es hoja
    if (!nodo->esHoja) {
        mostrarComentariosRecursivo(nodo->hijos[nodo->comentarios.size()]);
    }
}

NodoABB* ArbolABB::obtenerRaiz() const {
    return raiz;
}


void ArbolABB::preOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones) {
    if (nodo == nullptr) return;

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        publicaciones.push_back(actual);
        actual = actual->siguiente;
    }
    preOrden(nodo->izquierda, publicaciones);
    preOrden(nodo->derecha, publicaciones);
}

void ArbolABB::postOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones) {
    if (nodo == nullptr) return;

    postOrden(nodo->izquierda, publicaciones);
    postOrden(nodo->derecha, publicaciones);

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        publicaciones.push_back(actual);
        actual = actual->siguiente;
    }
}

void ArbolABB::inOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones) {
    if (nodo == nullptr) return;

    inOrden(nodo->izquierda, publicaciones);

    Publicacion* actual = nodo->listaPublicaciones;
    while (actual != nullptr) {
        publicaciones.push_back(actual);
        actual = actual->siguiente;
    }

    inOrden(nodo->derecha, publicaciones);
}
