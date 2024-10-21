#ifndef NUEVAPUBLICACION_H
#define NUEVAPUBLICACION_H

#include <QDialog>

namespace Ui {
class NuevaPublicacion;
}

class NuevaPublicacion : public QDialog
{
    Q_OBJECT

public:
    explicit NuevaPublicacion(QWidget *parent = nullptr);
    ~NuevaPublicacion();

    QString getTextoPublicacion() const;
    QString getRutaImagen() const;

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::NuevaPublicacion *ui;
    void seleccionarImagen();
    void aceptarPublicacion();
};

#endif // NUEVAPUBLICACION_H
