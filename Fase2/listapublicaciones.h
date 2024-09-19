#ifndef LISTAPUBLICACIONES_H
#define LISTAPUBLICACIONES_H
#include <QString>
#include <QDateTime>
#include "arbolb.h"

struct Publicacion {
    QString correoUsuario;
    QString contenido;
    QDateTime fechaHora;
    QString rutaImagen;
    ArbolB arbolComentarios;  // Árbol B para manejar los comentarios
    Publicacion* siguiente;
    Publicacion* anterior;

    Publicacion(const QString& correo, const QString& texto, const QString& ruta = "", const QDateTime& fecha = QDateTime())
        : correoUsuario(correo), contenido(texto), rutaImagen(ruta), fechaHora(fecha), siguiente(nullptr), anterior(nullptr) {}
};

class ListaPublicaciones {
public:
    ListaPublicaciones();
    ~ListaPublicaciones();

    void agregarPublicacion(const QString& correo, const QString& contenido, const QString& rutaImagen, const QDateTime& fechaHora);
    void mostrarPublicaciones() const;
    bool estaVacia() const;

    Publicacion* getCabeza() const;

private:
    Publicacion* cabeza;
    Publicacion* cola;

    void limpiar(); // Limpia la lista al destruir el objeto
};




struct NodoABB {
    QDate fecha; // Usar QDate para comparar fechas sin considerar la hora
    Publicacion* listaPublicaciones; // Lista simplemente enlazada de publicaciones
    NodoABB* izquierda;
    NodoABB* derecha;

    NodoABB(const QDate& fecha)
        : fecha(fecha), listaPublicaciones(nullptr), izquierda(nullptr), derecha(nullptr) {}
};



#endif // LISTAPUBLICACIONES_H
