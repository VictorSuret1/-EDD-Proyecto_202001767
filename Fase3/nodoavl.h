#ifndef NODOAVL_H
#define NODOAVL_H

#include <QString>
#include <QString>
#include <QList>
#include <QDate>

struct Solicitud {
    QString correoEmisor;
    QString correoReceptor;
    QString estado; // "PENDIENTE", "ACEPTADA", "RECHAZADA"
};

struct Usuario {
    QString nombres;
    QString apellidos;
    QString fechaNacimiento;
    QString correo;
    QString contrasena;
    QList<Solicitud*> solicitudesEnviadas; // Lista de solicitudes enviadas
    QList<Solicitud*> solicitudesRecibidas; // Lista de solicitudes recibidas

    Usuario() {}
    Usuario(QString n, QString a, QString f, QString c, QString p)
        : nombres(n), apellidos(a), fechaNacimiento(f), correo(c), contrasena(p) {}
};

struct NodoAVL {
    Usuario usuario;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    NodoAVL(const Usuario& usuario)
        : usuario(usuario), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};




#endif // NODOAVL_H
