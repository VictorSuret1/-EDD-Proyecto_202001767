

#include <nlohmann/json.hpp>
#include <fstream>
#include "matrix/headers/matrix.h"
#include "ListaPublicaciones.cpp"


using json = nlohmann::json;
 extern Matrix matrizAmistades;
 extern ListaPublicaciones listaPubli;
 extern ListaCircularPublicaciones circu;

class ListaUsuarios {
private:
    Usuario* cabeza;

public:
    ListaUsuarios() : cabeza(nullptr) {}


    void mostrarRelacionesDeAmistad(Matrix& matriz) {
    ListNode* rowNode = matriz.getRowHeader()->head;

    std::cout << "--- Relaciones de Amistad ---\n";
    
    // Recorremos cada fila de la matriz
    while (rowNode != nullptr) {
        std::string usuario1 = matriz.getNombrePorIndice(rowNode->position);
        MatrixNode* currentMatrixNode = rowNode->access;

        // Verificamos las relaciones de amistad para usuario1
        while (currentMatrixNode != nullptr) {
            std::string usuario2 = matriz.getNombrePorIndice(currentMatrixNode->col);

            // Si existe un nodo en la matriz, usuario1 y usuario2 son amigos
            std::cout << usuario1 << " es amigo de " << usuario2 << "\n";

            currentMatrixNode = currentMatrixNode->right;
        }

        rowNode = rowNode->next;
    }

    std::cout << "--- Fin de las Relaciones de Amistad ---\n";
}



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

        mostrarMenuAdmin();
        
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

void cargarMasivaPublicaciones(const std::string& path) {
    std::ifstream archivo(path);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << path << std::endl;
        return;
    }

    nlohmann::json jsonData;
    archivo >> jsonData;

    for (const auto& item : jsonData) {
        std::string correo = item["correo"];
        std::string contenido = item["contendio"];
        std::string fecha = item["fecha"];
        std::string hora = item["hora"];

            Publicacion* nuevaPublicacion = new Publicacion(correo, contenido);
            nuevaPublicacion->fecha = fecha;
            nuevaPublicacion->hora = hora;
            listaPubli.agregarPublicacion(correo, contenido);
            std::cout << "Publicación de " << correo << " cargada exitosamente." << std::endl;

    }
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
                matrizAmistades.insertarAmistad(correoEmisor,correoReceptor);
                std::cout << "Solicitud de amistad aceptada: " << correoEmisor << " -> " << correoReceptor << " (pendiente de implementación)\n";
            }
        } else {
            std::cout << "Error: JSON malformado. Faltan campos requeridos para una solicitud.\n";
        }
    }

    std::cout << "Carga masiva de solicitudes completada.\n";
}

void graficarListaUsuarios(Usuario* cabeza) {
    std::ofstream file("lista_usuarios.dot");
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo para escribir el grafo.\n";
        return;
    }

    file << "digraph G {\n";
    file << "    node [shape=record];\n";

    Usuario* actual = cabeza;
    int index = 0;

    while (actual != nullptr) {
        // Crear un identificador único para cada nodo basado en su posición en la lista
        std::string nodeName = "node" + std::to_string(index);

        // Escribir la información del nodo en el archivo
        file << "    " << nodeName << " [label=\"{" 
             << "Nombres: " << actual->nombres << " | "
             << "Apellidos: " << actual->apellidos << " | "
             << "Correo: " << actual->correo << " | "
             << "Fecha de Nacimiento: " << actual->fechaNacimiento << " | "
             << "Puntero siguiente: " << (actual->siguiente ? "node" + std::to_string(index + 1) : "nullptr")
             << "}\"];\n";

        // Si hay un siguiente nodo, crear la conexión
        if (actual->siguiente != nullptr) {
            file << "    " << nodeName << " -> " << "node" << (index + 1) << ";\n";
        }

        actual = actual->siguiente;
        index++;
    }

    file << "}\n";
    file.close();

    std::cout << "Archivo DOT generado: lista_usuarios.dot\n";
}

void graficarListaPublicaciones(ListaPublicaciones& lista) {
    std::ofstream archivo("grafoPublicaciones.dot");
    
    if (archivo.is_open()) {
        archivo << "digraph G {" << std::endl;
        archivo << "node [shape=record];" << std::endl;

        Publicacion* actual = lista.cabeza;
        int i = 0;

        // Crear nodos y enlaces
        while (actual != nullptr) {
            archivo << "nodo" << i << " [label=\"{Correo: " << actual->correoUsuario 
                    << " | Contenido: " << actual->contenido 
                    << " | Fecha: " << actual->fecha 
                    << " | Hora: " << actual->hora << "}\"];" << std::endl;

            if (actual->siguiente != nullptr) {
                archivo << "nodo" << i << " -> nodo" << i + 1 << " [dir=both];" << std::endl;
            }

            actual = actual->siguiente;
            i++;
        }

        archivo << "}" << std::endl;
        archivo.close();
    } else {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
    }

    // Generar imagen con Graphviz
    system("dot -Tpng grafoPublicaciones.dot -o grafoPublicaciones.png");
}

    void mostrarMenuAdmin(){
        int opcion;
        std::string path;
        std::string use;
        do {
            std::cout << "\n--- Menu de Administrador ---\n";
            std::cout << "1. Carga de Usuarios\n";
            std::cout << "2. Carga de Relaciones\n";
            std::cout << "3. Carga de Publicaciones\n";
            std::cout << "4. Gestionar usuarios\n";
            std::cout << "5. Reportes\n";
            std::cout << "6. Salir\n";
            std::cout << "Seleccione una opción: ";
            std::cin >> opcion;

            switch(opcion){
                case 1:
                    
                    std::cout << "Ingrese el path del archivo JSON para la carga masiva: ";
                    std::cin.ignore();
                    std::getline(std::cin, path);
                    cargarUsuariosDesdeJSON(path);
                    break;
                case 2:
                    std::cout << "Ingrese el path del archivo JSON para la carga masiva: ";
                    std::cin.ignore();
                    std::getline(std::cin, path);
                    cargarSolicitudesDesdeJSON(path);
                    break;
                case 3:
                    std::cout << "Ingrese el path del archivo JSON para la carga masiva: ";
                    std::cin.ignore();
                    std::getline(std::cin, path);
                    cargarMasivaPublicaciones(path);
                    break;
                case 4:
                    mostrarRelacionesDeAmistad(matrizAmistades);
                    std::cout << "Ingrese el usuario a eliminar: ";
                    std:: cin.ignore();
                    std::getline(std::cin, use);
                    eliminarUsuario(use);

                    break;
                case 5:
                    graficarListaUsuarios(cabeza);
                    graficarListaPublicaciones(listaPubli);
                    matrizAmistades.create_dot();
                    graficarTopUsuarios(listaPubli,matrizAmistades,*this);
                    break;
                case 6:
                    std::cout << "Cerrando sesión...\n";
                    break;
                default:
                    break;
            }
        }while (opcion != 6);
    }



    void mostrarPerfilUsuario(Usuario* usuario) {
    if (usuario == nullptr) {
        std::cout << "No hay ningún usuario en sesión.\n";
        return;
    }

    std::cout << "\n--- Perfil del Usuario ---\n";
    std::cout << "Nombres: " << usuario->nombres << "\n";
    std::cout << "Apellidos: " << usuario->apellidos << "\n";
    std::cout << "Fecha de Nacimiento: " << usuario->fechaNacimiento << "\n";
    std::cout << "Correo: " << usuario->correo << "\n";
    std::cout << "--------------------------------\n";
}


void menuPublicaciones(ListaPublicaciones& lista, Usuario* usuario, Matrix& matrizAmistades) {
    int opcion;
    do {
        std::cout << "1. Crear Publicación" << std::endl;
        std::cout << "2. Ver Publicaciones" << std::endl;
        std::cout << "3. Salir" << std::endl;
        std::cin >> opcion;
        std::cin.ignore(); 

        if (opcion == 1) {
            std::string contenido;
            std::cout << "Escribe tu publicación: ";
            std::getline(std::cin, contenido);
            lista.agregarPublicacion(usuario->correo, contenido);
        } else if (opcion == 2) {
            
            // Llamar a verPublicaciones para mostrar las publicaciones filtradas
            verPublicaciones(usuario, lista, matrizAmistades);
        }
    } while (opcion != 3);
}



void filtrarPublicacionesParaUsuario(ListaPublicaciones& listaPublicaciones, Usuario* usuarioLogeado, ListaCircularPublicaciones& listaFiltrada, Matrix& matrizAmistades) {
    Publicacion* actual = listaPublicaciones.getCabeza();

    while (actual != nullptr) {
        // Verificar si la publicación pertenece al usuario logeado o a un amigo
        if (actual->correoUsuario == usuarioLogeado->correo || matrizAmistades.sonAmigos(usuarioLogeado->correo, actual->correoUsuario)) {
            // Insertar la publicación en la lista circular doblemente enlazada
            listaFiltrada.insertarPublicacion(actual);
        }

        actual = actual->siguiente;
    }
}

void verPublicaciones(Usuario* usuarioLogeado, ListaPublicaciones& listaPublicaciones, Matrix& matrizAmistades) {
    ListaCircularPublicaciones listaFiltrada;
    
    // Filtrar publicaciones permitidas para el usuario logeado en cada llamada
    filtrarPublicacionesParaUsuario(listaPublicaciones, usuarioLogeado, listaFiltrada, matrizAmistades);

    // Explorar las publicaciones filtradas
    listaFiltrada.explorarPublicaciones();
}


std::unordered_map<std::string, int> contarPublicacionesPorUsuario(ListaPublicaciones& listaPublicaciones) {
    std::unordered_map<std::string, int> conteo;

    Publicacion* actual = listaPublicaciones.getCabeza();
    while (actual != nullptr) {
        conteo[actual->correoUsuario]++;
        actual = actual->siguiente;
    }

    return conteo;
}

std::unordered_map<std::string, int> contarAmigosPorUsuario(Matrix& matrizAmistades, ListaUsuarios& listaUsuarios) {
    std::unordered_map<std::string, int> conteo;

    Usuario* actual = cabeza;
    while (actual != nullptr) {
        int numAmigos = 0;
        MatrixNode* nodoAmistad = matrizAmistades.getRowHeader()->head->access;

        while (nodoAmistad != nullptr) {
            if (nodoAmistad->col == matrizAmistades.obtenerIndice(actual->correo)) {
                numAmigos++;
            }
            nodoAmistad = nodoAmistad->right;
        }

        conteo[actual->correo] = numAmigos;
        actual = actual->siguiente;
    }

    return conteo;
}

std::vector<std::pair<std::string, int>> obtenerTopUsuarios(const std::unordered_map<std::string, int>& conteo, bool ascendente = false) {
    std::vector<std::pair<std::string, int>> usuarios(conteo.begin(), conteo.end());

    if (ascendente) {
        std::sort(usuarios.begin(), usuarios.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
    } else {
        std::sort(usuarios.begin(), usuarios.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
    }

    if (usuarios.size() > 5) {
        usuarios.resize(5);
    }

    return usuarios;
}

void generarGraficoUsuarios(const std::vector<std::pair<std::string, int>>& usuarios, const std::string& nombreArchivo, const std::string& titulo) {
    std::ofstream file(nombreArchivo + ".dot");

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo para escribir el grafo.\n";
        return;
    }

    file << "digraph G {\n";
    file << "    label=\"" << titulo << "\";\n";
    file << "    node [shape=box];\n";

    for (const auto& [usuario, cantidad] : usuarios) {
        file << "    \"" << usuario << "\" [label=\"" << usuario << "\\n" << cantidad << "\"];\n";
    }

    file << "}\n";
    file.close();

    std::cout << "Archivo DOT generado: " << nombreArchivo << ".dot\n";
}

void graficarTopUsuarios(ListaPublicaciones& listaPublicaciones, Matrix& matrizAmistades, ListaUsuarios& listaUsuarios) {
    auto conteoPublicaciones = contarPublicacionesPorUsuario(listaPublicaciones);
    auto conteoAmigos = contarAmigosPorUsuario(matrizAmistades, listaUsuarios);

    auto top5Publicaciones = obtenerTopUsuarios(conteoPublicaciones);
    auto top5MenosAmigos = obtenerTopUsuarios(conteoAmigos, true);

    generarGraficoUsuarios(top5Publicaciones, "top5_publicaciones", "Top 5 Usuarios con Más Publicaciones");
    generarGraficoUsuarios(top5MenosAmigos, "top5_menos_amigos", "Top 5 Usuarios con Menos Amigos");
}

    void mostrarMenuUsuario(Usuario* usuario) {
    int opcion;
    int opcion2;
    do {
        std::cout << "\n--- Menu de Usuario ---\n";
        std::cout << "1. Perfil\n";
        std::cout << "2. Ver Solicitudes de Amistad\n";
        std::cout << "3. Enviar Solicitud de Amistad\n";
        std::cout << "4. Publicaciones\n";
        std::cout << "5. Cerrar Sesión\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "1. ver perfil ";
                std::cout << "2. eliminar cuenta ";
                std::cin >> opcion;
                switch(opcion){
                    case 1 :
                        mostrarPerfilUsuario(usuario);
                        break;
                    case 2:
                        std::cout << "seguro de borrar cuenta?";
                        std::cout << "1. si";
                        std::cout << "2. no";
                        std::cin >> opcion2;
                        if(opcion2 == 1){
                            eliminarUsuario(usuario->correo);
                        }else{
                            break;
                        }
                        
                }
                
            case 2:
                verSolicitudesAmistad(usuario);
                break;

            case 3: {
                std::string correoReceptor;
                std::cout << "Ingrese el correo del usuario al que quiere enviar la solicitud: ";
                std::cin.ignore();
                std::getline(std::cin, correoReceptor);
                enviarSolicitudAmistad(usuario->correo, correoReceptor);
                break;
            }
            case 4:
                std::cout << "Publicaciones \n";

                
                menuPublicaciones(listaPubli,usuario,matrizAmistades);

                
                break;
            case 5:
                std::cout << "Cerrando sesión...\n";
                break;
            default:
                std::cout << "Opción no válida.\n";
                break;
        }
    } while (opcion != 5);
}

    // Ver las solicitudes de amistad recibidas y aceptar o rechazar alguna
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

    // Aceptar o rechazar solicitud seleccionada
    int seleccion;
    std::cout << "Ingrese el número de la solicitud (0 para volver):  ";
    std::cin >> seleccion;

    if (seleccion > 0 && seleccion < index) {
        for (int i = 1; i < seleccion; i++) {
            usuario->solicitudesRecibidas.push(tempStack.top());
            tempStack.pop();
        }

        Solicitud* solicitudSeleccionada = tempStack.top();
        tempStack.pop();

        int opcion;
        std::cout << "¿Qué desea hacer con la solicitud?\n";
        std::cout << "1. Aceptar\n";
        std::cout << "2. Rechazar\n";
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;

        if (opcion == 1) {
            // Aceptar la solicitud
                   
            eliminarSolicitudEnviada(solicitudSeleccionada->emisor, usuario);
            matrizAmistades.insertarAmistad(solicitudSeleccionada->emisor->correo, usuario->correo);
            
            std::cout << "Solicitud de amistad aceptada.\n";
        } else if (opcion == 2) {
            // Rechazar la solicitud
            eliminarSolicitudEnviada(solicitudSeleccionada->emisor, usuario);
            delete solicitudSeleccionada;
            std::cout << "Solicitud de amistad rechazada.\n";
        } else {
            std::cout << "Opción no válida. La solicitud no se ha procesado.\n";
            usuario->solicitudesRecibidas.push(solicitudSeleccionada); // Reinsertar la solicitud si no se procesa
        }
    }

    // Volver a llenar la pila original con las solicitudes no procesadas
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

