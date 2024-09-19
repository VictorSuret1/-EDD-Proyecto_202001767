#ifndef ARBOLAVL_H
#define ARBOLAVL_H

#include "nodoavl.h"
#include "matrix.h"

class ArbolAVL {
public:
    NodoAVL* raiz;
    ArbolAVL();
    ~ArbolAVL();

    void insertar(const Usuario& usuario);
    NodoAVL* buscar(const QString& correo);
    NodoAVL* buscarRecursivo(NodoAVL* nodo, const QString& correo);
    void eliminar(const QString& correo);
    void mostrarUsuarios();


    void enviarSolicitud(const QString& correoEmisor, const QString& correoReceptor);
    void aceptarSolicitud(const QString& correoEmisor, const QString& correoReceptor, Matrix& matrizAmistades);
    void rechazarSolicitud(const QString& correoEmisor, const QString& correoReceptor);
    void eliminarSolicitud(const QString& correoEmisor, const QString& correoReceptor);
    NodoAVL* obtenerRaiz();

    QList<Usuario> obtenerRecorrido(const QString& tipoRecorrido);
    void exportarArbolAVLaDot(ArbolAVL& arbol, const std::string& rutaArchivo);
private:


    NodoAVL* insertarNodo(NodoAVL* nodo, const Usuario& usuario);
    NodoAVL* balancear(NodoAVL* nodo);
    int altura(NodoAVL* nodo);
    int factorBalance(NodoAVL* nodo);
    NodoAVL* rotarDerecha(NodoAVL* y);
    NodoAVL* rotarIzquierda(NodoAVL* x);
    NodoAVL* minimoValorNodo(NodoAVL* nodo);
    NodoAVL* eliminarNodo(NodoAVL* nodo, const QString& correo);
    NodoAVL* buscarNodo(NodoAVL* nodo, const QString& correo);
    void liberarMemoria(NodoAVL* nodo);

    void mostrarUsuariosInOrder(NodoAVL* nodo);

    void recorrerPreOrden(NodoAVL* nodo, QList<Usuario>& usuarios);
    void recorrerInOrden(NodoAVL* nodo, QList<Usuario>& usuarios);
    void recorrerPostOrden(NodoAVL* nodo, QList<Usuario>& usuarios);
};

#endif // ARBOLAVL_H
