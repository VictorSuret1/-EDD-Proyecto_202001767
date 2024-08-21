#include <iostream>
#include <string>
#include <stack>

struct Usuario;  // Declaración adelantada

struct Solicitud {
    Usuario* emisor;
    Solicitud* siguiente;
};

struct Usuario {
    std::string nombres;
    std::string apellidos;
    std::string fechaNacimiento;
    std::string correo;
    std::string contrasena;
    Usuario* siguiente;  // Puntero al siguiente usuario en la lista enlazada
    std::stack<Solicitud*> solicitudesRecibidas;  // Pila de solicitudes recibidas
    Solicitud* solicitudesEnviadas;  // Lista simple de solicitudes enviadas

    Usuario() : siguiente(nullptr), solicitudesEnviadas(nullptr) {}
};

struct Publicacion {
    std::string correoUsuario;
    std::string contenido;
    std::string fecha;
    std::string hora;
    
    Publicacion* siguiente;
    Publicacion* anterior;

    Publicacion(const std::string& correo, const std::string& texto)
        : correoUsuario(correo), contenido(texto), siguiente(nullptr), anterior(nullptr) {
        // Obtener la fecha y hora actuales
        time_t ahora = time(0);
        tm* tiempoLocal = localtime(&ahora);
        fecha = std::to_string(tiempoLocal->tm_mday) + "/" + 
                std::to_string(1 + tiempoLocal->tm_mon) + "/" + 
                std::to_string(1900 + tiempoLocal->tm_year);
        hora = std::to_string(tiempoLocal->tm_hour) + ":" + 
               std::to_string(tiempoLocal->tm_min);
    }
};

struct NodoPublicacion {
    Publicacion* publicacion;
    NodoPublicacion* siguiente;
    NodoPublicacion* anterior;

    NodoPublicacion(Publicacion* pub) : publicacion(pub), siguiente(nullptr), anterior(nullptr) {}
};