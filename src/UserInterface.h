// UserInterface.h
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <ESPAsyncWebServer.h>

class UserInterface {
private:
    int blockedCount = 0; // Contador de domínios bloqueados

public:
    // Inicia o servidor web
    void startWebServer() {
        // Implementação real: Iniciar ESPAsyncWebServer na porta 80.
        Serial.println("UserInterface: Servidor Web iniciado na porta 80.");
    }

    // Atualiza as estatísticas e permite o gerenciamento
    void atualizaContador() {
        blockedCount++;
    }

    int getBlockedCount() const { return blockedCount; }
};

#endif