# Shadow Portal

![Status](https://img.shields.io/badge/Status-Finalizado-success)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-blue)
![Certificação](https://img.shields.io/badge/Certificação-SYH2%20Solyd-red)
![License](https://img.shields.io/badge/License-Educational-yellow)

> **Dispositivo tático de Engenharia Social e Coleta de OSINT (Open Source Intelligence) baseado em ESP32.**

## Sobre o Projeto

O **Shadow Portal** é uma ferramenta de hardware hacking desenvolvida como requisito prático para a certificação **SYH2 (Solyd Hacking Hardware)**. O objetivo do projeto é demonstrar vetores de ataque baseados em confiança humana e configurações de rede, sem a necessidade de explorar vulnerabilidades de software complexas.

O dispositivo cria um **Rogue Access Point** (Ponto de Acesso Falso) que simula uma rede corporativa ou de visitantes legítima. Utilizando técnicas de *DNS Hijacking* e emulação de infraestrutura (IP Classe B - `172.0.0.1`), ele força dispositivos móveis a exibirem um *Captive Portal* automático, solicitando credenciais ou dados de identificação do usuário.

---

## Funcionalidades

* **Simulação de Infraestrutura "Hotel Mode":** Opera no Gateway `172.0.0.1`, aumentando a legitimidade técnica e compatibilidade com pop-ups de Android/iOS.
* **Captive Portal Forçado:** Intercepta requisições DNS e HTTP, redirecionando todo o tráfego para a interface de engenharia social.
* **Trigger de Sistema Operacional (CNA):** Detecta requisições de teste de conectividade (ex: `/generate_204`, `/hotspot-detect.html`) para disparar a tela de login automaticamente.
* **Persistência de Dados Segura:** Sistema de escrita em Cartão SD com *flush* imediato para evitar corrupção de dados em caso de perda de energia.
* **Diagnóstico Visual:** Feedback de status via LED onboard (Status de Wi-Fi e Erro de SD).

---

## Hardware Necessário

| Componente | Quantidade | Descrição |
| :--- | :---: | :--- |
| **ESP32 DevKit V1** | 1 | Microcontrolador principal (Wi-Fi/Bluetooth) |
| **Módulo MicroSD** | 1 | Leitor de cartão com interface SPI |
| **Cartão MicroSD** | 1 | Qualquer tamanho (Recomendado 4GB ou 8GB) |
| **Jumpers** | 4 | Fêmea-Fêmea ou conforme necessidade |
| **Fonte de Energia** | 1 | PowerBank ou Bateria LiPo |

### Diagrama de Conexões (Pinout SPI)

Conecte o módulo SD à ESP32 conforme a tabela abaixo:

| Pino Módulo SD | Pino ESP32 (GPIO) | Função |
| :--- | :--- | :--- |
| **CS** | D5 (GPIO 5) | Chip Select |
| **SCK** | D18 (GPIO 18) | Clock |
| **MOSI** | D23 (GPIO 23) | Master Out Slave In |
| **MISO** | D19 (GPIO 19) | Master In Slave Out |
| **VCC** | VIN (5V) | Alimentação |
| **GND** | GND | Terra |

---

## Instalação e Uso

### 1. Preparação do Cartão SD
1.  Formate o cartão MicroSD em **FAT32**.
2.  Copie o arquivo `index.html` (disponível na pasta `/sd_files` deste repositório) para a **raiz** do cartão.
3.  Insira o cartão no módulo.

### 2. Upload do Firmware
1.  Abra o arquivo `ShadowPortal.ino` na **Arduino IDE**.
2.  Instale as bibliotecas necessárias (geralmente nativas do pacote ESP32): `WiFi`, `DNSServer`, `WebServer`, `SD`, `SPI`.
3.  Selecione a placa **DOIT ESP32 DEVKIT V1**.
4.  Faça o upload do código.

### 3. Operação em Campo
1.  Conecte o Shadow Portal à bateria.
2.  **Verifique o LED Azul (GPIO 2):**
    **Aceso Fixo:** Sistema Online e pronto.
    **Piscando Lento:** Erro no Cartão SD (Verifique conexões).
    **Piscando Rápido:** Falha crítica no Wi-Fi.
3.  A rede Wi-Fi `Empresa_FUNCIONÁRIOS_Wi-Fi` aparecerá para os dispositivos próximos.
4.  Após a coleta, os dados estarão salvos no arquivo `coleta.txt` no cartão SD.

---

## Guia de Personalização (Customização)

O Shadow Portal é modular. Você pode alterar os dados solicitados (ex: pedir CPF em vez de E-mail) alterando o HTML e o Firmware. Existe uma "ponte" que liga os dois: o atributo `name` do HTML.

### Passo 1: Alterar o HTML (`index.html`)
Mude o atributo `name` no input que deseja alterar.

```html
<input type="text" name="setor" placeholder="Seu Setor">

<input type="text" name="cpf" placeholder="Seu CPF">

Passo 2: Alterar o Firmware (.ino)
No código Arduino, localize a função configurarRotas() e ajuste a leitura e a gravação:

C++

// 1. Recebendo a nova variável (O texto laranja deve ser igual ao 'name' do HTML)
String cpf = server.arg("cpf"); 

// 2. Salvando no arquivo
dataFile.println(cpf);
Nota: Sempre que alterar o atributo name no HTML, é obrigatório recompilar e reenviar o código para a ESP32.

Aviso Legal (Disclaimer)
Este projeto foi desenvolvido estritamente para fins educacionais e acadêmicos.

O uso deste software/hardware para:

Interceptar dados de terceiros sem consentimento;

Atacar redes sem autorização expressa;

Realizar fraudes;

...é ilegal e viola leis de crimes cibernéticos (no Brasil, Lei 12.737/2012). O autor e a Solyd Offensive Security não se responsabilizam pelo mau uso desta ferramenta. Utilize apenas em ambientes controlados (laboratórios) ou em auditorias de Red Teaming contratadas.

📜 Licença
Distribuído sob a licença MIT. Veja LICENSE para mais informações.

Desenvolvido por [Seu Nome] - Projeto de Certificação SYH2.
