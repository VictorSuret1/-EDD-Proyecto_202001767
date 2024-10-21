#include "grafoamistades.h"
#include "adjacencylist.h"
#include <filesystem>
extern AdjacencyList adlist;

GrafoAmistades::GrafoAmistades() {}

void GrafoAmistades::agregarUsuario(const QString& usuario) {
    if (!listaAdyacencia.contains(usuario)) {
        listaAdyacencia[usuario] = new AdjacencyList();
    }
}

void GrafoAmistades::agregarAmistad(const QString& usuario1, const QString& usuario2) {
    if (!listaAdyacencia.contains(usuario1)) {
        agregarUsuario(usuario1);
    }
    if (!listaAdyacencia.contains(usuario2)) {
        agregarUsuario(usuario2);
    }

    listaAdyacencia[usuario1]->insert(usuario2);
    listaAdyacencia[usuario2]->insert(usuario1);
}

void GrafoAmistades::eliminarAmistad(const QString& usuario1, const QString& usuario2) {
    if (listaAdyacencia.contains(usuario1)) {
        listaAdyacencia[usuario1]->remove(usuario2);
    }
    if (listaAdyacencia.contains(usuario2)) {
        listaAdyacencia[usuario2]->remove(usuario1);
    }
}

void GrafoAmistades::eliminarUsuario(const QString& usuario) {
    if (listaAdyacencia.contains(usuario)) {
        // Eliminar todas las referencias al usuario en las listas de amigos
        for (auto it = listaAdyacencia.begin(); it != listaAdyacencia.end(); ++it) {
            it.value()->remove(usuario);
        }
        // Eliminar el usuario del grafo
        delete listaAdyacencia[usuario]; // Liberar la memoria
        listaAdyacencia.remove(usuario);
    }
}

void GrafoAmistades::mostrarAmistades() const {
    for (auto it = listaAdyacencia.begin(); it != listaAdyacencia.end(); ++it) {
        qDebug() << it.key() << "es amigo de:";
        it.value()->print(); 
    }
}

void GrafoAmistades::mostrarGrafo() const {
    qDebug() << "Grafo de Amistades - Lista de Adyacencias:";
    for (auto it = listaAdyacencia.begin(); it != listaAdyacencia.end(); ++it) {
        qDebug() << "Usuario:" << it.key() << "-> Amigos:";
        it.value()->print();
    }


}

bool GrafoAmistades::sonAmigos(const QString& usuario1, const QString& usuario2) const {
    if (listaAdyacencia.contains(usuario1)) {
        return listaAdyacencia[usuario1]->contains(usuario2);
    }
    return false;
}

QList<QString> GrafoAmistades::obtenerAmigos(const QString& usuario) const {
    if (listaAdyacencia.contains(usuario)) {
        return listaAdyacencia[usuario]->toList();
    }
    return QList<QString>();
}



void GrafoAmistades::graficarGrafo(const std::string& fileName) const {
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << fileName << std::endl;
        return;
    }

    outFile << "graph GrafoAmistades {" << std::endl;
    outFile << "  rankdir=LR;" << std::endl;
    outFile << "  node [shape=circle, style=filled, fillcolor=lightblue];" << std::endl;

    for (auto it = listaAdyacencia.begin(); it != listaAdyacencia.end(); ++it) {
        QString usuario = it.key();
        AdjacencyList* listaAmigos = it.value();

        // Nodo del usuario
        outFile << "  \"" << usuario.toStdString() << "\";" << std::endl;

        // Graficar todas las conexiones de los vecinos (amistades)
        node* amigo = listaAmigos->head;
        while (amigo) {
            if (usuario < amigo->value) { // Para evitar duplicar aristas
                outFile << "  \"" << usuario.toStdString() << "\" -- \"" << amigo->value.toStdString() << "\";" << std::endl;
            }
            amigo = amigo->next;
        }
    }

    outFile << "}" << std::endl;
    outFile.close();

    std::cout << "Archivo " << fileName << " generado correctamente. Usando Graphviz para generar la imagen." << std::endl;

    std::string command = "dot -Tpng " + fileName + " -o amistades_grafo.png";
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Imagen 'amistades_grafo.png' generada correctamente." << std::endl;
    } else {
        std::cerr << "Error al generar la imagen con Graphviz." << std::endl;
    }
}

void GrafoAmistades::graficarListaAdyacencia(const std::string& fileName) const {

    std::string dotFileName = fileName + ".dot";
    std::string pngFileName = fileName + ".png";


    if (std::filesystem::exists(dotFileName)) {
        std::filesystem::remove(dotFileName);
    }
    if (std::filesystem::exists(pngFileName)) {
        std::filesystem::remove(pngFileName);
    }


    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << fileName << std::endl;
        return;
    }

    outFile << "digraph ListaAdyacencia {" << std::endl;
    outFile << "  rankdir=LR;" << std::endl;
    outFile << "  node [shape=box, style=filled, fillcolor=lightyellow];" << std::endl;

    for (auto it = listaAdyacencia.begin(); it != listaAdyacencia.end(); ++it) {
        QString usuario = it.key();
        AdjacencyList* listaAmigos = it.value();

        outFile << "  \"" << usuario.toStdString() << "\" [label=\"Usuario: " << usuario.toStdString() << "\"]" << ";" << std::endl;

        // Crear nodos para cada amigo en una cadena lineal
        node* amigo = listaAmigos->head;
        std::string prevNodo = usuario.toStdString();
        int amigoIndex = 1;
        while (amigo) {
            std::string amigoNodo = usuario.toStdString() + "_amigo_" + std::to_string(amigoIndex);
            outFile << "  \"" << amigoNodo << "\" [label=\"Amigo: " << amigo->value.toStdString() << "\"]" << ";" << std::endl;
            outFile << "  \"" << prevNodo << "\" -> \"" << amigoNodo << "\";" << std::endl;
            prevNodo = amigoNodo;
            amigo = amigo->next;
            amigoIndex++;
        }
    }

    outFile << "}" << std::endl;
    outFile.close();

    std::cout << "Archivo " << fileName << " generado correctamente. Usando Graphviz para generar la imagen." << std::endl;

    std::string command = "dot -Tpng " + fileName + " -o lista_adyacencia.png";
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Imagen 'lista_adyacencia.png' generada correctamente." << std::endl;
    } else {
        std::cerr << "Error al generar la imagen con Graphviz." << std::endl;
    }
}


QList<QPair<QString, int>> GrafoAmistades::sugerenciasAmistad(const QString& usuario) const {
    QMap<QString, int> sugerencias;

    QList<QString> amigos = obtenerAmigos(usuario);

    // Obtener amigos de los amigos
    for (const QString& amigo : amigos) {
        QList<QString> amigosDeAmigo = obtenerAmigos(amigo);
        for (const QString& posibleSugerencia : amigosDeAmigo) {
            // Evitar incluir al usuario y sus amigos directos
            if (posibleSugerencia != usuario && !amigos.contains(posibleSugerencia)) {
                sugerencias[posibleSugerencia]++;
            }
        }
    }

    // Convertir el QMap a QList para ordenar
    QList<QPair<QString, int>> listaSugerencias;
    for (auto it = sugerencias.begin(); it != sugerencias.end(); ++it) {
        listaSugerencias.append(qMakePair(it.key(), it.value()));
    }
    // Ordenar las sugerencias por la cantidad de amigos en común (de mayor a menor)
    std::sort(listaSugerencias.begin(), listaSugerencias.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
        return a.second > b.second;
    });

    // Mostrar sugerencias para depuración
    qDebug() << "Sugerencias de amistad para el usuario:" << usuario;
    for (const auto& sugerencia : listaSugerencias) {
        qDebug() << "Usuario sugerido:" << sugerencia.first << ", Amigos en común:" << sugerencia.second;
    }

    return listaSugerencias;
}



void GrafoAmistades::graficarSugerenciasAmistad(const QString& usuario, const std::string& fileName) const {

    std::string dotFileName = fileName + ".dot";
    std::string pngFileName = fileName + ".png";

    // Delete existing files if they exist
    // Using C++17 filesystem
    if (std::filesystem::exists(dotFileName)) {
        std::filesystem::remove(dotFileName);
    }
    if (std::filesystem::exists(pngFileName)) {
        std::filesystem::remove(pngFileName);
    }


    // Obtener sugerencias de amistad para el usuario
    QList<QPair<QString, int>> sugerencias = sugerenciasAmistad(usuario);

    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << fileName << std::endl;
        return;
    }

    outFile << "graph SugerenciasAmistad {" << std::endl;
    outFile << "  rankdir=LR;" << std::endl; // Dirección del grafo (izquierda a derecha)
    outFile << "  node [shape=circle, style=filled];" << std::endl;

    // Nodo del usuario principal
    outFile << "  \"" << usuario.toStdString() << "\" [fillcolor=lightblue, style=filled];" << std::endl;

    // Graficar los amigos directos del usuario
    QList<QString> amigos = obtenerAmigos(usuario);
    for (const QString& amigo : amigos) {
        outFile << "  \"" << usuario.toStdString() << "\" -- \"" << amigo.toStdString() << "\" [color=black];" << std::endl;
    }

    // Graficar las sugerencias de amistad
    for (const auto& sugerencia : sugerencias) {
        QString sugerido = sugerencia.first;

        // Los amigos del usuario estarán conectados a las sugerencias
        for (const QString& amigo : amigos) {
            if (obtenerAmigos(amigo).contains(sugerido)) {
                outFile << "  \"" << amigo.toStdString() << "\" -- \"" << sugerido.toStdString() << "\";" << std::endl;
            }
        }

        // Nodo sugerido en color verde
        outFile << "  \"" << sugerido.toStdString() << "\" [fillcolor=green, style=filled];" << std::endl;
    }

    outFile << "}" << std::endl;
    outFile.close();

    std::cout << "Archivo " << fileName << " generado correctamente. Usando Graphviz para generar la imagen." << std::endl;

    // Generar la imagen utilizando Graphviz
    std::string command = "dot -Tpng " + fileName + " -o sugerencias_amistad.png";
    int result = std::system(command.c_str());
    if (result == 0) {
        std::cout << "Imagen 'sugerencias_amistad.png' generada correctamente." << std::endl;
    } else {
        std::cerr << "Error al generar la imagen con Graphviz." << std::endl;
    }
}
