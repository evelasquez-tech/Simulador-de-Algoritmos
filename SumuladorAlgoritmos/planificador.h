#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <vector>
#include <QString>
#include "proceso.h"


struct SegmentoGantt {
    QString id;
    int inicio;
    int fin;
};

//  Métricas por proceso
struct MetricaProceso {
    QString id;
    int llegada;
    int rafaga;
    int fin;
    int turnaround;
    int espera;
    int respuesta;
};


struct ResultadoPlanificacion {
    std::vector<SegmentoGantt> gantt;
    std::vector<MetricaProceso> metricas;
    int makespan = 0;
    double promEspera = 0;
    double promTurnaround = 0;
    double promRespuesta = 0;
};


class PlanificadorCPU {
public:
    // FCFS
    static ResultadoPlanificacion fcfs(const std::vector<Proceso>& procesos);

    // Round Robin con quantum (q >= 1)
    static ResultadoPlanificacion rr(const std::vector<Proceso>& procesos, int q);
};

#endif
