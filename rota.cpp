#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <set>

using namespace std;

// Estrutura para representar cada ponto de ônibus
struct BusStop {
    string name;
    string location;
    pair<double, double> coordinates; // (latitude, longitude)
    vector<pair<int, int>> buses; // (número do ônibus, minutos até a chegada)
    vector<string> peakHours; // Horários de pico
    int busFrequency; // Frequência dos ônibus (em minutos)
    int busCapacity; // Capacidade dos ônibus
};

// Grafo representando as rotas de ônibus
class BusRouteGraph {
private:
    unordered_map<int, BusStop> stops; // Mapa de pontos de ônibus (número do ponto, dados do ponto)
    unordered_map<int, vector<int>> adjList; // Lista de adjacência representando as conexões entre pontos de ônibus

public:
    // Adiciona um ponto de ônibus ao grafo
    void addBusStop(int stopNumber, const string& stopName, const string& stopLocation, double latitude, double longitude) {
        stops[stopNumber].name = stopName;
        stops[stopNumber].location = stopLocation;
        stops[stopNumber].coordinates = make_pair(latitude, longitude);
    }

    // Adiciona uma aresta entre dois pontos de ônibus (rota de ônibus)
    void addEdge(int stopNumber1, int stopNumber2) {
        adjList[stopNumber1].push_back(stopNumber2);
        adjList[stopNumber2].push_back(stopNumber1); // Se as rotas são bidirecionais
    }

    // Adiciona um ônibus a um ponto de ônibus com o tempo estimado de chegada
    void addBusToStop(int stopNumber, int busNumber, int minutes) {
        stops[stopNumber].buses.push_back(make_pair(busNumber, minutes));
    }

    // Retorna os ônibus que estão a caminho do ponto de ônibus especificado
    vector<pair<int, int>>& getBusesApproaching(int stopNumber) {
        return stops[stopNumber].buses;
    }

    // Função para acessar os pontos de ônibus no grafo
    unordered_map<int, BusStop>::iterator begin() { return stops.begin(); }
    unordered_map<int, BusStop>::iterator end() { return stops.end(); }

    // Adiciona uma lista de pontos de ônibus ao grafo
    void addBusStops(const vector<pair<int, pair<string, pair<string, pair<double, double>>>>>& busStops) {
        for (const auto& stop : busStops) {
            addBusStop(stop.first, stop.second.first, stop.second.second.first, stop.second.second.second.first, stop.second.second.second.second);
        }
    }

    // Adiciona uma lista de arestas ao grafo
    void addEdges(const vector<pair<int, int>>& edges) {
        for (const auto& edge : edges) {
            addEdge(edge.first, edge.second);
        }
    }

    // Retorna o nome da localização do ponto de ônibus
    string getStopLocation(int stopNumber) {
        return stops[stopNumber].location;
    }

    // Retorna o nome do ponto de ônibus
    string getStopName(int stopNumber) {
        return stops[stopNumber].name;
    }

    // Simula a atualização dos tempos de chegada dos ônibus
    void updateBusTimes() {
        for (auto& stop : stops) {
            for (auto& bus : stop.second.buses) {
                if (bus.second > 0) {
                    bus.second--; // Decrementa o tempo de chegada em 1 minuto
                }
            }
        }
    }

    // Exibe os ônibus a caminho de um ponto específico
    void displayBuses(int stopNumber) {
        auto approachingBuses = getBusesApproaching(stopNumber);
        if (approachingBuses.empty()) {
            cout << "Não há ônibus a caminho do ponto " << stopNumber << "." << endl;
        } else {
            cout << "\nPonto " << stopNumber << ":" << endl;
            cout << setw(10) << "Linha" << setw(20) << "Localização" << setw(15) << "Próximo" << setw(25) << "Seguinte" << endl;
            cout << "-----------------------------------------------------------------------------" << endl;
            for (auto& bus : approachingBuses) {
                int currentTime = bus.second; // Tempo atual
                int nextTime = currentTime + 1; // Tempo seguinte sempre maior que o atual
                cout << setw(10) << bus.first
                     << setw(20) << getStopLocation(stopNumber)
                     << setw(15) << currentTime << " min"
                     << setw(25) << nextTime << " min" << endl;
            }
        }
    }

    // Mostra as conexões entre os pontos de ônibus
    void showConnections() {
        cout << "\nConexões entre pontos de ônibus:" << endl;
        for (const auto& pair : adjList) {
            cout << "Ponto " << pair.first << " está conectado a: ";
            for (const auto& neighbor : pair.second) {
                cout << neighbor << " (" << getStopName(neighbor) << ") ";
            }
            cout << endl;
        }
    }

    // Verifica se um ponto de ônibus existe no grafo
    bool isValidStop(int stopNumber) {
        return stops.find(stopNumber) != stops.end();
    }

    // Limpa todos os dados do grafo
    void clearGraph() {
        stops.clear();
        adjList.clear();
    }
};

void displaySpinner() {
    const char spinner[] = {'|', '/', '-', '\\'};
    for (int i = 0; i < 60; ++i) { // 60 segundos
        cout << "\rAtualizando em " << (59 - i) << " segundos... " << spinner[i % 4] << flush;
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "\r" << string(30, ' ') << "\r"; // Limpa a linha depois que o contador termina
}

int main() {
    srand(time(0));
    BusRouteGraph graph;

    // Exemplo: Adicionando pontos de ônibus com coordenadas fictícias
    vector<pair<int, pair<string, pair<string, pair<double, double>>>>> busStops = {
        {1234, {"Vila Isabel", {"Goianira", {rand() % 100 / 10.0, rand() % 100 / 10.0}}}},
        {5678, {"Itatiaia", {"Goiania", {rand() % 100 / 10.0, rand() % 100 / 10.0}}}},
        {3698, {"Setor Universitário", {"Goiania", {rand() % 100 / 10.0, rand() % 100 / 10.0}}}}
    };

    graph.addBusStops(busStops);

    // Exemplo: Adicionando arestas entre pontos de ônibus
    vector<pair<int, int>> edges = {
        {1234, 3698},
        {3698, 5678},
        {5678, 1234}
    };

    graph.addEdges(edges);

    // Exemplo: Adicionando ônibus aos pontos de ônibus
    graph.addBusToStop(1234, 113, 10);
    graph.addBusToStop(5678, 105, 12);
    graph.addBusToStop(5678, 263, 5);
    graph.addBusToStop(3698, 132, 7);

    // Loop para simular atualizações e entrada do usuário
    while (true) {
        // Simulação de entrada do usuário
        int userStop;
        cout << "\nInsira o número do ponto de ônibus (-1 para sair): ";
        cin >> userStop;

        if (userStop == -1) break;

        // Verifica se o ponto de ônibus é válido
        while (!graph.isValidStop(userStop)) {
            cout << "Ponto de ônibus não encontrado. Digite novamente: ";
            cin >> userStop;
        }

        while (true) {
            // Atualiza os tempos de chegada dos ônibus
            graph.updateBusTimes();

            // Exibe os ônibus a caminho do ponto inserido
            graph.displayBuses(userStop);

            // Mostrar conexões entre pontos de ônibus
            graph.showConnections();

            // Exibe o spinner e espera por 1 minuto
            displaySpinner();

            // Limpa a tela para a próxima atualização
            system("clear");
        }
    }

    return 0;
}
