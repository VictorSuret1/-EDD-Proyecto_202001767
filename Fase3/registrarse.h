#ifndef REGISTRARSE_H
#define REGISTRARSE_H

#include <QDialog>

namespace Ui {
class Registrarse;
}

class Registrarse : public QDialog
{
    Q_OBJECT

public:
    explicit Registrarse(QWidget *parent = nullptr);
    ~Registrarse();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Registrarse *ui;
};

#endif // REGISTRARSE_H
