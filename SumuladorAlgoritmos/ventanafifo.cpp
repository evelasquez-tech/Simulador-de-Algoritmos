#include "ventanafifo.h"
#include "ui_ventanafifo.h"

VentanaFifo::VentanaFifo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VentanaFifo)
{
    ui->setupUi(this);
}

VentanaFifo::~VentanaFifo()
{
    delete ui;
}
