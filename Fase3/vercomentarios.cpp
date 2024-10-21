#include "vercomentarios.h"
#include "ui_vercomentarios.h"

#include <QVBoxLayout>
VerComentarios::VerComentarios(ArbolB* arbolComentarios, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VerComentarios)
{
    ui->setupUi(this);
    mostrarComentarios(arbolComentarios);
}

VerComentarios::~VerComentarios()
{
    delete ui;
}

void VerComentarios::mostrarComentarios(ArbolB* arbolComentarios) {
    // Crear un QListWidget para mostrar los comentarios
    QListWidget* listaComentarios = new QListWidget(this);

    // Recorrer el árbol B para extraer los comentarios y mostrarlos en el QListWidget
    std::function<void(NodoB*)> recorrerArbol = [&](NodoB* nodo) {
        if (!nodo) return;

        // Mostrar comentarios en orden
        for (int i = 0; i < nodo->comentarios.size(); ++i) {
            if (!nodo->esHoja) {
                recorrerArbol(nodo->hijos[i]);
            }

            // Formatear el comentario
            QString comentario = QString("%1 %2: %3")
                                     .arg(nodo->comentarios[i].fechaHora.toString("dd/MM/yyyy hh:mm"))
                                     .arg(nodo->comentarios[i].correoUsuario)
                                     .arg(nodo->comentarios[i].contenido);

            // Agregar el comentario al QListWidget
            listaComentarios->addItem(comentario);
        }

        // Recorrer el último hijo si no es hoja
        if (!nodo->esHoja) {
            recorrerArbol(nodo->hijos[nodo->comentarios.size()]);
        }
    };

    // Iniciar el recorrido desde la raíz del árbol de comentarios
    recorrerArbol(arbolComentarios->raiz);

    // Configurar el layout y agregar el QListWidget
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(listaComentarios);
    setLayout(layout);
}
