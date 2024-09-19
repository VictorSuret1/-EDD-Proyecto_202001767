#ifndef VUSER_H
#define VUSER_H

#include <QDialog>
#include "nodoavl.h"
#include "arbolavl.h"
#include "arbolabb.h"
#include <QScrollArea>
#include <QTableWidget>
#include <QGroupBox>
namespace Ui {
class vUser;
}

class vUser : public QDialog
{
    Q_OBJECT

public:
    explicit vUser(const Usuario& usuario, QWidget *parent = nullptr);
    ~vUser();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();


    void llenarTablaSolicitudes();

    void on_tableWidget_2_cellActivated(int row, int column);
    void llenarTablaDesdeArbol(NodoAVL* nodo);

    void on_tableWidget_cellActivated(int row, int column);

    void llenarTablaSolicitudesEnviadas();

    void mostrarPublicacionesUsuario(QScrollArea* scrollArea, Usuario* usuarioLogeado, ArbolABB& arbolPublicaciones);

    void mostrarPublicacionesEnScrollArea(QScrollArea* scrollArea, const QList<Publicacion*>& publicaciones);
    void extraerPublicaciones(ArbolABB& arbol, QList<Publicacion*>& listaPublicaciones);
    void on_pushButton_5_clicked();

    void agregarNuevaPublicacion();
    void poblarComboBoxFechas();

    void obtenerFechasDesdeArbol(NodoABB* nodo, QList<QDate>& fechas);

    void mostrarPublicacionesPorFecha();
    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void mostrarPublicacionesEnScrollArea2(QScrollArea* scrollArea, const std::vector<Publicacion*>& publicaciones, int limite);
    void on_pushButton_8_clicked();

    void llenarTablaFechasYPublicaciones(QTableWidget* tabla, ArbolABB& arbolPublicaciones);
    void llenarTablaPublicacionesConComentarios(QTableWidget* tabla, ArbolABB& arbolPublicaciones);

    void poblarComboBoxABB();
    void graficarArbolABB(NodoABB* nodoFecha);

    void on_pushButton_9_clicked();

    void mostrarImagenEnGroupBox(QGroupBox* groupBox, const QString& rutaImagen);



    void on_pushButton_4_clicked();

private:

    Ui::vUser *ui;
    Usuario usuarioActual;
    ArbolAVL* arbolUsuarios;
};

#endif // VUSER_H
