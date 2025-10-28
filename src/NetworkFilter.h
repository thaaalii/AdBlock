#ifndef NETWORK_FILTER_H
#define NETWORK_FILTER_H

#include "AdListManager.h"
#include "DNS_Structs.h"
#include "AsyncUDP.h"

class NetworkFilter {
private:
    AdListManager& adListManager;
    // Extrai o nome de domínio
    String extractPrimaryDomain(AsyncUDPPacket& packet);

public:
    NetworkFilter(AdListManager& manager) : adListManager(manager) {} // Inicializa 
    // Lógica central: processa e decide se bloqueia. Retorna TRUE se for bloqueado.
    bool processAndDecide(AsyncUDPPacket& packet, String& primaryDomainOut) {
        // Extrai domínio do pacote
        primaryDomainOut = extractPrimaryDomain(packet);
        
        // Compara e decide o status
        return adListManager.isBlocked(primaryDomainOut);
    }
};

// Implementação da função auxiliar de extração de domínio (adaptada do artigo)
String NetworkFilter::extractPrimaryDomain(AsyncUDPPacket& packet) {
     
    char fullDomainName[253];
    int idx = 0;
    bool needDot = false;
    
    // Inicia a leitura após o cabeçalho DNS
    for (int i = sizeof(DNSHeader); i < packet.length(); i++) {
        uint8_t len = packet.data()[i];
        if (len == 0) break;

        if (needDot) fullDomainName[idx++] = '.';
        
        for (int j = 0; j < len && (i + 1 + j) < packet.length(); j++) {
            fullDomainName[idx++] = (char)packet.data()[i + 1 + j];
        }
        i += len;
        needDot = true;
    }
    fullDomainName[idx] = '\0';

    // Lógica para extrair o domínio primário (<second_level_domain>.<TLD>)
    int dotsSeen = 0;
    int primaryStart = 0;
    while (--idx >= 0) {
        if (fullDomainName[idx] == '.') {
            dotsSeen++;
            if (dotsSeen == 2) {
                primaryStart = idx + 1;
                break;
            }
        }
    }
    return String(fullDomainName + primaryStart);
}

#endif