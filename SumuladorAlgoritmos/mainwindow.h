#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTableWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColor>
#include "planificador.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void simular();
    void agregarFila();
    void eliminarFila();

private:
    QWidget* root = nullptr;

    // Entrada de procesos
    QTableWidget* tblEntrada = nullptr;
    QPushButton* btnAddRow = nullptr;
    QPushButton* btnDelRow = nullptr;

    // Controles de simulación
    QComboBox*  cmbAlgoritmo = nullptr;
    QSpinBox*   spnQuantum   = nullptr;
    QPushButton* btnSimular  = nullptr;

    // Salida
    QTableWidget* tblGantt    = nullptr;
    QTableWidget* tblMetricas = nullptr;


    std::vector<Proceso> leerProcesosDesdeTabla();
    void poblarGantt(const std::vector<SegmentoGantt>& g);
    void poblarMetricas(const ResultadoPlanificacion& R);
    QColor colorParaId(const QString& id) const;
};

#endif


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_boton_fifo_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

