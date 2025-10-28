// blocklist.h
#ifndef BLOCKLIST_H
#define BLOCKLIST_H

// Simula os domínios de anúncios/rastreadores
const char* INITIAL_BLOCKED_DOMAINS[] = {
  "instagram.com",
  "facebook.com",
  "ads.google.com",
  "doubleclick.net",
  "youtube.com"
};

const int INITIAL_BLOCKED_DOMAINS_COUNT = 5;

// Simula domínios que o usuário pode querer permitir (whitelist)
const char* INITIAL_WHITELISTED_DOMAINS[] = {

};
const int INITIAL_WHITELISTED_DOMAINS_COUNT = 0;

#endif