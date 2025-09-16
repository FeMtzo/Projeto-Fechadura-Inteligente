#include "FS.h"
#include "LittleFS.h"

#define SENHAS_FILE "/senhas.txt"
#include "config.h"


// Inicializa LittleFS
void securityInit() {
  if (!LittleFS.begin(true)) {
    Serial.println("Erro ao iniciar LittleFS!");
    return;
  }
  Serial.println("LittleFS iniciado.");
}

// Adiciona uma tag ou senha qualquer
void addSenha(String value) {
  if (value == "") return;

  File f = LittleFS.open(SENHAS_FILE, "a"); // "a" = append
  if (!f) {
    Serial.println("Erro ao abrir senhas.txt");
    return;
  }
  f.println(value);
  f.close();
  Serial.println("Adicionado: " + value);
}

// Verifica se uma tag ou senha está cadastrada
bool checkSenha(String value) {
  if (value == "") return false;

  File f = LittleFS.open(SENHAS_FILE, "r");
  if (!f) {
    Serial.println("Erro ao abrir senhas.txt");
    return false;
  }

  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line == value) {
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}

// Remove uma tag ou senha
void removeSenha(String value) {
  if (value == "") return;

  File f = LittleFS.open(SENHAS_FILE, "r");
  if (!f) return;

  File temp = LittleFS.open("/temp.txt", "w");
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line != value) temp.println(line);
  }
  f.close();
  temp.close();

  LittleFS.remove(SENHAS_FILE);
  LittleFS.rename("/temp.txt", SENHAS_FILE);

  Serial.println("Removido: " + value);
}

// Mostra todas as senhas/tags (opcional, debug)
void listSenhas() {
  File f = LittleFS.open(SENHAS_FILE, "r");
  if (!f) {
    Serial.println("Erro ao abrir senhas.txt");
    return;
  }
  Serial.println("=== Lista de senhas/tags ===");
  while (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.println(line);
  }
  f.close();
}
