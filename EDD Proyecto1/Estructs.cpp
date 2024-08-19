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
    std::string correo;
    std::string contenido;
    std::string fecha;
    std::string hora;
    Publicacion* anterior;
    Publicacion* siguiente;

    Publicacion(const std::string& correo, const std::string& contenido, const std::string& fecha, const std::string& hora)
        : correo(correo), contenido(contenido), fecha(fecha), hora(hora), anterior(nullptr), siguiente(nullptr) {}
};

