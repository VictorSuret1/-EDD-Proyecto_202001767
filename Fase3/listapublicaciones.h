#ifndef LISTAPUBLICACIONES_H
#define LISTAPUBLICACIONES_H
#include <QString>
#include <QDateTime>
#include "arbolb.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

struct Publicacion {
    QString correoUsuario;
    QString contenido;
    QDateTime fechaHora;
    QString rutaImagen;
    ArbolB arbolComentarios;    // Árbol B para manejar los comentarios
    Publicacion* siguiente;
    Publicacion* anterior;

    Publicacion(const QString& correo, const QString& texto, const QString& ruta = "", const QDateTime& fecha = QDateTime())
        : correoUsuario(correo), contenido(texto), fechaHora(fecha), rutaImagen(ruta), siguiente(nullptr), anterior(nullptr) {}


    std::string toJson() const {
        try {
            json j;
            j["correo"] = correoUsuario.toStdString();
            j["contenido"] = contenido.toStdString();
            j["fecha"] = fechaHora.date().toString("dd/MM/yyyy").toStdString();
            j["hora"] = fechaHora.time().toString("hh:mm:ss").toStdString();

            // Verifica si `rutaImagen` no está vacío antes de agregarlo
            if (!rutaImagen.isEmpty()) {
                j["imagen"] = rutaImagen.toStdString();
            } else {
                j["imagen"] = "";  // Si está vacío, agregar una cadena vacía
            }

            qDebug() << "Publicación generada para JSON:" << QString::fromStdString(j.dump());

            // Añadir comentarios si existen en el árbol B
            std::vector<json> comentariosJson;
            arbolComentarios.recorrerComentarios([&](const Comentario& comentario) {
                comentariosJson.push_back(json::parse(comentario.toJson()));
            });
            j["comentarios"] = comentariosJson;

            return j.dump();
        } catch (const std::exception& e) {
            qDebug() << "Error en toJson de Publicacion: " << e.what();
            return "{}";  // Devuelve un JSON vacío si ocurre un error
        }
    }

};

class ListaPublicaciones {
public:
    ListaPublicaciones();
    ~ListaPublicaciones();

    void agregarPublicacion(const QString& correo, const QString& contenido, const QString& rutaImagen, const QDateTime& fechaHora);
    void mostrarPublicaciones() const;
    bool estaVacia() const;

    Publicacion* getCabeza() const;
    std::string toJson() const;
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
