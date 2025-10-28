#ifndef VENTANAFIFO_H
#define VENTANAFIFO_H

#include <QDialog>
#include <QTableWidget>
#include <QString>
#include <vector>
#include <QHash>

namespace Ui {
class VentanaFifo;
}

struct Proceso{
    int id;
    int rafaga;
    int llegada;
    QString estado;
};

class VentanaFifo : public QDialog
{
    Q_OBJECT

public:
    explicit VentanaFifo(QWidget *parent = nullptr);
    ~VentanaFifo();

private slots:
    void on_btnAgregar_clicked();

    void on_btnIniciar_clicked();

private:
    Ui::VentanaFifo *ui;
    QHash<int, int> rowById_;

    std::vector<Proceso> procesos_;
    int nextId_ = 1;

    QTableWidgetItem* makeCell(const QString& text, bool center = false);
    void agregarProcesoATabla(const Proceso& p);

    int findRowByProcessId(int id) const;
};

#endif // VENTANAFIFO_H
