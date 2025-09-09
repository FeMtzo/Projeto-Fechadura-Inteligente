<<<<<<< HEAD
| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |

# Projeto Fechadura Inteligente com ESP32

Este repositório contém o código-fonte para o projeto de uma fechadura inteligente multifator, utilizando a plataforma **ESP32**. O sistema permite a autenticação através de RFID e teclado matricial, com monitoramento e notificações em tempo real via Telegram.

## Funcionalidades Principais

* **Múltiplos Métodos de Acesso**:
    * Leitor **RFID** para acesso com tags ou cartões.
    * **Teclado Matricial** para senhas numéricas.
* **Controle e Monitoramento Remoto**:
    * Integração com um **bot do Telegram** para enviar notificações de acesso (autorizado ou negado).
    * Comandos para abrir a fechadura remotamente.
* **Feedback ao Usuário**:
    * **LEDs** de status (verde para acesso liberado, vermelho para negado, azul para ocioso).
    * **Buzzer** para feedback sonoro.
* **Hardware Robusto**:
    * Acionamento da trava elétrica através de um **módulo relé**, garantindo isolamento e segurança para o microcontrolador.

## Como Usar o Projeto

### Pré-requisitos

1.  **Hardware**: Monte o circuito conforme os diagramas fornecidos durante o desenvolvimento.
2.  **Software**:
    * [Arduino IDE](https://www.arduino.cc/en/software) (versão 2.x recomendada).
    * Placa **ESP32** configurada na Arduino IDE.
3.  **Bibliotecas**: Instale as seguintes bibliotecas através do Gerenciador de Bibliotecas da IDE:
    * `MFRC522` by GitHubCommunity
    * `Keypad` by Mark Stanley, Alexander Brevig
    * `UniversalTelegramBot` by Brian Lough

### Configuração

1.  Abra o arquivo **`config.h`**.
2.  Insira as credenciais da sua rede Wi-Fi nas constantes `WIFI_SSID` e `WIFI_PASSWORD`.
3.  Insira o token do seu bot e o seu Chat ID do Telegram em `TELEGRAM_BOT_TOKEN` e `TELEGRAM_CHAT_ID`.
4.  Defina a senha padrão em `MASTER_PASSWORD`.
5.  Use o sketch de leitura de UID (fornecido separadamente) para descobrir o ID da sua tag RFID e atualize a constante `MASTER_UID`.

### Compilação e Upload

1.  Conecte seu ESP32 ao computador.
2.  Selecione a placa e a porta COM correta na Arduino IDE.
3.  Clique em **Carregar** (Upload).

## Estrutura de Arquivos do Projeto

O projeto é organizado de forma modular para facilitar a manutenção e o entendimento do código.

## Troubleshooting

* **Erro de compilação: `file not found`**:
    * Certifique-se de que todas as bibliotecas listadas na seção de pré-requisitos foram instaladas corretamente através do Gerenciador de Bibliotecas.
* **Leitor RFID não funciona**:
    * Verifique a fiação do barramento SPI (MOSI, MISO, SCK, SDA). Lembre-se que o módulo MFRC522 deve ser alimentado com **3.3V**.
* **Nada acontece ao digitar a senha**:
    * Confira a ordem dos pinos de linhas e colunas do teclado definidos no `config.h`.
=======
# Projeto-Fechadura-Inteligente
Parte do software para o projeto da oficina de integração: Projeto de Fechadura Inteligente com ESP32, Biometria, RFID, Teclado e notificações via Telegram. 
Aqui faremos todo o código para o esp32!
>>>>>>> 06146139a99c78551225feb1c7d914df3bb72447
