#ifndef ARBOLABB_H
#define ARBOLABB_H
#include <QString>
#include <QDate>
#include "listapublicaciones.h"

class ArbolABB {
public:
    ArbolABB();
    ~ArbolABB();
    NodoABB* raiz;
    void insertarPublicacion(const QString& correo, const QString& contenido, const QString& rutaImagen, const QDate& fecha);
    void mostrarPublicacionesOrdenadas() const;
    NodoABB* buscar(const QDate& fecha) const;
    void mostrarPublicaciones() const;
    NodoABB* obtenerRaiz() const;

    void preOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones);
    void postOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones);
    void inOrden(NodoABB* nodo, std::vector<Publicacion*>& publicaciones);

private:


    NodoABB* insertarNodo(NodoABB* nodo, const QDate& fecha);
    void insertarEnLista(NodoABB* nodo, Publicacion* nuevaPublicacion);
    void mostrarEnOrden(NodoABB* nodo) const;
    void limpiar(NodoABB* nodo); // Función para limpiar el árbol
    NodoABB* buscarNodo(NodoABB* nodo, const QDate& fecha) const;// Función recursiva de búsqueda
    void mostrarPublicacionesRecursivo(NodoABB* nodo) const;
};
#endif // ARBOLABB_H
