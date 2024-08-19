#include "ListaUsuarios.cpp"

int main() {
    ListaUsuarios listaUsuarios;
    int opcion;
    std::string nombres, apellidos, fechaNacimiento, correo, contrasena;

    do {
        std::cout << "1. Registrar Usuario\n";
        std::cout << "2. Iniciar Sesion\n";
        std::cout << "3. Eliminar Cuenta\n";
        std::cout << "4. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                std::cout << "Ingrese nombres: ";
                std::cin.ignore();
                std::getline(std::cin, nombres);
                std::cout << "Ingrese apellidos: ";
                std::getline(std::cin, apellidos);
                std::cout << "Ingrese fecha de nacimiento (dd/mm/aaaa): ";
                std::getline(std::cin, fechaNacimiento);
                std::cout << "Ingrese correo: ";
                std::getline(std::cin, correo);
                std::cout << "Ingrese contraseña: ";
                std::getline(std::cin, contrasena);
                listaUsuarios.registrarUsuario(nombres, apellidos, fechaNacimiento, correo, contrasena);
                break;

            case 2:
                std::cout << "Ingrese correo: ";
                std::cin.ignore();
                std::getline(std::cin, correo);
                std::cout << "Ingrese contraseña: ";
                std::getline(std::cin, contrasena);
                listaUsuarios.iniciarSesion(correo, contrasena);
                break;

            case 3:
                std::cout << "Ingrese correo: ";
                std::cin.ignore();
                std::getline(std::cin, correo);
                listaUsuarios.eliminarUsuario(correo);
                break;

            case 4:
                std::cout << "Saliendo...\n";
                break;

            default:
                std::cout << "Opcion no valida.\n";
                break;
        }
    } while (opcion != 4);

    return 0;
}