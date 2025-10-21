#ifndef VENTANAFIFO_H
#define VENTANAFIFO_H

#include <QDialog>

namespace Ui {
class VentanaFifo;
}

class VentanaFifo : public QDialog
{
    Q_OBJECT

public:
    explicit VentanaFifo(QWidget *parent = nullptr);
    ~VentanaFifo();

private:
    Ui::VentanaFifo *ui;
};

#endif // VENTANAFIFO_H
