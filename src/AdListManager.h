#ifndef AD_LIST_MANAGER_H
#define AD_LIST_MANAGER_H

#include <unordered_set>
#include <WString.h>
#include "blocklist.h"
#include <string>

// Estrutura Hash personalizada para String
struct StringHash {
    size_t operator()(const String& s) const {
        return std::hash<std::string>()(s.c_str());
    }
};

class AdListManager {
private:
    // Listas de hash
    std::unordered_set<String, StringHash> blockedDomains;
    std::unordered_set<String, StringHash> whitelistedDomains;
    String masterAdListURL = "http://Documents/adlist.txt";

public:
    // Construtor agora fica vazio
    AdListManager() : blockedDomains(), whitelistedDomains() 
    {
        // A lógica foi movida para init()
    }
    
    // Nova função de inicialização
    void init() {
        // Preenche as listas com os dados iniciais
        for (int i = 0; i < INITIAL_BLOCKED_DOMAINS_COUNT; i++) {
            blockedDomains.insert(String(INITIAL_BLOCKED_DOMAINS[i]));
        }
        for (int i = 0; i < INITIAL_WHITELISTED_DOMAINS_COUNT; i++) {
            whitelistedDomains.insert(String(INITIAL_WHITELISTED_DOMAINS[i]));
        }
        Serial.println("AdListManager: Listas iniciais carregadas.");
    }
    
    // Lógica de Bloqueio
    bool isBlocked(const String& domain) const {
        bool isBlock = blockedDomains.count(domain); //Se houver elemento = true
        bool isWhite = whitelistedDomains.count(domain);
        return isBlock && !isWhite; // Bloqueia se estiver na lista de bloqueio e não na lista branca 
    }

    // Update da blocklist
    void updateAdList() {
        // Implementação futura usaria HTTPClient para baixar masterAdListURL
        Serial.println("AdListManager: Iniciando atualização da lista principal em segundo plano...");
    }

    // Métodos para gerenciamento manual via UI
    void addCustomDomain(const String& domain, bool isBlocked) {
        if (isBlocked) blockedDomains.insert(domain);
        else whitelistedDomains.insert(domain);
    }
    void removeCustomDomain(const String& domain, bool isBlocked) {
        if (isBlocked) blockedDomains.erase(domain);
        else whitelistedDomains.erase(domain);
    }
};

#endif