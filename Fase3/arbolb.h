#ifndef ARBOLB_H
#define ARBOLB_H

#include <QDateTime>
#include <QString>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Comentario {
public:
    QString correoUsuario;
    QString contenido;
    QDateTime fechaHora;

    // Constructor por defecto
    Comentario() : correoUsuario(""), contenido(""), fechaHora(QDateTime::currentDateTime()) {}

    // Constructor completo
    Comentario(const QString& correo, const QString& texto, const QDateTime& fechaHora)
        : correoUsuario(correo), contenido(texto), fechaHora(fechaHora) {}

    // Nueva función toJson() para convertir Comentario a JSON
    std::string toJson() const {
        json j;
        j["correo"] = correoUsuario.toStdString();
        j["contenido"] = contenido.toStdString();
        j["fecha"] = fechaHora.date().toString("dd/MM/yyyy").toStdString();
        j["hora"] = fechaHora.time().toString("hh:mm:ss").toStdString();
        return j.dump();
    }

};

struct NodoB {
    bool esHoja;
    std::vector<Comentario> comentarios;
    std::vector<NodoB*> hijos;

    NodoB(bool esHoja) : esHoja(esHoja) {}
};


class ArbolB {
public:
    ArbolB();
    ~ArbolB();

    NodoB* raiz;
    void insertarComentario(const QString& usuario, const QString& texto, const QDateTime& fechaHora);
    void mostrarComentariosOrdenados() const;
    void mostrarComentarios() const;
    void graficarArbolB(ArbolB* arbol, const QString& filename) const;
    void recorrerComentarios(const std::function<void(const Comentario&)>& accion) const;


private:


    void insertarEnNodo(NodoB* nodo, const Comentario& comentario);
    void dividirNodo(NodoB* nodo, int i, NodoB* hijo);
    void mostrarEnOrden(NodoB* nodo) const;
    void limpiar(NodoB* nodo);  // Limpiar la memoria
    void mostrarComentariosRecursivo(NodoB* nodo) const;
    void recorrerComentariosRecursivo(NodoB* nodo, const std::function<void(const Comentario&)>& accion) const;

};

#endif // ARBOLB_H
