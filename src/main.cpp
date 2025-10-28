#include "RouterAdBlocker.h"
#include <arpa/inet.h> // nhtohs converte o ID de 16 bits da 'Ordem de Byte de Rede' (Big Endian) para a 'Ordem de Byte do Host'.
#include <Arduino.h> // Necessário para setup() e loop()

// Inicialização da variável estática
RouterAdBlocker* RouterAdBlocker::instance = nullptr;
RouterAdBlocker adBlocker; // Instância global do RouterAdBlocker

// Construtor
RouterAdBlocker::RouterAdBlocker() : networkFilter(adListManager) // Inicializa NetworkFilter com AdListManager
{
    instance = this;
}

// Recebe a resposta do upstream e a envia ao cliente original
void RouterAdBlocker::handleUpstreamResponse(AsyncUDPPacket packet) {
    DNSHeader* responseHeader = (DNSHeader*) packet.data();
    uint16_t id = ntohs(responseHeader->id);

    if (dnsRequests.count(id)) {
        ClientInfo& client = dnsRequests[id];
        // Envia o pacote de resposta real de volta ao cliente
        clientDnsUdp.writeTo(packet.data(), packet.length(), client.addr, client.port);
        dnsRequests.erase(id);
        Serial.printf("Response: (ID %d) Forwarded real response to %s\n", id, client.addr.toString().c_str());
    }
}

// Recebe e processa a requisição DNS do cliente
void RouterAdBlocker::handleIncomingDnsQuery(AsyncUDPPacket packet) {
    String primaryDomain;
    DNSHeader* dnsHeader = (DNSHeader*) packet.data();

    // Processamento e Decisão
    bool isBlocked = networkFilter.processAndDecide(packet, primaryDomain);
    
    // Domínio Bloqueado
    if (isBlocked) {
        // RouterAdBlocker::notificaBloqueio (Simulado)
        userInterface.atualizaContador();

        Serial.printf("Block: (ID %d) Domain %s BLOCKED. Total: %d\n", 
                      ntohs(dnsHeader->id), primaryDomain.c_str(), userInterface.getBlockedCount());

        // Constrói e envia a Resposta Falsa
        //Pego a requisição original e monto a resposta DNS com o IP de Sinkhole
        int position = packet.length(); // Tamanho da requisição original
        uint8_t response[position + 16]; // Cria um array de bytes para a resposta DNS + 16 bytes da seção de resposta
        memcpy(response, packet.data(), position); // Copia a requisição original para a resposta
        DNSHeader* responseHeader = (DNSHeader*) response; // Aponta para o cabeçalho DNS na resposta

        // Configura cabeçalho para resposta e ANCOUNT=1
        responseHeader->flags = htons(ntohs(dnsHeader->flags) | 0x8000); // Define o bit de resposta
        responseHeader->ancount = htons(1); // Diz ao dispositivo que há 1 resposta nesse pacote DNS

        // Adiciona a seção de resposta DNS
        response[position++] = 0xC0; // Nome (DNS Compression Pointer)
        response[position++] = 0x0C;
        response[position++] = 0x00; // Record: A record
        response[position++] = 0x01;
        response[position++] = 0x00; // Class: standard
        response[position++] = 0x01;
        response[position++] = 0x00; // TTL (60s)
        response[position++] = 0x00;
        response[position++] = 0x00;
        response[position++] = 0x3C; 
        response[position++] = 0x00; // Data length (4 bytes)
        response[position++] = 0x04;

        // Endereço IP de Sinkhole (0.0.0.0)
        response[position++] = SINKHOLE_IP[0];
        response[position++] = SINKHOLE_IP[1];
        response[position++] = SINKHOLE_IP[2];
        response[position++] = SINKHOLE_IP[3];

        clientDnsUdp.writeTo(response, position, packet.remoteIP(), packet.remotePort());
        return; // Sai após enviar a resposta falsa
    }
    
    // Domínio Permitido, encaminhamento para Upstream:
    Serial.printf("Pass: (ID %d) Domain %s FORWARDING to Upstream...\n", 
                  ntohs(dnsHeader->id), primaryDomain.c_str());

    // Salva info do cliente e encaminha para o Upstream DNS
    ClientInfo client = { packet.remoteIP(), packet.remotePort() };
    dnsRequests[ntohs(dnsHeader->id)] = client;

    // Encaminha a requisição DNS para o servidor upstream
    upstreamDnsUdp.writeTo(packet.data(), packet.length(), DNS_SERVER, DNS_PORT);
}

// Método principal de inicialização
void RouterAdBlocker::startBlocking() {
    Serial.begin(115200);

    // Inicialização do Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed! Check credentials.");
        while (1) delay(1000);
    }
    Serial.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());
    adListManager.init();
    // Inicialização dos Módulos
    userInterface.startWebServer(); // UserInterface

    // Inicialização dos Listeners DNS
    if (clientDnsUdp.listen(DNS_PORT)) {
        Serial.println("Servidor DNS escutando na porta 53.");
        clientDnsUdp.onPacket(staticHandleIncomingDnsQuery); // Listener de Requisições
    }

    // Conexão upstreamDnsUdp
    if (upstreamDnsUdp.connect(DNS_SERVER, DNS_PORT)) {
        Serial.printf("Conectado ao Upstream DNS: %s\n", DNS_SERVER.toString().c_str());
        upstreamDnsUdp.onPacket(staticHandleUpstreamResponse); // Listener de Respostas
    }
    
}

void RouterAdBlocker::runLoop() {
    // Chamada periódica e assíncrona para updateAdList
    static unsigned long lastUpdate = 0;
    const unsigned long UPDATE_INTERVAL = 3600000; // 1 hora em ms

    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        adListManager.updateAdList(); 
        lastUpdate = millis();
    }
    delay(1); 
}

void setup() {
    // Chama o método de inicialização principal da sua classe
    adBlocker.startBlocking(); 
}

void loop() {
    // Chama o método de loop da sua classe
    adBlocker.runLoop();
}