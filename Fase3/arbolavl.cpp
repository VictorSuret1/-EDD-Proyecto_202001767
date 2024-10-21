#include "arbolavl.h"
#include <iostream>
#include <fstream>
#include <string>

ArbolAVL::ArbolAVL() : raiz(nullptr) {}

ArbolAVL::~ArbolAVL() {
    liberarMemoria(raiz);
}

void ArbolAVL::insertar(const Usuario& usuario) {
    raiz = insertarNodo(raiz, usuario);
}

void ArbolAVL::mostrarUsuarios() {
    if (!raiz) {
        std::cout << "El árbol está vacío.\n";
        return;
    }
    mostrarUsuariosInOrder(raiz);
}

void ArbolAVL::mostrarUsuariosInOrder(NodoAVL* nodo) {
    if (!nodo) return;

    // Recorrer el sub izquierdo
    mostrarUsuariosInOrder(nodo->izquierdo);

    std::cout << "Correo: " << nodo->usuario.correo.toStdString() << "\n";
    std::cout << "Nombres: " << nodo->usuario.nombres.toStdString() << "\n";
    std::cout << "Apellidos: " << nodo->usuario.apellidos.toStdString() << "\n";
    std::cout << "Fecha de Nacimiento: " << nodo->usuario.fechaNacimiento.toStdString() << "\n";
    std::cout << "----------------------------------------\n";

    // Recorrer el sub derecho
    mostrarUsuariosInOrder(nodo->derecho);
}


NodoAVL* ArbolAVL::insertarNodo(NodoAVL* nodo, const Usuario& usuario) {
    if (!nodo) {
        return new NodoAVL(usuario);
    }

    if (usuario.correo < nodo->usuario.correo) {
        nodo->izquierdo = insertarNodo(nodo->izquierdo, usuario);
    } else if (usuario.correo > nodo->usuario.correo) {
        nodo->derecho = insertarNodo(nodo->derecho, usuario);
    } else {
        return nodo; // Correo duplicado, no se inserta
    }

    nodo->altura = 1 + std::max(altura(nodo->izquierdo), altura(nodo->derecho));
    return balancear(nodo);
}

int ArbolAVL::altura(NodoAVL* nodo) {
    return nodo ? nodo->altura : 0;
}

int ArbolAVL::factorBalance(NodoAVL* nodo) {
    return nodo ? altura(nodo->izquierdo) - altura(nodo->derecho) : 0;
}

NodoAVL* ArbolAVL::balancear(NodoAVL* nodo) {
    int balance = factorBalance(nodo);

    if (balance > 1) {
        if (factorBalance(nodo->izquierdo) < 0) {
            nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        }
        return rotarDerecha(nodo);
    }

    if (balance < -1) {
        if (factorBalance(nodo->derecho) > 0) {
            nodo->derecho = rotarDerecha(nodo->derecho);
        }
        return rotarIzquierda(nodo);
    }

    return nodo;
}

NodoAVL* ArbolAVL::rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* T2 = x->derecho;

    x->derecho = y;
    y->izquierdo = T2;

    y->altura = std::max(altura(y->izquierdo), altura(y->derecho)) + 1;
    x->altura = std::max(altura(x->izquierdo), altura(x->derecho)) + 1;

    return x;
}

NodoAVL* ArbolAVL::rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* T2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = T2;

    x->altura = std::max(altura(x->izquierdo), altura(x->derecho)) + 1;
    y->altura = std::max(altura(y->izquierdo), altura(y->derecho)) + 1;

    return y;
}

NodoAVL* ArbolAVL::buscar(const QString& correo) {
    return buscarRecursivo(raiz, correo);
}

NodoAVL* ArbolAVL::buscarRecursivo(NodoAVL* nodo, const QString& correo) {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nodo->usuario.correo == correo) {
        return nodo;
    } else if (correo < nodo->usuario.correo) {
        return buscarRecursivo(nodo->izquierdo, correo);  // Buscar en la subárbol izquierdo
    } else {
        return buscarRecursivo(nodo->derecho, correo);  // Buscar en la subárbol derecho
    }
}

void ArbolAVL::eliminar(const QString& correo) {
    raiz = eliminarNodo(raiz, correo);
}

NodoAVL* ArbolAVL::eliminarNodo(NodoAVL* nodo, const QString& correo) {
    if (!nodo) {
        return nodo;
    }

    // Buscar el nodo a eliminar
    if (correo < nodo->usuario.correo) {
        nodo->izquierdo = eliminarNodo(nodo->izquierdo, correo);
    } else if (correo > nodo->usuario.correo) {
        nodo->derecho = eliminarNodo(nodo->derecho, correo);
    } else {
        // Nodo encontrado
        if (!nodo->izquierdo || !nodo->derecho) {
            // Nodo con un solo hijo o sin hijos
            NodoAVL* temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;

            // Sin hijos
            if (!temp) {
                temp = nodo;
                nodo = nullptr;
            } else {
                // Un solo hijo
                *nodo = *temp; // Copiar los contenidos del nodo no vacío
            }
            delete temp;
        } else {
            // Nodo con dos hijos
            NodoAVL* temp = minimoValorNodo(nodo->derecho); // Encontrar el sucesor en orden
            nodo->usuario = temp->usuario; // Copiar los datos del sucesor al nodo actual
            nodo->derecho = eliminarNodo(nodo->derecho, temp->usuario.correo); // Eliminar el sucesor
        }
    }

    // Si el arbol tenia solo un nodo, regresa
    if (!nodo) {
        return nodo;
    }

    // Actualizar la altura del nodo actual
    nodo->altura = 1 + std::max(altura(nodo->izquierdo), altura(nodo->derecho));

    return balancear(nodo);
}

NodoAVL* ArbolAVL::minimoValorNodo(NodoAVL* nodo) {
    NodoAVL* actual = nodo;

    // El nodo mas a la izquierda tiene el valor minimo
    while (actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }

    return actual;
}

NodoAVL* ArbolAVL::buscarNodo(NodoAVL* nodo, const QString& correo) {
    if (!nodo || nodo->usuario.correo == correo) {
        return nodo;
    }

    if (correo < nodo->usuario.correo) {
        return buscarNodo(nodo->izquierdo, correo);
    } else {
        return buscarNodo(nodo->derecho, correo);
    }
}

void ArbolAVL::liberarMemoria(NodoAVL* nodo) {
    if (nodo) {
        liberarMemoria(nodo->izquierdo);
        liberarMemoria(nodo->derecho);
        delete nodo;
    }
}



void ArbolAVL::enviarSolicitud(const QString& correoEmisor, const QString& correoReceptor) {
    NodoAVL* emisor = buscar(correoEmisor);
    NodoAVL* receptor = buscar(correoReceptor);

    if (!emisor || !receptor) {
        std::cout << "Error: Emisor o receptor no encontrados para la solicitud.\n";
        return;
    }

    // Verificar si ya existe una solicitud pendiente en ambas direcciones
    for (const auto& solicitud : emisor->usuario.solicitudesEnviadas) {
        if (solicitud->correoReceptor == correoReceptor && solicitud->estado == "PENDIENTE") {
            std::cout << "Ya existe una solicitud pendiente entre estos usuarios.\n";
            return;
        }
    }

    Solicitud* nuevaSolicitud = new Solicitud{correoEmisor, correoReceptor, "PENDIENTE"};

    emisor->usuario.solicitudesEnviadas.append(nuevaSolicitud);

    receptor->usuario.solicitudesRecibidas.append(nuevaSolicitud);

    std::cout << "Solicitud de amistad enviada: " << correoEmisor.toStdString() << " -> " << correoReceptor.toStdString() << "\n";
}


void ArbolAVL::aceptarSolicitud(const QString& correoEmisor, const QString& correoReceptor, GrafoAmistades& grafo) {
    NodoAVL* receptor = buscar(correoReceptor);

    if (!receptor) {
        std::cout << "Error: Receptor no encontrado para la solicitud.\n";
        return;
    }

    // Buscar la solicitud pendiente en la lista de solicitudes recibidas
    auto it = std::find_if(receptor->usuario.solicitudesRecibidas.begin(),
                           receptor->usuario.solicitudesRecibidas.end(),
                           [&correoEmisor](Solicitud* s) { return s->correoEmisor == correoEmisor && s->estado == "PENDIENTE"; });

    if (it != receptor->usuario.solicitudesRecibidas.end()) {
        Solicitud* solicitud = *it;
        solicitud->estado = "ACEPTADA";

        // Convertir QString a std::string para usar con el grafo
        QString emisorQString = correoEmisor;
        QString receptorQString = correoReceptor;

        // Agregar la relación de amistad al grafo
        grafo.agregarAmistad(emisorQString, receptorQString);

        // Remover la solicitud de las listas de enviadas y recibidas
        eliminarSolicitud(correoEmisor, correoReceptor);

        std::cout << "Solicitud de amistad aceptada: " << emisorQString.toStdString() << " -> " << receptorQString.toStdString() << "\n";
    } else {
        std::cout << "Solicitud no encontrada o ya aceptada.\n";
    }
}


void ArbolAVL::rechazarSolicitud(const QString& correoEmisor, const QString& correoReceptor) {
    NodoAVL* receptor = buscar(correoReceptor);

    if (!receptor) {
        std::cout << "Error: Receptor no encontrado para la solicitud.\n";
        return;
    }

    // Buscar la solicitud pendiente en la lista de solicitudes recibidas
    auto it = std::find_if(receptor->usuario.solicitudesRecibidas.begin(),
                           receptor->usuario.solicitudesRecibidas.end(),
                           [&correoEmisor](Solicitud* s) { return s->correoEmisor == correoEmisor && s->estado == "PENDIENTE"; });

    if (it != receptor->usuario.solicitudesRecibidas.end()) {
        Solicitud* solicitud = *it;
        solicitud->estado = "RECHAZADA";

        // Remover la solicitud de las listas de enviadas y recibidas
        eliminarSolicitud(correoEmisor, correoReceptor);

        std::cout << "Solicitud de amistad rechazada: " << correoEmisor.toStdString() << " -> " << correoReceptor.toStdString() << "\n";
    } else {
        std::cout << "Solicitud no encontrada o ya rechazada.\n";
    }
}


void ArbolAVL::eliminarSolicitud(const QString& correoEmisor, const QString& correoReceptor) {
    NodoAVL* emisor = buscar(correoEmisor);
    NodoAVL* receptor = buscar(correoReceptor);

    if (emisor) {
        auto it = std::remove_if(emisor->usuario.solicitudesEnviadas.begin(),
                                 emisor->usuario.solicitudesEnviadas.end(),
                                 [&correoReceptor](Solicitud* s) { return s->correoReceptor == correoReceptor; });

        if (it != emisor->usuario.solicitudesEnviadas.end()) {
            emisor->usuario.solicitudesEnviadas.erase(it, emisor->usuario.solicitudesEnviadas.end());
        }
    }

    if (receptor) {
        auto it = std::remove_if(receptor->usuario.solicitudesRecibidas.begin(),
                                 receptor->usuario.solicitudesRecibidas.end(),
                                 [&correoEmisor](Solicitud* s) { return s->correoEmisor == correoEmisor; });

        if (it != receptor->usuario.solicitudesRecibidas.end()) {
            receptor->usuario.solicitudesRecibidas.erase(it, receptor->usuario.solicitudesRecibidas.end());
        }
    }
}

NodoAVL* ArbolAVL::obtenerRaiz() {
    return raiz;
}


void ArbolAVL::recorrerPreOrden(NodoAVL* nodo, QList<Usuario>& usuarios) {
    if (nodo == nullptr) return;

    usuarios.append(nodo->usuario);
    recorrerPreOrden(nodo->izquierdo, usuarios);
    recorrerPreOrden(nodo->derecho, usuarios);
}


void ArbolAVL::recorrerInOrden(NodoAVL* nodo, QList<Usuario>& usuarios) {
    if (nodo == nullptr) return;

    recorrerInOrden(nodo->izquierdo, usuarios);
    usuarios.append(nodo->usuario);
    recorrerInOrden(nodo->derecho, usuarios);
}


void ArbolAVL::recorrerPostOrden(NodoAVL* nodo, QList<Usuario>& usuarios) {
    if (nodo == nullptr) return;

    recorrerPostOrden(nodo->izquierdo, usuarios);
    recorrerPostOrden(nodo->derecho, usuarios);
    usuarios.append(nodo->usuario);
}

QList<Usuario> ArbolAVL::obtenerRecorrido(const QString& tipoRecorrido) {
    QList<Usuario> usuarios;

    if (tipoRecorrido == "PreOrden") {
        recorrerPreOrden(raiz, usuarios);
    } else if (tipoRecorrido == "InOrden") {
        recorrerInOrden(raiz, usuarios);
    } else if (tipoRecorrido == "PostOrden") {
        recorrerPostOrden(raiz, usuarios);
    }

    return usuarios;
}

void generarDotConEtiquetas(NodoAVL* nodo, std::ofstream& file) {
    if (nodo == nullptr) return;

    if (nodo->izquierdo) {
        file << "    \"" << nodo->usuario.correo.toStdString() << "\" -> \"" << nodo->izquierdo->usuario.correo.toStdString()
             << "\" [label=\"Izquierdo\"];\n";
        generarDotConEtiquetas(nodo->izquierdo, file);
    }
    if (nodo->derecho) {
        file << "    \"" << nodo->usuario.correo.toStdString() << "\" -> \"" << nodo->derecho->usuario.correo.toStdString()
             << "\" [label=\"Derecho\"];\n";
        generarDotConEtiquetas(nodo->derecho, file);
    }
}

void generarDotArbolAVL(NodoAVL* nodo, std::ofstream& file) {
    if (nodo == nullptr) return;


    file << "    \"" << nodo->usuario.correo.toStdString() << "\" [label=\""
         << nodo->usuario.nombres.toStdString() << " "
         << nodo->usuario.apellidos.toStdString() << "\\n"
         << nodo->usuario.correo.toStdString() << "\"];\n";

    if (nodo->izquierdo) {
        file << "    \"" << nodo->usuario.correo.toStdString() << "\" -> \"" << nodo->izquierdo->usuario.correo.toStdString()
        << "\" [label=\"Izquierdo\"];\n";
        generarDotConEtiquetas(nodo->izquierdo, file);
    }
    if (nodo->derecho) {
        file << "    \"" << nodo->usuario.correo.toStdString() << "\" -> \"" << nodo->derecho->usuario.correo.toStdString()
        << "\" [label=\"Derecho\"];\n";
        generarDotConEtiquetas(nodo->derecho, file);
    }
}

void ArbolAVL::exportarArbolAVLaDot(ArbolAVL& arbol, const std::string& rutaArchivo) {
    std::ofstream file(rutaArchivo);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escribir el DOT.\n";
        return;
    }

    file << "digraph AVL {\n";
    file << "    node [shape=ellipse];\n";
    generarDotArbolAVL(arbol.obtenerRaiz(), file);
    file << "}\n";
    file.close();
}
