/*
  Exemplo do código para ESP32/Arduino
  No código está comentado como seria para o esp32

*/

#include <WiFi.h>
#include <PubSubClient.h>

// ALTERE OS VALORES DE ACORDO COM SUA REDE
// Caso seja no esp32 ou esp8266
/*const char* ssid = "SSID";
const char* password = "PWD";
*/
IPAddress localIp(192, 168, 1, 125); // IP para seu dispositivo
const char* mqtt_server = "192.168.1.9"; // IP LOCAL
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
// CONSTANTES DOS PINOS ANALÓGICOS 
#define pot_r  A0 
#define pot_g  A1 
#define pot_b  A2 

// VÁRIAVEIS PARA OS VALORES DO POTENCIOMETRO
int r = 0; 
int g = 0;
int b = 0;

//INSTANCIA DO WIFI
//WiFiClient Con;
Ethernet Con;


//INSTANCIA DO PUBSUBCLIENT
PubSubClient client(Con);

//Função para Wifi
/*
void setup_wifi() {

  delay(10);
  // Inicia a conexão wifi
  Serial.println();
  Serial.print("Conectando à ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}
*/
//Função que obtém as mensagens do broker que vc se inscreveu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Messagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//função da biblioteca pubsubclient para criar conexão
//Ela publica em outTopic para teste.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//Variavel usada para enviar os dados quando forem alterados
String buf_save;
void setup() {

  //Serial.begin(115200); // ESP32
  Serial.begin(9600); 
  //setup_wifi(); // cria conexão wifi
  Ethernet.begin(mac, localIP);

  client.setServer(mqtt_server, 1883); //Salva o server para conexão
  client.setCallback(callback); // Salva a função de callback - caso precise. Nesse caso, só será enviado informações.
}

void loop() {

//Cria a conexão
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  //Divisão para ficar dentro do intervalo 0-255 (cores rgb), pode ser utilizado a função map.
  //Esp32 lê valores analógicos até 4091, arduino até 1023
  //r = analogRead(pot_r)/16;    
  //g = analogRead(pot_g)/16;
  //b = analogRead(pot_b)/16;
  r = analogRead(pot_r)/4;    
  g = analogRead(pot_g)/4;
  b = analogRead(pot_b)/4;         

  //coloca os valores inteiros em um array de char separados por "_"(R_G_B)
  char buf[100];
  sprintf(buf,"%d_%d_%d",r,g,b);

  //verifica se houve alteração nos dados
  if(buf_save != String(buf)) {
    //publica os dados no broker, no canal fita
    client.publish("fita", buf);
  
  Serial.println(buf);
  
  }
  delay(300);
  
  //Salva String para proxima comparação
  buf_save = String(buf);
}
