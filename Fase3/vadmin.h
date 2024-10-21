#ifndef VADMIN_H
#define VADMIN_H

#include <QDialog>
#include "arbolavl.h"
#include <QScrollArea>
#include "blockchain.h"

extern Blockchain blockchain;
namespace Ui {
class vAdmin;
}

class vAdmin : public QDialog
{
    Q_OBJECT

public:
    explicit vAdmin(QWidget *parent = nullptr);
    ~vAdmin();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();



    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void llenarTablaUsuarios(NodoAVL* nodo);
    void modificarUsuario(const QString& correo);
    void eliminarUsuario(const QString& correo);
    void actualizarTabla();
    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void generarMerkleTreeUltimoBloque();

    void mostrarBloquesEnScrollArea(QScrollArea* scrollArea, Blockchain& blockchain);

    void on_pushButton_10_clicked();

private:
    Ui::vAdmin *ui;
};

#endif // VADMIN_H
