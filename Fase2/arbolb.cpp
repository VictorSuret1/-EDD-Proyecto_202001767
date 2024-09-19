#include "arbolb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <QProcess>
ArbolB::ArbolB() : raiz(nullptr) {}

ArbolB::~ArbolB() {
    limpiar(raiz);
}

void ArbolB::insertarComentario(const QString& usuario, const QString& texto, const QDateTime& fechaHora) {
    Comentario nuevoComentario(usuario, texto, fechaHora);

    // Si el arbol esta vacio crea la raiz
    if (!raiz) {
        raiz = new NodoB(true);
        raiz->comentarios.push_back(nuevoComentario);
    } else {
        // Si la raiz esta llena, se necesita dividir
        if (raiz->comentarios.size() == 4) {  //ajustar orden del arbol
            NodoB* nuevaRaiz = new NodoB(false);
            nuevaRaiz->hijos.push_back(raiz);
            dividirNodo(nuevaRaiz, 0, raiz);

            int i = (nuevaRaiz->comentarios[0].fechaHora < nuevoComentario.fechaHora) ? 1 : 0;
            insertarEnNodo(nuevaRaiz->hijos[i], nuevoComentario);

            raiz = nuevaRaiz;
        } else {
            insertarEnNodo(raiz, nuevoComentario);
        }
    }
}

void ArbolB::insertarEnNodo(NodoB* nodo, const Comentario& comentario) {
    int i = nodo->comentarios.size() - 1;

    // Si el nodo es hoja, inserta directamente en el nodo
    if (nodo->esHoja) {
        nodo->comentarios.push_back(comentario);
        std::sort(nodo->comentarios.begin(), nodo->comentarios.end(), [](const Comentario& a, const Comentario& b) {
            return a.fechaHora < b.fechaHora;
        });
    } else {
        // Encuentra la posición correcta para el hijo
        while (i >= 0 && comentario.fechaHora < nodo->comentarios[i].fechaHora) {
            i--;
        }
        i++;

        // Si el hijo está lleno se divide
        if (nodo->hijos[i]->comentarios.size() == 4) {
            dividirNodo(nodo, i, nodo->hijos[i]);

            if (comentario.fechaHora > nodo->comentarios[i].fechaHora) {
                i++;
            }
        }
        insertarEnNodo(nodo->hijos[i], comentario);
    }
}

void ArbolB::dividirNodo(NodoB* padre, int i, NodoB* hijo) {
    NodoB* nuevoNodo = new NodoB(hijo->esHoja);
    nuevoNodo->comentarios.insert(nuevoNodo->comentarios.end(), hijo->comentarios.begin() + 2, hijo->comentarios.end());
    hijo->comentarios.resize(2);

    if (!hijo->esHoja) {
        nuevoNodo->hijos.insert(nuevoNodo->hijos.end(), hijo->hijos.begin() + 2, hijo->hijos.end());
        hijo->hijos.resize(2);
    }

    padre->hijos.insert(padre->hijos.begin() + i + 1, nuevoNodo);
    padre->comentarios.insert(padre->comentarios.begin() + i, hijo->comentarios[2]);
    hijo->comentarios.pop_back();
}

void ArbolB::mostrarComentariosOrdenados() const {
    mostrarEnOrden(raiz);
}

void ArbolB::mostrarEnOrden(NodoB* nodo) const {
    if (!nodo) return;

    for (size_t i = 0; i < nodo->comentarios.size(); i++) {
        if (!nodo->esHoja) {
            mostrarEnOrden(nodo->hijos[i]);
        }
        std::cout << "Usuario: " << nodo->comentarios[i].correoUsuario.toStdString() << std::endl;
        std::cout << "Fecha y Hora: " << nodo->comentarios[i].fechaHora.toString("dd/MM/yyyy hh:mm:ss").toStdString() << std::endl;
        std::cout << "Texto: " << nodo->comentarios[i].contenido.toStdString() << std::endl;
        std::cout << "--------------------------------------" << std::endl;
    }

    if (!nodo->esHoja) {
        mostrarEnOrden(nodo->hijos.back());
    }
}

void ArbolB::limpiar(NodoB* nodo) {
    if (!nodo) return;

    for (auto hijo : nodo->hijos) {
        limpiar(hijo);
    }

    delete nodo;
}


void ArbolB::graficarArbolB(ArbolB* arbol, const QString& filename) const {
    std::ofstream archivo(filename.toStdString() + ".dot");

    if (archivo.is_open()) {
        archivo << "digraph G {" << std::endl;
        archivo << "node [shape=record, style=filled, fillcolor=white, fontsize=10];" << std::endl;
        archivo << "splines=false;" << std::endl;

        // Función recursiva para recorrer y graficar el árbol
        std::function<void(NodoB*)> graficarNodo = [&](NodoB* nodo) {
            if (!nodo) return;

            // Crear nodo con identificador único basado en la dirección de memoria
            QString nodoId = QString("nodo_%1").arg(reinterpret_cast<uintptr_t>(nodo));
            archivo << nodoId.toStdString() << " [label=\"<f0>|";

            for (int i = 0; i < nodo->comentarios.size(); ++i) {
                archivo << "<f" << i + 1 << "> Comentario "
                        << nodo->comentarios[i].fechaHora.toString("dd/MM/yyyy hh:mm").toStdString()
                        << "\\nUsuario: " << nodo->comentarios[i].correoUsuario.toStdString()
                        << "\\n" << nodo->comentarios[i].contenido.toStdString();

                if (i < nodo->comentarios.size() - 1) {
                    archivo << "|";
                }
            }

            archivo << "|<f" << nodo->comentarios.size() + 1 << ">\"];" << std::endl;

            // Recursivamente graficar hijos si no es hoja
            for (int i = 0; i <= nodo->comentarios.size(); ++i) {
                if (!nodo->esHoja && i < nodo->hijos.size()) {
                    QString childId = QString("nodo_%1").arg(reinterpret_cast<uintptr_t>(nodo->hijos[i]));
                    graficarNodo(nodo->hijos[i]);

                    // Conectar los hijos con el nodo actual
                    archivo << nodoId.toStdString() << ":f" << i << " -> " << childId.toStdString() << ":f0;" << std::endl;
                }
            }
        };

        graficarNodo(arbol->raiz);

        archivo << "}" << std::endl;
        archivo.close();

        QProcess::execute("dot", QStringList() << "-Tpng" << filename + ".dot" << "-o" << filename + ".png");
    } else {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
    }
}

void ArbolB::recorrerComentarios(const std::function<void(const Comentario&)>& accion) const {
    recorrerComentariosRecursivo(raiz, accion);
}

void ArbolB::recorrerComentariosRecursivo(NodoB* nodo, const std::function<void(const Comentario&)>& accion) const {
    if (!nodo) return;

    for (int i = 0; i < nodo->comentarios.size(); ++i) {
        if (!nodo->esHoja) {
            recorrerComentariosRecursivo(nodo->hijos[i], accion);
        }
        accion(nodo->comentarios[i]);
    }

    // Procesar el último hijo si no es hoja
    if (!nodo->esHoja) {
        recorrerComentariosRecursivo(nodo->hijos[nodo->comentarios.size()], accion);
    }
}
