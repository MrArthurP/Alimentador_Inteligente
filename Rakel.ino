#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Inclusão da internet que será conectada, e senha
// como fazer isso de forma automática??
const char* ssid = "Arthur";
const char* password = "tutu2023";

// Especificação do servidor a ser utilizado
ESP8266WebServer server(80);

const int ledPin = D4; // Pino do LED
bool ledState = false; // Estado inicial do LED

// Condições Iniciais
void setup() {
  pinMode(ledPin, OUTPUT); // Configura o pino ledPin como OUTPUT
  digitalWrite(ledPin, LOW); // Seta o estado do pino como LOW (DESLIGADO)

  Serial.begin(115200); // inicia o Monitor Serial 

  WiFi.begin(ssid, password); // Se conecta com o wifi especificado anteriormente
  while (WiFi.status() != WL_CONNECTED) { // enquanto não conectar: sinalizar a tentativa de conexão
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // liga e desliga o led para indicar a conexão
  
for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);  // Ligar o LED
    delay(100);
    digitalWrite(ledPin, LOW);  // Desligar o LED
    delay(100);
    }

  Serial.println(WiFi.localIP()); // indica o IP que realizou a conexão

  server.on("/", []() {
    server.send(200, "text/plain", "Hello from NodeMCU!");
  });

  server.on("/led", []() { 
    if (server.hasArg("action")) {
      String action = server.arg("action");
      if (action == "ligar") {
        digitalWrite(ledPin, HIGH);
        Serial.println("Ligou");
        ledState = true;
      } else if (action == "desligar") {
        digitalWrite(ledPin, LOW);
        Serial.println("Desligou");
        ledState = false;
      }
    }
      
    server.send(200, "text/plain", ledState ? "LED Ligado" : "LED Desligado");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
