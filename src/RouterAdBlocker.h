// RouterAdBlocker.h
#ifndef ROUTER_AD_BLOCKER_H
#define ROUTER_AD_BLOCKER_H

#include <WiFi.h>
#include <AsyncUDP.h>
#include <unordered_map>
#include "DNS_Structs.h"
#include "AdListManager.h"
#include "NetworkFilter.h"
#include "UserInterface.h"

class RouterAdBlocker {
private:
    // Configuração de rede
    const char* ssid = "MinhaRedeWiFi";
    const char* password = "SenahaSegura123";
    
    // Configurações DNS
    const IPAddress DNS_SERVER = IPAddress(8, 8, 8, 8); 
    const int DNS_PORT = 53;
    const IPAddress SINKHOLE_IP = IPAddress(0, 0, 0, 0); 
    
    AsyncUDP clientDnsUdp;
    AsyncUDP upstreamDnsUdp;
    
    // Instâncias dos Módulos
    AdListManager adListManager;
    NetworkFilter networkFilter;
    UserInterface userInterface;
    
    // Mapa para rastrear requisições pendentes
    std::unordered_map<uint16_t, ClientInfo> dnsRequests;

    // Métodos de manipulação de pacotes
    void handleUpstreamResponse(AsyncUDPPacket packet);
    void handleIncomingDnsQuery(AsyncUDPPacket packet);

    // Variável estática para permitir que as funções de callback acessem a instância
    static RouterAdBlocker* instance;

public:
    RouterAdBlocker();

    // Método principal de inicialização
    void startBlocking(); 

    // Método principal de loop (para tarefas de segundo plano)
    void runLoop();

    // Funções estáticas de callback para AsyncUDP
    static void staticHandleUpstreamResponse(AsyncUDPPacket packet) {
        if (instance) instance->handleUpstreamResponse(packet);
    }
    static void staticHandleIncomingDnsQuery(AsyncUDPPacket packet) {
        if (instance) instance->handleIncomingDnsQuery(packet);
    }
};

#endif
