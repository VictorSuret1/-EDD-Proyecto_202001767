#include "Estructs.cpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;




class ListaUsuarios {
private:
    Usuario* cabeza;

public:
    ListaUsuarios() : cabeza(nullptr) {}

    // Verificar si el correo ya está registrado
    bool correoExiste(const std::string& correo) {
        Usuario* actual = cabeza;
        while (actual != nullptr) {
            if (actual->correo == correo) {
                return true;
            }
            actual = actual->siguiente;
        }
        return false;
    }

    // Obtener un usuario por correo
    Usuario* obtenerUsuario(const std::string& correo) {
        Usuario* actual = cabeza;
        while (actual != nullptr) {
            if (actual->correo == correo) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // Registrar un nuevo usuario
    void registrarUsuario(const std::string& nombres, const std::string& apellidos,
                          const std::string& fechaNacimiento, const std::string& correo,
                          const std::string& contrasena) {
        if (correoExiste(correo)) {
            std::cout << "El correo ya está registrado.\n";
            return;
        }

        Usuario* nuevoUsuario = new Usuario();
        nuevoUsuario->nombres = nombres;
        nuevoUsuario->apellidos = apellidos;
        nuevoUsuario->fechaNacimiento = fechaNacimiento;
        nuevoUsuario->correo = correo;
        nuevoUsuario->contrasena = contrasena;
        nuevoUsuario->siguiente = nullptr;
        nuevoUsuario->solicitudesEnviadas = nullptr;  // Inicializa la lista de solicitudes enviadas

        

        if (cabeza == nullptr) {
            cabeza = nuevoUsuario;
        } else {
            Usuario* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevoUsuario;
        }

        std::cout << "Usuario registrado exitosamente.\n";
        mostrarUsuarios();
    }

    // Iniciar sesión
    bool iniciarSesion(const std::string& correo, const std::string& contrasena) {
        
    if (correo == "admin@gmail.com" && contrasena == "EDD2S2024") {
        std::cout << "Inicio de sesión como administrador exitoso.\n";
        std::string path;
        std::string path2;
        std::cout << "Ingrese el path del archivo JSON para la carga masiva: ";
        std::cin.ignore();
        std::getline(std::cin, path);
        cargarUsuariosDesdeJSON("C:/Users/19724/OneDrive/Desktop/EDD Proyecto1/Users.Json");
        std::cout << "Ingrese el path del archivo JSON para la carga masiva de solicitudes: ";
        std::cin.ignore();
        std::getline(std::cin, path2);
        cargarSolicitudesDesdeJSON("C:/Users/19724/OneDrive/Desktop/EDD Proyecto1/Soli.Json");
        return true;
    }

        Usuario* usuario = obtenerUsuario(correo);
        if (usuario != nullptr && usuario->contrasena == contrasena) {
            std::cout << "Inicio de sesión exitoso.\n";
            mostrarUsuarios();
            mostrarMenuUsuario(usuario);
            return true;
        }
        std::cout << "Correo o contraseña incorrectos.\n";
        return false;
    }

    void cargarUsuariosDesdeJSON(const std::string& path) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << path << "\n";
        return;
    }

    json root;
    archivo >> root;

    for (const auto& usuarioData : root) {
        // Verificar si cada campo existe antes de intentar acceder
        if (usuarioData.contains("nombres") && 
            usuarioData.contains("apellidos") && 
            usuarioData.contains("fecha_de_nacimiento") && 
            usuarioData.contains("correo") && 
            usuarioData.contains("contraseña")) {

            std::string nombres = usuarioData["nombres"];
            std::string apellidos = usuarioData["apellidos"];
            std::string fechaNacimiento = usuarioData["fecha_de_nacimiento"];
            std::string correo = usuarioData["correo"];
            std::string contrasena = usuarioData["contraseña"];

            // Registrar el usuario en la lista
            registrarUsuario(nombres, apellidos, fechaNacimiento, correo, contrasena);
        } else {
            std::cout << "Error: JSON malformado. Faltan campos requeridos para un usuario.\n";
        }
    }

    std::cout << "Carga masiva completada.\n";
}


void cargarSolicitudesDesdeJSON(const std::string& path) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo: " << path << "\n";
        return;
    }

    json root;
    archivo >> root;

    for (const auto& solicitudData : root) {
        // Verificar que los campos "emisor", "receptor" y "estado" existan
        if (solicitudData.contains("emisor") && 
            solicitudData.contains("receptor") && 
            solicitudData.contains("estado")) {

            std::string correoEmisor = solicitudData["emisor"];
            std::string correoReceptor = solicitudData["receptor"];
            std::string estado = solicitudData["estado"];

            Usuario* emisor = obtenerUsuario(correoEmisor);
            Usuario* receptor = obtenerUsuario(correoReceptor);

            if (!emisor || !receptor) {
                std::cout << "Error: Emisor o receptor no encontrados para la solicitud.\n";
                continue;
            }

            if (estado == "PENDIENTE") {
                // Crear la solicitud y agregarla a la lista de solicitudes del receptor
                Solicitud* nuevaSolicitud = new Solicitud{emisor, nullptr};

                // Agregar a la lista de solicitudes enviadas del emisor
                nuevaSolicitud->siguiente = emisor->solicitudesEnviadas;
                emisor->solicitudesEnviadas = nuevaSolicitud;

                // Agregar a la pila de solicitudes recibidas del receptor
                receptor->solicitudesRecibidas.push(nuevaSolicitud);

                std::cout << "Solicitud de amistad pendiente cargada: " << correoEmisor << " -> " << correoReceptor << "\n";
            } else if (estado == "ACEPTADA") {
                // Dejamos pendiente la implementación del estado "ACEPTADA"
                std::cout << "Solicitud de amistad aceptada: " << correoEmisor << " -> " << correoReceptor << " (pendiente de implementación)\n";
            }
        } else {
            std::cout << "Error: JSON malformado. Faltan campos requeridos para una solicitud.\n";
        }
    }

    std::cout << "Carga masiva de solicitudes completada.\n";
}



    // Mostrar menú para el usuario que ha iniciado sesión
    void mostrarMenuUsuario(Usuario* usuario) {
        int opcion;
        do {
            std::cout << "\n--- Menu de Usuario ---\n";
            std::cout << "1. Ver Solicitudes de Amistad\n";
            std::cout << "2. Enviar Solicitud de Amistad\n";
            std::cout << "3. Cerrar Sesión\n";
            std::cout << "Seleccione una opción: ";
            std::cin >> opcion;

            switch (opcion) {
                case 1:
                    verSolicitudesAmistad(usuario);
                    break;

                case 2: {
                    std::string correoReceptor;
                    std::cout << "Ingrese el correo del usuario al que quiere enviar la solicitud: ";
                    std::cin.ignore();
                    std::getline(std::cin, correoReceptor);
                    enviarSolicitudAmistad(usuario->correo, correoReceptor);
                    break;
                }
                case 3:
                    std::cout << "Cerrando sesión...\n";
                    break;

                default:
                    std::cout << "Opción no válida.\n";
                    break;
            }
        } while (opcion != 3);
    }

    // Ver las solicitudes de amistad recibidas y rechazar alguna
    void verSolicitudesAmistad(Usuario* usuario) {
        if (usuario->solicitudesRecibidas.empty()) {
            std::cout << "No tienes solicitudes de amistad.\n";
            return;
        }

        std::stack<Solicitud*> tempStack;
        int index = 1;
        std::cout << "Solicitudes de amistad recibidas:\n";
        while (!usuario->solicitudesRecibidas.empty()) {
            Solicitud* solicitud = usuario->solicitudesRecibidas.top();
            usuario->solicitudesRecibidas.pop();
            tempStack.push(solicitud);
            std::cout << index << ". De: " << solicitud->emisor->nombres << " " << solicitud->emisor->apellidos
                      << " (" << solicitud->emisor->correo << ")\n";
            index++;
        }

        // Rechazar solicitud seleccionada
        int seleccion;
        std::cout << "Ingrese el número de la solicitud que desea rechazar (o 0 para no rechazar ninguna): ";
        std::cin >> seleccion;

        if (seleccion > 0 && seleccion < index) {
            for (int i = 1; i < seleccion; i++) {
                usuario->solicitudesRecibidas.push(tempStack.top());
                tempStack.pop();
            }

            Solicitud* solicitudARechazar = tempStack.top();
            tempStack.pop();
            eliminarSolicitudEnviada(solicitudARechazar->emisor, usuario);
            delete solicitudARechazar;
            std::cout << "Solicitud de amistad rechazada.\n";
        }

        // Volver a llenar la pila original con las solicitudes no rechazadas
        while (!tempStack.empty()) {
            usuario->solicitudesRecibidas.push(tempStack.top());
            tempStack.pop();
        }
    }


    // Eliminar una solicitud de la lista de solicitudes enviadas
    void eliminarSolicitudEnviada(Usuario* emisor, Usuario* receptor) {
        Solicitud* actual = emisor->solicitudesEnviadas;
        Solicitud* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->emisor == receptor) {
                if (anterior == nullptr) {
                    emisor->solicitudesEnviadas = actual->siguiente;
                } else {
                    anterior->siguiente = actual->siguiente;
                }
                delete actual;
                break;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
    }

    


    // Eliminar cuenta de usuario
    void eliminarUsuario(const std::string& correo) {
        if (cabeza == nullptr) return;

        if (cabeza->correo == correo) {
            Usuario* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
            std::cout << "Cuenta eliminada exitosamente.\n";
            mostrarUsuarios();
            return;
        }

        Usuario* actual = cabeza;
        while (actual->siguiente != nullptr) {
            if (actual->siguiente->correo == correo) {
                Usuario* temp = actual->siguiente;
                actual->siguiente = actual->siguiente->siguiente;
                delete temp;
                std::cout << "Cuenta eliminada exitosamente.\n";
                mostrarUsuarios();
                return;
            }
            actual = actual->siguiente;
        }

        std::cout << "Correo no encontrado.\n";
    }

    // Enviar solicitud de amistad
    void enviarSolicitudAmistad(const std::string& correoEmisor, const std::string& correoReceptor) {
        Usuario* emisor = obtenerUsuario(correoEmisor);
        Usuario* receptor = obtenerUsuario(correoReceptor);

        if (!emisor || !receptor) {
            std::cout << "Usuario no encontrado.\n";
            return;
        }

        // Verificar si ya existe una solicitud pendiente en ambas direcciones
        if (solicitudPendiente(emisor, receptor) || solicitudPendiente(receptor, emisor)) {
            std::cout << "Ya existe una solicitud pendiente entre estos usuarios.\n";
            return;
        }

        // Crear la solicitud
        Solicitud* nuevaSolicitud = new Solicitud{emisor, nullptr};

        // Agregar a la lista de solicitudes enviadas del emisor
        nuevaSolicitud->siguiente = emisor->solicitudesEnviadas;
        emisor->solicitudesEnviadas = nuevaSolicitud;

        // Agregar a la pila de solicitudes recibidas del receptor
        receptor->solicitudesRecibidas.push(nuevaSolicitud);

        std::cout << "Solicitud de amistad enviada.\n";
    }

    // Verificar si existe una solicitud pendiente
    bool solicitudPendiente(Usuario* emisor, Usuario* receptor) {
        Solicitud* actual = emisor->solicitudesEnviadas;
        while (actual != nullptr) {
            if (actual->emisor == receptor) {
                return true;
            }
            actual = actual->siguiente;
        }
        return false;
    }

    // Mostrar el estado actual de la lista de usuarios
    void mostrarUsuarios() {
        if (cabeza == nullptr) {
            std::cout << "No hay usuarios registrados.\n";
            return;
        }

        Usuario* actual = cabeza;
        std::cout << "Lista de usuarios:\n";
        while (actual != nullptr) {
            std::cout << "Nombres: " << actual->nombres << ", Apellidos: " << actual->apellidos
                      << ", Correo: " << actual->correo << ", Fecha de Nacimiento: " << actual->fechaNacimiento << "\n";
            actual = actual->siguiente;
        }
    }





    // Destructor para limpiar la lista
    ~ListaUsuarios() {
        Usuario* actual = cabeza;
        while (actual != nullptr) {
            Usuario* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
};

