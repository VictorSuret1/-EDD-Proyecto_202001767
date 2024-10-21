#ifndef COMENTAR_H
#define COMENTAR_H

#include <QDialog>

namespace Ui {
class Comentar;
}

class Comentar : public QDialog
{
    Q_OBJECT

public:
    explicit Comentar(QWidget *parent = nullptr);
    ~Comentar();
    QString getComentario() const;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Comentar *ui;
    QString comentario;
};


#endif // COMENTAR_H
