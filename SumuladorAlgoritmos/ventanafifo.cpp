#include "ventanafifo.h"
#include "ui_ventanafifo.h"
//#include <QDialog>

//#include <QTableWidget>
//#include <QString>
#include <QHeaderView>
#include<QMessageBox>
#include<QIntValidator>
#include<QPushButton>

#include <algorithm>


//Constructor
VentanaFifo::VentanaFifo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VentanaFifo)
{
    ui->setupUi(this);
    setWindowTitle("Simulador FIFO");
    //Tabla de procesos


    ui->tblProcesos->setColumnCount(4);
    ui->tblProcesos->setHorizontalHeaderLabels(
        {"#Proceso", "Ráfaga CPU","Tiempo de Llegada", "Estado"});

    ui->tblProcesos->verticalHeader()->setVisible(false);
    ui->tblProcesos->setAlternatingRowColors(true);

    ui->tblProcesos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblProcesos->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblProcesos->setShowGrid(true);
    ui->tblProcesos->setSortingEnabled(false);

    auto *h1 =ui->tblProcesos->horizontalHeader();
    h1->setDefaultAlignment(Qt::AlignCenter);
    h1->setSectionResizeMode(QHeaderView::Interactive);
    //h1->setStretchLastSection(false);


    ui->tblProcesos->setColumnWidth(0,100);
    ui->tblProcesos->setColumnWidth(1,140);
    ui->tblProcesos->setColumnWidth(2,170);
    ui->tblProcesos->setColumnWidth(3,150);

    ui->tblProcesos->setStyleSheet("QTableWidget { font-size: 11pt; }");
    ui->tblProcesos->verticalHeader()->setDefaultSectionSize(28);


    //Tabla Resultados

    ui->tblResultados->setColumnCount(3);
    ui->tblResultados->setHorizontalHeaderLabels(
        {"#Proceso", "Tiempo de Espera","Tiempo de Sistema"});

    ui->tblResultados->verticalHeader()->setVisible(false);
    ui->tblResultados->setAlternatingRowColors(true);

    ui->tblResultados->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblResultados->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblResultados->setSortingEnabled(false);


    auto *h2 =ui->tblResultados->horizontalHeader();
    h2->setDefaultAlignment(Qt::AlignCenter);
    h2->setSectionResizeMode(QHeaderView::Interactive);
    //h2->setStretchLastSection(false);


    ui->tblResultados->setColumnWidth(0,140);
    ui->tblResultados->setColumnWidth(1,210);
    ui->tblResultados->setColumnWidth(2,210);


    ui->tblResultados->setStyleSheet("QTableWidget { font-size: 11pt; }");
    ui->tblResultados->verticalHeader()->setDefaultSectionSize(28);


    //Botones

    auto *val = new QIntValidator(0,1000000, this);
    ui->txtRafaga->setValidator(val);
    ui->txtLlegada->setValidator(val);

    //connect(ui->btnAgregar, &QPushButton::clicked,
        //   this, &VentanaFifo::on_btnAgregar_clicked);

   // ui->tblProcesos->setSortingEnabled(false);
    //ui->tblResultados->setSortingEnabled(false);
}

VentanaFifo::~VentanaFifo()
{
    delete ui;
}


//Utilidades

QTableWidgetItem* VentanaFifo::makeCell(const QString &text, bool center)
{
    auto *it = new QTableWidgetItem(text);
    Qt::Alignment align = center ? (Qt::AlignHCenter | Qt::AlignVCenter)
                                 : (Qt::AlignLeft | Qt::AlignVCenter);
    //if(center){
       // align=Qt::AlignHCenter | Qt::AlignVCenter;
    //}else{
       // align = Qt::AlignLeft | Qt::AlignVCenter;
   // }
    it->setTextAlignment(align);
return it;
}

void VentanaFifo::agregarProcesoATabla(const Proceso &p)
{
    int r = ui->tblProcesos->rowCount();
    ui->tblProcesos->insertRow(r);

    ui->tblProcesos->setItem(r, 0, makeCell(QString("P%1").arg(p.id), true));
    ui->tblProcesos->setItem(r, 1, makeCell(QString::number(p.rafaga), true));
    ui->tblProcesos->setItem(r, 2, makeCell(QString::number(p.llegada), true));
    ui->tblProcesos->setItem(r, 3, makeCell(p.estado));

    rowById_[p.id]=r;

}

//Helper de ayuda para busqueda
int VentanaFifo::findRowByProcessId(int id) const
{
    const QString needle = QString ("P%1").arg(id);
    for (int r = 0; r < ui->tblProcesos->rowCount(); ++r) {
        QTableWidgetItem *it = ui->tblProcesos->item(r,0);
        //if(!it)continue;
       // if (it->text().trimmed() ==needle) {
        if(it && it->text().trimmed()==needle)
            return r;
        }
        return -1;

}

void VentanaFifo::on_btnAgregar_clicked()
{
    const QString sRafaga = ui->txtRafaga->text().trimmed();
    const QString sLlegada = ui->txtLlegada->text().trimmed();

    if (sRafaga.isEmpty() || sLlegada.isEmpty()) {
        QMessageBox::warning(this, "Campos Vacíos",
                             "Ingrese la Rafaga de CPU y el tiempo de llegada");
        return;
    }

    bool ok1=false, ok2= false;
    int rafaga=sRafaga.toInt(&ok1);
    int llegada=sLlegada.toInt(&ok2);

    if(!ok1 || !ok2 || rafaga <0 || llegada <0)
    {
        QMessageBox::warning(this, "Datos inválidos",
                             "Ingrese numeros enteros no negativos");
        return;
    }

    Proceso p;
    p.id = nextId_++;
    p.rafaga = rafaga;
    p.llegada = llegada;
    p.estado = "En cola";

    procesos_.push_back(p);
    agregarProcesoATabla(p);

    ui->txtRafaga->clear();
    ui->txtLlegada->clear();
    ui->txtRafaga->setFocus();

}


void VentanaFifo::on_btnIniciar_clicked()
{
    if (procesos_.empty()){
        QMessageBox::information(this, "Sin datos",
                                 "Primero ingrese uno o mas procesos");
        return;
    }

    ui->tblResultados->setRowCount(0);

    //ordenamiento timpo de llegada
    std::vector<Proceso> cola =procesos_;
    std::sort(cola.begin(), cola.end(),
        [](const Proceso& a, const Proceso& b){
            if (a.llegada !=b.llegada) return a.llegada < b.llegada;
            return a.id < b.id;
    });

    for(const auto& p:cola){
        int row = rowById_.value(p.id, -1);
        if (row>=0) ui->tblProcesos->setItem(row, 3, makeCell("En cola"));
    }

    int tiempo=0;
    long long sumaEspera=0, sumaSistema=0;


    for (const auto& p: cola){
       //int row = findRowByProcessId(p.id);
       // if(row >= 0) ui->tblProcesos->setItem(row, 3, makeCell("En cola"));
   // }

    //for(const auto& p: cola){
        if(tiempo < p.llegada)
            tiempo = p.llegada;

        int inicio = tiempo;
        int espera = inicio - p.llegada;
        int fin = inicio + p.rafaga;
        int sistema = fin - p.llegada;

        //int rowProc = findRowByProcessId(p.id);
        int rowProc = rowById_.value(p.id, -1);
        if (rowProc >=0)
            ui->tblProcesos->setItem(rowProc, 3, makeCell(("Terminado")));


        int r = ui->tblResultados-> rowCount();
        ui->tblResultados->insertRow(r);
        ui->tblResultados->setItem(r, 0, makeCell(QString("P%1").arg(p.id), true));
        ui->tblResultados->setItem(r, 1, makeCell(QString::number(espera),true));
        ui->tblResultados->setItem(r, 2, makeCell(QString::number(sistema),true));

        sumaEspera += espera;
        sumaSistema += sistema;
        tiempo = fin;
    }

    int n = static_cast<int>(cola.size());
    double promEspera = n ? (double)sumaEspera / n : 0.0;
    double promSistema = n ? (double)sumaSistema / n : 0.0;

    int r = ui->tblResultados->rowCount();
    ui->tblResultados->insertRow(r);
    ui->tblResultados->setItem(r, 0, makeCell("Promedios", false));
    ui->tblResultados->setItem(r,1,makeCell(QString::number(promEspera, 'f', 2), true));
    ui->tblResultados->setItem(r,2,makeCell(QString::number(promSistema, 'f', 2), true));
}

