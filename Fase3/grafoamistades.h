#ifndef GRAFOAMISTADES_H
#define GRAFOAMISTADES_H

#include <QString>
#include <QMap>
#include <QDebug>
#include "adjacencylist.h"

class GrafoAmistades
{
public:
    GrafoAmistades();
    void agregarUsuario(const QString& usuario);
    void agregarAmistad(const QString& usuario1, const QString& usuario2);
    void eliminarAmistad(const QString& usuario1, const QString& usuario2);
    void eliminarUsuario(const QString& usuario);
    void mostrarAmistades() const;
    void mostrarGrafo() const;
    bool sonAmigos(const QString& usuario1, const QString& usuario2) const;
    QList<QString> obtenerAmigos(const QString& usuario) const;
    void graficarListaAdyacencia(const std::string& fileName) const;
    void graficarGrafo(const std::string& fileName) const;
    QList<QPair<QString, int>> sugerenciasAmistad(const QString& usuario) const;

    void graficarSugerenciasAmistad(const QString& usuario, const std::string& fileName) const;

private:
    QMap<QString, AdjacencyList*> listaAdyacencia;
};

#endif // GRAFOAMISTADES_H
