// DNS_Structs.h
#ifndef DNS_STRUCTS_H
#define DNS_STRUCTS_H

#include <stdint.h>
#include <IPAddress.h>

// Estrutura do Cabeçalho DNS
struct DNSHeader {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

// Informações do Cliente para o mapeamento de requisições
struct ClientInfo {
  IPAddress addr;
  uint16_t port;
};

#endif