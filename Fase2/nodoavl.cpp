#ifndef NODOAVL_H
#define NODOAVL_H

#include <QString>

struct Usuario {
    QString nombres;
    QString apellidos;
    QString fechaNacimiento;
    QString correo;
    QString contrasena;

    Usuario() {} // Constructor por defecto
    Usuario(QString nombres, QString apellidos, QString fechaNacimiento, QString correo, QString contrasena)
        : nombres(nombres), apellidos(apellidos), fechaNacimiento(fechaNacimiento), correo(correo), contrasena(contrasena) {}
};

struct NodoAVL {
    Usuario usuario;        // Información del usuario almacenada en el nodo
    NodoAVL* izquierdo;     // Puntero al nodo hijo izquierdo
    NodoAVL* derecho;       // Puntero al nodo hijo derecho
    int altura;             // Altura del nodo en el árbol

    // Constructor para inicializar un nodo con un usuario dado
    NodoAVL(const Usuario& usuario)
        : usuario(usuario), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

#endif // NODOAVL_H
