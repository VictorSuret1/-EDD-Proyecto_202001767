#ifndef VERARBOLCOMENTARIOS_H
#define VERARBOLCOMENTARIOS_H

#include <QDialog>

namespace Ui {
class VerArbolComentarios;
}

class VerArbolComentarios : public QDialog
{
    Q_OBJECT

public:
    explicit VerArbolComentarios(QWidget *parent = nullptr);
    ~VerArbolComentarios();

private:
    Ui::VerArbolComentarios *ui;
};

#endif // VERARBOLCOMENTARIOS_H
