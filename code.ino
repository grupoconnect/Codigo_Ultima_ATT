#include <PZEM004Tv30.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h> //Biblioteca NTPClient para trabalhar com tempo
#include <HTTPClient.h>



#define FIREBASE_HOST "consumidor-iiot-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "P8CgJ3w7PqqlVPTLN4JNwzKVbM3rCzPv5ORR8dRd"
#define CODIGO4 "gc007"
#define CODIGO5 "gc008"
#define CODIGO6 "gc009"


# define  PZEM_RX_PIN  21
# define  PZEM_TX_PIN  22

# define  PZEM_SERIAL Serial2

#define TX 22
#define RX 21

#define INTERVALO_TEMPO 30

const char* WIFI_SSID = "PQTEC VISITOR";


//Dias da semana
String weekDays[7]={"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sabado"};
//Nome dos meses
String months[12]={"Janeiro", "Fevereiro", "Marco", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};

//Flag para a mensagem
bool messageSent = false; // Flag para controlar o envio da mensagem
int lastDayOfMonth = -1; // Armazena o último dia do mês verificado

WiFiUDP udp; //Socket UDP que a lib do NTP utiliza para recuperar dados sobre o horário
NTPClient timeClient(
    udp,                    //socket udp
    "0.br.pool.ntp.org",    //URL do servwer NTP
    (-3)*3600,          //Deslocamento do horário em relacão ao GMT 0
    999);


PZEM004Tv30 pzem4(Serial2,21, 22, 0x01);
PZEM004Tv30 pzem5(Serial2,21, 22, 0x02);
PZEM004Tv30 pzem6(Serial2,21, 22, 0x03);

FirebaseData firebaseData;

// Definição do Watchdog Timer
#include "esp_system.h"
#define WATCHDOG_TIMEOUT 10 // Tempo de timeout em segundos
hw_timer_t *timer = NULL;

// Função do Watchdog Timer
void IRAM_ATTR resetModule() {
  ets_printf("Reiniciando ESP32...\n");
  esp_restart();
}

void connectToWifi() {
 WiFi.begin(WIFI_SSID);
 while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
  }
  Serial.println("Conectado!");
}

void setup() {
  Serial.begin(115200);
  connectToWifi();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin(); //Inicia a variavel ntp
  timeClient.setTimeOffset(-10800); //Setta o Meridiano de Greenwich (Brasil = -10800)

  //-----------------------------------------------Começo função cão de guarda--------------------------------------------------
  // Configura o Watchdog Timer
  timer = timerBegin(0, 80, true); // timer 0, prescaler 80 (1MHz)
  timerAttachInterrupt(timer, &resetModule, true);
  timerAlarmWrite(timer, WATCHDOG_TIMEOUT * 90000000, false); // Alarme em microssegundos
  timerAlarmEnable(timer);
  //-----------------------------------------------Fim função cão de guarda--------------------------------------------------
}
void loop() {
  connectToWifi();
  timeClient.update();
  variaveisTempoReal4();
  variaveisTempoReal5();
  variaveisTempoReal6();
  gerarHistorico();
  
  //-----------------------------------------------Começo função cão de guarda--------------------------------------------------
  timerWrite(timer, 0); // Reinicia o contador do watchdog
  //-----------------------------------------------Fim função cão de guarda--------------------------------------------------

  Serial.println();
  delay(2000);
   if (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    WiFi.begin(WIFI_SSID);
  } //AQUI KOREIA TEM UM INTERVALO DE 2000 MILISEGUNDOS OU SEJA 2 SEGUNDOS ENT ELE TA PEGANDO OS DADOS ESPERANDO 2 SEGUNDOS E PEGANDO DNV SE QUISRE MUDAR AQUI PRA 1000
  // AI ELE VAI PEGAR DE 1 EM 1 SEGUNDO
  // 500 DE MEIO EM MEIO AI VC FAZ OS TESTES QUE QUISER 
  // É BOM DEIXAR LIGADO AQUI E IR DELISGANDO DE VEZ EM QUANDO
  // AI EU VI LA O GC008 QUANDO ELE TA LIGADO ELE PEGA 0.038 AMPERES E AQUI O
}

//-----------------------------------------------------------------------------------------------------------------
void variaveisTempoReal4() {

  float voltage4 = pzem4.voltage();
  float current4 = pzem4.current();
  float power4 = pzem4.power();
  float energy4 = pzem4.energy();
  float frequency4 = pzem4.frequency();
  float pf4 = pzem4.pf();
  
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/voltage", voltage4);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/current", current4);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/power", power4);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/frequency", frequency4);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/powerfactor", pf4);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO4) + "/energy/khw", energy4);
}
//-----------------------------------------------------------------------------------------------------------------
void variaveisTempoReal5() {

  float voltage5 = pzem5.voltage();
  float current5 = pzem5.current(); 
  float power5 = pzem5.power();
  float energy5 = pzem5.energy();
  float frequency5 = pzem5.frequency();
  float pf5 = pzem5.pf();
  
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/voltage", voltage5);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/current", current5);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/power", power5);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/frequency", frequency5);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/powerfactor", pf5);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO5) + "/energy/khw", energy5);
}
//-----------------------------------------------------------------------------------------------------------------
void variaveisTempoReal6() {

  float voltage6 = pzem6.voltage();
  float current6 = pzem6.current();
  float power6 = pzem6.power();
  float energy6 = pzem6.energy();
  float frequency6 = pzem6.frequency();
  float pf6 = pzem6.pf();
  
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/voltage", voltage6);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/current", current6);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/power", power6);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/frequency", frequency6);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/powerfactor", pf6);
  Firebase.setFloat(firebaseData, "/dispositivos/" + String(CODIGO6) + "/energy/khw", energy6);
}

void gerarHistorico() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime(); //EpochTime = 1664235476
  String formattedTime = timeClient.getFormattedTime(); // HH:MM:SS
  String weekDay = weekDays[timeClient.getDay()]; //Dia da semana
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday; //dia do mes numero = 26
  int currentMonth = ptm->tm_mon+1; //Mes numero = 9
  String currentMonthName = months[currentMonth-1]; //Nome do mes
  int currentYear = ptm->tm_year+1900; //Ano
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  String formattedHour = (currentHour < 10) ? "0" + String(currentHour) : String(currentHour);

  // Cria a string no formato "hh:00:00"
  String formattedTimeHour = formattedHour + ":00:00";

  String statusAtual4;
  String ultimoStatus4;
  float current4 = pzem4.current();
  
  if(current4 > 0.2){
    statusAtual4 = "ligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }
  if(current4 < 0.2){
    statusAtual4 = "desligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }

  if (Firebase.getString(firebaseData, "/dispositivos/" + String(CODIGO4) + "/ultimoStatus")) {  //On successful Read operation, function returns 1 
    ultimoStatus4 = firebaseData.stringData();

    if(ultimoStatus4 == "ligado" && statusAtual4 == "desligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO4) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current4);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO4) + "/ultimoStatus", "desligado");     
    }
    if(ultimoStatus4 == "desligado" && statusAtual4 == "ligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO4) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current4);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO4) + "/ultimoStatus", "ligado");  
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
  
  //-------------------------------------------------------------------------------------
  String statusAtual5;
  String ultimoStatus5;
  float current5 = pzem5.current();
  
  if(current5 > 0.2){
    statusAtual5 = "ligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }
  if(current5 < 0.2){
    statusAtual5 = "desligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }

  if (Firebase.getString(firebaseData, "/dispositivos/" + String(CODIGO5) + "/ultimoStatus")) {  //On successful Read operation, function returns 1 
    ultimoStatus5 = firebaseData.stringData();

    if(ultimoStatus5 == "ligado" && statusAtual5 == "desligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO5) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current5);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO5) + "/ultimoStatus", "desligado");     
    }
    if(ultimoStatus5 == "desligado" && statusAtual5 == "ligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO5) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current5);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO5) + "/ultimoStatus", "ligado");  
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }

  //---------------------------------------------------------------------------------

  float current6 = pzem6.current();
  String statusAtual6;
  String ultimoStatus6;
  
  if(current6> 0.2){
    statusAtual6 = "ligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }
  if(current6 < 0.2){
    statusAtual6 = "desligado";
    //Firebase.setString(firebaseData, "/dispositivos/" + /String(CODIGO3) + "/ultimoStatus", ultimoStatus);
  }

  if (Firebase.getString(firebaseData, "/dispositivos/" + String(CODIGO6) + "/ultimoStatus")) {  //On successful Read operation, function returns 1 
    ultimoStatus6 = firebaseData.stringData();

    if(ultimoStatus6 == "ligado" && statusAtual6 == "desligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO6) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current6);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO6) + "/ultimoStatus", "desligado");     
    }
    if(ultimoStatus6 == "desligado" && statusAtual6 == "ligado"){
      Firebase.setFloat(firebaseData, 
           "/dispositivos/" + String(CODIGO6) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTime) , current6);
      Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO6) + "/ultimoStatus", "ligado");  
    }
  } else {
    Serial.println(firebaseData.errorReason());
  }
  

  //----------------------------------------------------------------------------------------------------------------------------------------------------
  if (Firebase.getString(firebaseData, 
   "/dispositivos/" + String(CODIGO4) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour))) {  //On successful Read operation, function returns 1 
    Serial.println(firebaseData.stringData());
  } else {
    Firebase.setFloat(firebaseData, 
   "/dispositivos/" + String(CODIGO4) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour), current4);
  }
  
  if (Firebase.getString(firebaseData, 
   "/dispositivos/" + String(CODIGO5) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour))) {  //On successful Read operation, function returns 1 
    Serial.println(firebaseData.stringData());
  } else {
    Firebase.setFloat(firebaseData, 
   "/dispositivos/" + String(CODIGO5) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour), current5);
  }

   if (Firebase.getString(firebaseData, 
   "/dispositivos/" + String(CODIGO6) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour))) {  //On successful Read operation, function returns 1 
    Serial.println(firebaseData.stringData());
  } else {
    Firebase.setFloat(firebaseData, 
   "/dispositivos/" + String(CODIGO6) + "/energy_historic/" + String(currentYear) + "/" + currentMonthName + "/" + String(monthDay) + "/energy/" + String(formattedTimeHour), current6);
  }


  //----------------------------------------------------------------------------------------------------------------------------------------------------
  
  //ATUALIZA O A VARIAVEL ULTIMAATT NO BANCO DE DADOS!
  Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO4) + "/ultimaAtt", String(formattedTime));
  Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO5) + "/ultimaAtt", String(formattedTime));
  Firebase.setString(firebaseData, "/dispositivos/" + String(CODIGO6) + "/ultimaAtt", String(formattedTime));

  sendMessage(current4, currentHour, currentMinute, monthDay, "Espaço+Netuno+Sala+de+Reunião" );
  sendMessage(current5, currentHour, currentMinute, monthDay, "Espaço+Netuno+D");
  sendMessage(current6, currentHour, currentMinute, monthDay, "Espaço+Netuno+E");
  
}

void sendMessage(float current, int currentHour, int currentMinute, int dayOfMonth, String code) {
  // Verifica se o horário NÃO está entre 07:00:00 e 18:00:00
  if (!((currentHour > 7 && currentHour < 16) || (currentHour == 7 && currentMinute >= 0) || (currentHour == 16 && currentMinute == 0))) {
    // Verifica se a corrente é maior que 0.2
    if (current > 0.12) {
      // Verifica se é um novo dia do mês
      if (dayOfMonth != lastDayOfMonth) {
        messageSent = false; // Reset a flag se for um novo dia
        lastDayOfMonth = dayOfMonth; // Atualiza o último dia verificado
      }
      
      // Envia a mensagem se a flag for falsa
      //if (!messageSent) {


        HTTPClient http;
        String url = "https://api.callmebot.com/whatsapp.php?phone=553195626116&text=Ar-Condicionado+codigo+" + code + "+ligado++fora+da+hora&apikey=2243350";

        http.begin(url);
        int httpResponseCode = http.POST("");
        
        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String response = http.getString();
          Serial.println(response);
        } else {
          Serial.print("Error on HTTP request: ");
          Serial.println(httpResponseCode);
        }
        http.end();

        messageSent = true; // Marca a mensagem como enviada
      //}
    }
  }
}
