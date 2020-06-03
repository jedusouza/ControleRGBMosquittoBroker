
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8 // Pino da fita


/*
#define PIN_led1 6
#define PIN_led2 7
*/


//INSTÂNCIA DA BIBLIOTECA CONTROLADORA DA FITA DE LED RGB
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


//const char* ssid = "ssid";
//const char* password = "pwd";

//DEFINE IP DO DISPOSITIVO
IPAddress ip(192, 168, 1, 125);

//IP DO SERVIDOR MQTT
IPAddress server(000, 000, 000, 000);

//MAC PARA ETHERNET SHIELD
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };

//instacia internet
EthernetClient ethClient;
PubSubClient client(ethClient);




//Função para obter valores separados por "_"
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


//Função que obtém as mensagens do broker que vc se inscreveu
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  String pl = String((char *)payload);
  

  String topico = String(topic);

  //verifica o topico
    if(topico == "fita"){
    Serial.println("entrou_fita");
    //Salva os dados obtidos do broker em variaveis
    String part01 = getValue(pl,'_',0);
    String part02 = getValue(pl,'_',1);
    String part03 = getValue(pl,'_',2);

    //Converte o valor para inteiro
    int r = part01.toInt();
    int g = part02.toInt();
    int b = part03.toInt();

    //Seta a cor na fita
    colorWipe(strip.Color(r,   g,   b), 10); 
    }

  //Exemplo para acender um led
  /*
  if(topico == "led1"){
    Serial.println("entrou1");
    Serial.println();
      if((char)payload[0] == '1') digitalWrite(PIN_led1, HIGH);
      else digitalWrite(PIN_led1, LOW); 
    }
    if(topico == "led2"){
    Serial.println("entrou2");
    Serial.println();
      if((char)payload[0] == '1') digitalWrite(PIN_led2, HIGH);
      else digitalWrite(PIN_led2, LOW); 
    }
*/





}
//Função padrão da biblioteca
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Client";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      
      //Se inscreve no topico fita, para obter as informações lá postadas.
      client.subscribe("fita");



      //Para obter info de led
      /*
      client.subscribe("led1");
      client.subscribe("led2");*/
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      
    }
  }
}

void setup() {
  Serial.begin(9600);

  //Ref. a biblioteca da  fita rgb
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif

  //Inicia a fita rgb
  strip.begin();
  strip.setBrightness(50);
  strip.show();

  //seta a conexão com o broker
  client.setServer(server, 1883);
  client.setCallback(callback);

  //inicia a conexão com a internet
  Ethernet.begin(mac, ip);
  delay(1500);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


}

//Função para exibir as cores na fita.
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
