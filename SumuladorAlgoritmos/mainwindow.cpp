#include "mainwindow.h"
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulador de Planificación de CPU");
    root = new QWidget(this);
    setCentralWidget(root);

    // ENTRaDA DE PROCESOS
    tblEntrada = new QTableWidget(this);
    tblEntrada->setColumnCount(3);
    tblEntrada->setHorizontalHeaderLabels({"ID", "Llegada", "Ráfaga"});
    tblEntrada->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblEntrada->verticalHeader()->setVisible(false);


    tblEntrada->setRowCount(4);
    tblEntrada->setItem(0,0,new QTableWidgetItem("P1"));
    tblEntrada->setItem(0,1,new QTableWidgetItem("0"));
    tblEntrada->setItem(0,2,new QTableWidgetItem("5"));
    tblEntrada->setItem(1,0,new QTableWidgetItem("P2"));
    tblEntrada->setItem(1,1,new QTableWidgetItem("1"));
    tblEntrada->setItem(1,2,new QTableWidgetItem("3"));
    tblEntrada->setItem(2,0,new QTableWidgetItem("P3"));
    tblEntrada->setItem(2,1,new QTableWidgetItem("2"));
    tblEntrada->setItem(2,2,new QTableWidgetItem("8"));
    tblEntrada->setItem(3,0,new QTableWidgetItem("P4"));
    tblEntrada->setItem(3,1,new QTableWidgetItem("3"));
    tblEntrada->setItem(3,2,new QTableWidgetItem("6"));

    btnAddRow = new QPushButton("+ Fila", this);
    btnDelRow = new QPushButton("– Fila", this);
    connect(btnAddRow, &QPushButton::clicked, this, &MainWindow::agregarFila);
    connect(btnDelRow, &QPushButton::clicked, this, &MainWindow::eliminarFila);

    //  CONTROLES DE SIMULACIÓN
    cmbAlgoritmo = new QComboBox(this);
    cmbAlgoritmo->addItem("FCFS");
    cmbAlgoritmo->addItem("Round Robin");

    spnQuantum = new QSpinBox(this);
    spnQuantum->setRange(1, 1000);
    spnQuantum->setValue(2);
    spnQuantum->setEnabled(false);
    connect(cmbAlgoritmo, &QComboBox::currentTextChanged, this, [this](const QString& t){
        spnQuantum->setEnabled(t.contains("Round"));
    });

    btnSimular = new QPushButton("Simular", this);
    connect(btnSimular, &QPushButton::clicked, this, &MainWindow::simular);

    //  TABLS DE SALIDA
    tblGantt = new QTableWidget(this);
    tblGantt->setColumnCount(3);
    tblGantt->setHorizontalHeaderLabels({"Proceso","Inicio","Fin"});
    tblGantt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblGantt->verticalHeader()->setVisible(false);
    tblGantt->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tblMetricas = new QTableWidget(this);
    tblMetricas->setColumnCount(7);
    tblMetricas->setHorizontalHeaderLabels({"ID","Llegada","Ráfaga","Fin","Turnaround","Espera","Respuesta"});
    tblMetricas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblMetricas->verticalHeader()->setVisible(false);
    tblMetricas->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //  VISTA
    auto filaEntradaBtns = new QHBoxLayout();
    filaEntradaBtns->addWidget(new QLabel("Procesos (edita ID, Llegada, Ráfaga):"));
    filaEntradaBtns->addStretch();
    filaEntradaBtns->addWidget(btnAddRow);
    filaEntradaBtns->addWidget(btnDelRow);

    auto filaControles = new QHBoxLayout();
    filaControles->addWidget(new QLabel("Algoritmo:"));
    filaControles->addWidget(cmbAlgoritmo);
    filaControles->addSpacing(10);
    filaControles->addWidget(new QLabel("Quantum:"));
    filaControles->addWidget(spnQuantum);
    filaControles->addSpacing(20);
    filaControles->addWidget(btnSimular);
    filaControles->addStretch();

    auto layout = new QVBoxLayout();
    layout->addLayout(filaEntradaBtns);
    layout->addWidget(tblEntrada, 1);
    layout->addSpacing(8);
    layout->addLayout(filaControles);
    layout->addWidget(new QLabel("Diagrama de Gantt"));
    layout->addWidget(tblGantt, 1);
    layout->addWidget(new QLabel("Métricas"));
    layout->addWidget(tblMetricas, 1);

    root->setLayout(layout);
}


void MainWindow::agregarFila() {
    tblEntrada->insertRow(tblEntrada->rowCount());
}
void MainWindow::eliminarFila() {
    int r = tblEntrada->currentRow();
    if (r >= 0) tblEntrada->removeRow(r);
}

//  Lógica
std::vector<Proceso> MainWindow::leerProcesosDesdeTabla()
{
    std::vector<Proceso> v;
    const int rows = tblEntrada->rowCount();

    for (int r=0; r<rows; ++r) {
        auto *idItem = tblEntrada->item(r,0);
        auto *lItem  = tblEntrada->item(r,1);
        auto *bItem  = tblEntrada->item(r,2);

        const QString id = idItem ? idItem->text().trimmed() : QString();
        if (id.isEmpty()) continue; // este if ignnora lass  filas vacias

        bool okL=false, okB=false;
        int llegada = lItem ? lItem->text().toInt(&okL) : 0;
        int rafaga  = bItem ? bItem->text().toInt(&okB) : 0;

        if (!okL || !okB || llegada < 0 || rafaga <= 0) {
            QMessageBox::warning(this, "Datos inválidos",
                                 QString("Fila %1: verifica Llegada (>=0) y Ráfaga (>0)").arg(r+1));
            v.clear();
            return v;
        }
        v.push_back(Proceso{id, llegada, rafaga});
    }

    if (v.empty()) {
        QMessageBox::information(this, "Sin procesos", "Agrega al menos un proceso.");
    }
    return v;
}

QColor MainWindow::colorParaId(const QString& id) const
{
    if (id == "IDLE") return QColor(180,180,180);

    const uint h = qHash(id) % 360;

    return QColor::fromHsl(h, 170, 140);
}

void MainWindow::poblarGantt(const std::vector<SegmentoGantt>& g)
{
    tblGantt->setRowCount(static_cast<int>(g.size()));
    for (int i=0; i<(int)g.size(); ++i) {
        const auto& s = g[i];
        auto *c0 = new QTableWidgetItem(s.id);
        auto *c1 = new QTableWidgetItem(QString::number(s.inicio));
        auto *c2 = new QTableWidgetItem(QString::number(s.fin));

        const QColor col = colorParaId(s.id);
        c0->setBackground(col);
        c1->setBackground(col);
        c2->setBackground(col);


        const QColor text = (col.lightness() < 130) ? Qt::white : Qt::black;
        c0->setForeground(text);
        c1->setForeground(text);
        c2->setForeground(text);

        tblGantt->setItem(i,0,c0);
        tblGantt->setItem(i,1,c1);
        tblGantt->setItem(i,2,c2);
    }
}

void MainWindow::poblarMetricas(const ResultadoPlanificacion& R)
{
    tblMetricas->setRowCount(static_cast<int>(R.metricas.size()));
    for (int i=0;i<(int)R.metricas.size();++i) {
        const auto& m = R.metricas[i];
        tblMetricas->setItem(i,0,new QTableWidgetItem(m.id));
        tblMetricas->setItem(i,1,new QTableWidgetItem(QString::number(m.llegada)));
        tblMetricas->setItem(i,2,new QTableWidgetItem(QString::number(m.rafaga)));
        tblMetricas->setItem(i,3,new QTableWidgetItem(QString::number(m.fin)));
        tblMetricas->setItem(i,4,new QTableWidgetItem(QString::number(m.turnaround)));
        tblMetricas->setItem(i,5,new QTableWidgetItem(QString::number(m.espera)));
        tblMetricas->setItem(i,6,new QTableWidgetItem(QString::number(m.respuesta)));
    }
    // Fila de promedios
    int r = tblMetricas->rowCount();
    tblMetricas->insertRow(r);
    tblMetricas->setItem(r,0,new QTableWidgetItem("PROMEDIO"));
    tblMetricas->setItem(r,4,new QTableWidgetItem(QString::number(R.promTurnaround,'f',2)));
    tblMetricas->setItem(r,5,new QTableWidgetItem(QString::number(R.promEspera,'f',2)));
    tblMetricas->setItem(r,6,new QTableWidgetItem(QString::number(R.promRespuesta,'f',2)));
}

void MainWindow::simular()
{

    std::vector<Proceso> procesos = leerProcesosDesdeTabla();
    if (procesos.empty()) return;


    ResultadoPlanificacion R;
    const QString alg = cmbAlgoritmo->currentText();
    if (alg.contains("Round")) {
        R = PlanificadorCPU::rr(procesos, spnQuantum->value());
    } else {
        R = PlanificadorCPU::fcfs(procesos);
    }


    poblarGantt(R.gantt);
    poblarMetricas(R);
}
