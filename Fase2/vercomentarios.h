#ifndef VERCOMENTARIOS_H
#define VERCOMENTARIOS_H

#include <QDialog>
#include <QListWidget>
#include "arbolb.h"

namespace Ui {
class VerComentarios;
}

class VerComentarios : public QDialog
{
    Q_OBJECT

public:
    explicit VerComentarios(ArbolB *arbolComentarios, QWidget *parent = nullptr);
    ~VerComentarios();

private:
    Ui::VerComentarios *ui;
    void mostrarComentarios(ArbolB *arbolComentarios); // Función para mostrar comentarios
};

#endif // VERCOMENTARIOS_H
