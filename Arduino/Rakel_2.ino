#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <ESP8266WiFi.h>
 
//defines:
//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SUBSCRIBE "Alimentador_TccRecebe"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "Alimentador_TccEnvio"    //tópico MQTT de envio de informações para Broker
                                                   //IMPORTANTE: recomendamos fortemente alterar os nomes
                                                   //            desses tópicos. Caso contrário, há grandes
                                                   //            chances de você controlar e monitorar o NodeMCU
                                                   //            de outra pessoa.
#define ID_MQTT  "HomeAut"     //id mqtt (para identificação de sessão)
                               //IMPORTANTE: este deve ser único no broker (ou seja,
                               //            se um client MQTT tentar entrar com o mesmo
                               //            id de outro já conectado ao broker, o broker
                               //            irá fechar a conexão de um deles).
                               
 
//defines - mapeamento de pinos do NodeMCU


#define D14    14
#define pinBotao1    12 //D6


 
// WIFI
const char* SSID = "RENATA"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "010507ggr!"; // Senha da rede WI-FI que deseja se conectar
 
// MQTT
// MQTT


const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
 
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


 
//Prototypes
void initWiFi();
void initMQTT();
void reconectWiFi();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);


/*
 *  Implementações das funções
 */
void setup()
{
    //inicializações:


    pinMode(pinBotao1, INPUT_PULLUP);      
    pinMode(D14, OUTPUT);
    digitalWrite(D14, LOW);    
   


    Serial.begin(115200);
    initWiFi();
    initMQTT();
}
 
//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum


//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum


void loop(){
  VerificaConexoesWiFIEMQTT();
  enviaValores();
  MQTT.loop();
}




void initWiFi()
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}
 
//Função: inicializa parâmetros de conexão MQTT(endereço do
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT()
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
//Função: função de callback
//        esta função é chamada toda vez que uma informação de
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum


void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
    String msg;
    // aqui e recebido a msg enviada pelo Flask
 
    for(int i = 0; i < length; i++)
    {
       char c = (char)payload[i];
       msg += c;
    }
    Serial.println(msg);
   
    if(msg == "ligar2"){
      digitalWrite(D14, HIGH);    
    }
    else if(msg == "desligar2"){
      digitalWrite(D14, LOW);  
    }


     
}
 
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT()
{
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT))
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        }
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
 
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi()
{
    //se já está conectado a rede WI-FI, nada é feito.
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
 
//Função: verifica o estado das conexões WiFI e ao broker MQTT.
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected())
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 


//programa principal

void enviaValores() {
static bool estadoBotao1 = LOW;
static bool estadoBotao1Ant = HIGH;
static unsigned long debounceBotao1;


  estadoBotao1 = digitalRead(pinBotao1);
  if (  (millis() - debounceBotao1) > 30 ) {  //Elimina efeito Bouncing
     if (!estadoBotao1 && estadoBotao1Ant) {


        //Botao Apertado    
        MQTT.publish(TOPICO_PUBLISH, "0");
        Serial.println("Botao1 Solto. Payload enviado.");
       
        debounceBotao1 = millis();
     } else if (estadoBotao1 && !estadoBotao1Ant) {


        //Botao Solto
        MQTT.publish(TOPICO_PUBLISH, "1");
        Serial.println("Botao1 Apertado. Payload enviado.");
       
        debounceBotao1 = millis();
     }
     
  }
  estadoBotao1Ant = estadoBotao1;
}






