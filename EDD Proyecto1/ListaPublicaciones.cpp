
#include "Estructs.cpp"

class ListaPublicaciones {
private:
    

public:
Publicacion* cabeza;
    Publicacion* cola;
    ListaPublicaciones() : cabeza(nullptr), cola(nullptr) {}

    void agregarPublicacion(const std::string& correo, const std::string& contenido) {
        Publicacion* nuevaPublicacion = new Publicacion(correo, contenido);
        if (cabeza == nullptr) {
            cabeza = cola = nuevaPublicacion;
        } else {
            cola->siguiente = nuevaPublicacion;
            nuevaPublicacion->anterior = cola;
            cola = nuevaPublicacion;
        }
    }

    void mostrarPublicaciones() const {
        Publicacion* actual = cabeza;
        while (actual != nullptr) {
            std::cout << "Usuario: " << actual->correoUsuario << std::endl;
            std::cout << "Fecha: " << actual->fecha << std::endl;
            std::cout << "Hora: " << actual->hora << std::endl;
            std::cout << "Contenido: " << actual->contenido << std::endl;
            std::cout << "--------------------------------------" << std::endl;
            actual = actual->siguiente;
        }
    }
    
    Publicacion* getCabeza() const {
        return cabeza;
    }
};


class ListaCircularPublicaciones {
private:
    NodoPublicacion* cabeza;

public:
    ListaCircularPublicaciones() : cabeza(nullptr) {}

    void insertarPublicacion(Publicacion* pub) {
        NodoPublicacion* nuevoNodo = new NodoPublicacion(pub);
        if (cabeza == nullptr) {
            cabeza = nuevoNodo;
            cabeza->siguiente = cabeza;
            cabeza->anterior = cabeza;
        } else {
            NodoPublicacion* ultimo = cabeza->anterior;
            ultimo->siguiente = nuevoNodo;
            nuevoNodo->anterior = ultimo;
            nuevoNodo->siguiente = cabeza;
            cabeza->anterior = nuevoNodo;
        }
    }

    void explorarPublicaciones() const {
        if (cabeza == nullptr) {
            std::cout << "No hay publicaciones disponibles." << std::endl;
            return;
        }

        NodoPublicacion* actual = cabeza;
        char opcion;

        do {
            std::cout << "Usuario: " << actual->publicacion->correoUsuario << std::endl;
            std::cout << "Fecha: " << actual->publicacion->fecha << std::endl;
            std::cout << "Hora: " << actual->publicacion->hora << std::endl;
            std::cout << "Contenido: " << actual->publicacion->contenido << std::endl;
            std::cout << "--------------------------------------" << std::endl;

            std::cout << "a) Anterior | s) Siguiente | q) Salir: ";
            std::cin >> opcion;

            if (opcion == 'a') {
                actual = actual->anterior;
            } else if (opcion == 's') {
                actual = actual->siguiente;
            }

        } while (opcion != 'q');
    }


   
    
};

