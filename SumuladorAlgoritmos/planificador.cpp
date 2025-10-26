#include "planificador.h"

#include <algorithm>
#include <queue>
#include <QHash>


static ResultadoPlanificacion finalizar(
    const std::vector<SegmentoGantt>& gantt,
    const std::vector<Proceso>& procesos,
    const QHash<QString,int>& tPrimerInicio,
    const QHash<QString,int>& tFinAcumulado)
{
    ResultadoPlanificacion R;
    R.gantt = gantt;

    int sumaW = 0, sumaT = 0, sumaR = 0;

    for (const auto& p : procesos) {
        MetricaProceso m;
        m.id      = p.id;
        m.llegada = p.llegada;
        m.rafaga  = p.rafaga;

        // Fin
        auto itF = tFinAcumulado.constFind(p.id);
        m.fin = (itF == tFinAcumulado.cend()) ? 0 : *itF;

        // Métricas
        m.turnaround = m.fin - m.llegada;
        m.espera     = m.turnaround - m.rafaga;

        auto itResp = tPrimerInicio.constFind(p.id);
        m.respuesta = (itResp == tPrimerInicio.cend()) ? 0 : (*itResp - m.llegada);

        R.metricas.push_back(m);

        sumaW += m.espera;
        sumaT += m.turnaround;
        sumaR += m.respuesta;

        R.makespan = std::max(R.makespan, m.fin);
    }

    const int n = static_cast<int>(procesos.size());
    if (n > 0) {
        R.promEspera     = static_cast<double>(sumaW) / n;
        R.promTurnaround = static_cast<double>(sumaT) / n;
        R.promRespuesta  = static_cast<double>(sumaR) / n;
    }
    return R;
}


// FCFS

ResultadoPlanificacion PlanificadorCPU::fcfs(const std::vector<Proceso>& procesosIn)
{
    auto procesos = procesosIn;
    std::stable_sort(procesos.begin(), procesos.end(),
                     [](const Proceso& a, const Proceso& b){
                         return a.llegada < b.llegada;
                     });

    std::vector<SegmentoGantt> gantt;
    QHash<QString,int> primerInicio;
    QHash<QString,int> finDeProceso;

    int tiempo = 0;
    for (const auto& p : procesos) {
        if (tiempo < p.llegada) {

            gantt.push_back({"IDLE", tiempo, p.llegada});
            tiempo = p.llegada;
        }
        if (!primerInicio.contains(p.id)) primerInicio[p.id] = tiempo;

        gantt.push_back({p.id, tiempo, tiempo + p.rafaga});
        tiempo += p.rafaga;

        finDeProceso[p.id] = tiempo;
    }
    return finalizar(gantt, procesos, primerInicio, finDeProceso);
}


// Round Robin

ResultadoPlanificacion PlanificadorCPU::rr(const std::vector<Proceso>& procesosIn, int q)
{
    auto procesos = procesosIn;
    std::stable_sort(procesos.begin(), procesos.end(),
                     [](const Proceso& a, const Proceso& b){
                         return a.llegada < b.llegada;
                     });

    struct Nodo { Proceso p; int restante; };
    std::queue<Nodo> cola;
    std::vector<SegmentoGantt> gantt;
    QHash<QString,int> primerInicio;
    QHash<QString,int> finDeProceso;

    int tiempo = 0;
    std::size_t idxLlegan = 0;

    auto encolarLlegadas = [&](int tLim){
        while (idxLlegan < procesos.size() && procesos[idxLlegan].llegada <= tLim) {
            cola.push({procesos[idxLlegan], procesos[idxLlegan].rafaga});
            ++idxLlegan;
        }
    };

    // Arranque
    encolarLlegadas(0);
    if (cola.empty() && !procesos.empty()) {
        gantt.push_back({"IDLE", 0, procesos[0].llegada});
        tiempo = procesos[0].llegada;
        encolarLlegadas(tiempo);
    }

    while (!cola.empty() || idxLlegan < procesos.size()) {
        if (cola.empty()) {
            const int tNext = procesos[idxLlegan].llegada;
            gantt.push_back({"IDLE", tiempo, tNext});
            tiempo = tNext;
            encolarLlegadas(tiempo);
            continue;
        }

        Nodo cur = cola.front(); cola.pop();

        if (!primerInicio.contains(cur.p.id))
            primerInicio[cur.p.id] = tiempo;

        const int run  = std::min(q, cur.restante);
        const int tIni = tiempo;
        const int tFin = tiempo + run;

        gantt.push_back({cur.p.id, tIni, tFin});
        tiempo = tFin;
        cur.restante -= run;

        // Llegadas durante este quantum
        encolarLlegadas(tiempo);

        if (cur.restante > 0) {
            cola.push(cur);
        } else {
            finDeProceso[cur.p.id] = tiempo;
        }
    }

    return finalizar(gantt, procesos, primerInicio, finDeProceso);
}
