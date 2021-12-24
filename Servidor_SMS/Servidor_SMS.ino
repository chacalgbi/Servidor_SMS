#include "definicoes.h"
#include "variaveis.h"
#include "funcoes_gerais.h"
//(77) 9.9831-5535
// AT+CBC    Bateria
//AT+CMTE?   Temperatura

// sudo dmesg | grep tty  // vê seriais no terminal

unsigned int total = 0;
unsigned int sucesso = 0;
unsigned int erro = 0;

void SMS(String number, String text){
  int tempo = 200;
  Serial.print(F("AT+CMGF=1\r"));
  delay(tempo);
  Serial.println("AT+CNMI=2,2,0,0,0");
  delay(tempo);
  Serial.println("ATX4");
  delay(tempo);
  Serial.println("AT+COLP=1");
  delay(tempo);
  Serial.println("AT+CMGDA=\"DEL ALL\"");
  delay(tempo);
  Serial.print(F("AT+CMGS=\""));
  delay(tempo);
  Serial.print(number);
  Serial.print(F("\"\r"));
  delay(tempo);
  Serial.print(text);
  Serial.print("\r");
  delay(tempo);
  Serial.print((char)26);
  delay(tempo);
}

void tempo(){ // Aguarda 12 segundos para se conectar na rede GSM
  mostrar_display("Resetando", "Modulo", "SMS", "...",16, 0);
  digitalWrite(reset_sms, LOW);  delay(1000);  digitalWrite(reset_sms, HIGH); // reset modulo SIM800L
 
  for (int i = 0 ; i < 10; i++){ // Dá o tempo de se conectar
    mostrar_display("Iniciando", "modulo SMS", "", String(i) + "/10",16, 0);
    delay(1000);
    restart_();
  }
}

void configuraGSM(){
  mostrar_display("Configurando", "", "SIM800L", "",16, 0);
   Serial.println("AT+CMGF=1"); // Ativamos a função de envio de SMS
   delay(200);
   Serial.println("AT+CNMI=2,2,0,0,0");
   delay(200);
   Serial.println("ATX4");
   delay(200);
   Serial.println("AT+COLP=1");
   delay(200);
   Serial.println("AT+CMGDA=\"DEL ALL\""); // Apagar todas os SMS estando modo texto
   delay(200);
   Serial.println("AT+IPR=115200");
   delay(200);
}

void config_rede(){
  WiFi.softAP("Servidor_SMS");
  server.on("/", handleRoot);
  server.on("/form", HTTP_POST, formularioEnviado);
  server.begin();
  mostrar_display("Modo CONFIG", "", "Ativo", "", 16, 0);

}

void iniciar_OTA(){
  ArduinoOTA.setHostname("Servidor_SMS");
  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() { LittleFS.end(); mostrar_display("", "Atualizando...", "", "", 16, 0); });
  ArduinoOTA.onEnd([]()   { mostrar_display("Fim da", "Atualização", "", "", 16, 0); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
    unsigned int progress1;
    if(progress1 != progress){ progress1 = progress;}
    mostrar_display("", "Atualizando...", "", "", 16, 0);
    });
  ArduinoOTA.onError([](ota_error_t error) { mostrar_display("ERROR", " ao", "Atualizar", "", 16, 0); });
}

void tratarCorpo(){
  restart_();
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) { 
    server.send(200, "text/plain", "ERRO - Arquivo Mal Formatado");
    mostrar_display("ERRO", "Arquivo mal", "formatado", "", 16, 0);
    return;
    }

  uint64_t limite = 20000;
  String str = "";
  const char* key = doc["key"];
  const char* numero = doc["numero"];
  const char* mensagem = doc["msg"];

  String teste_key = key;

  if (teste_key != "QMDCGQ889B"){
    mostrar_display("Token:", "Invalido", teste_key, "", 16, 0);
    server.send(200, "text/plain", "ERRO - Token Invalido");
    return;
  }
  
  String a = "Total: " + String(total);
  String b = "   OK: " + String(sucesso);
  mostrar_display("Env SMS", numero, a, b, 16, 0);
  
  SMS(numero, mensagem);

  uint64_t timeOld1 = millis(); 
  while ( !Serial.available() && !(millis() > timeOld1 + limite) ){ delay(10); restart_(); }
  while(Serial.available()){ if(Serial.available()>0){ str += (char)Serial.read(); } }
  
  delay(2000);
  str = "";

  uint64_t timeOld2 = millis();
  while ( !Serial.available() && !(millis() > timeOld2 + limite) ){ delay(100); restart_(); }  
  while(Serial.available()){ if(Serial.available()>0){ str += (char)Serial.read(); } }

  total++;

  if(str.indexOf("ERRO") != -1 ){
    server.send(200, "text/plain", "ERRO - SMS nao enviado"); 
    mostrar_display("ERROR", "SMS nao", " Enviado", "", 16, 0);
    erro++;
    }
  else if(str.indexOf("OK") != -1 || str.indexOf("CMGS") != -1){
    server.send(200, "text/plain", "OK - SMS Enviado");
    mostrar_display("OK", "SMS Enviado", "", "", 16, 0);
    sucesso++;
    }
  else{ 
    server.send(200, "text/plain", "Tempo Esgotado na resposta do SMS");
    mostrar_display("Tempo", "", "Esgotado", "", 16, 0);
    }

  ultimo_tempo = millis();
}

void normal(){
  readFile();
  converter_strings();
  TemWifi = conecta_wifi_blynk(ssid, pw);

  //Se não conseguir conectar a rede ao iniciar, reseta o ESP8266
  if (WiFi.status() != WL_CONNECTED){ ESP.restart(); }
 
  iniciar_OTA();
  tempo();
  configuraGSM();
 
  server.on("/teste", tratarCorpo);
  server.begin();
  mostrar_display("", "", "", "", 16, 0);
  ultimo_tempo = millis();
}

void normal_loop(){
  restart_();
  ArduinoOTA.handle();
  server.handleClient(); 
  //AoReceberSMS();

  if(millis() - ultimo_tempo > intervalo){
    ultimo_tempo = millis();
    conecta_WIFI++;
    if(TemWifi){ wifi= "Wifi ON"; }else{ wifi= "Wifi OFF";}
    String a = "Total: " + String(total);
    String b = "   OK: " + String(sucesso);
    String c = " Erro: " + String(erro);
    mostrar_display(IP(), a, b, c, 16, 0);
    
    if(conecta_WIFI > 120){
      conecta_WIFI = 0;
      chama_conecta_wifi_blynk();
     }
  }
}

void setup() {
  iniciar_PINs();
  if(!digitalRead(config_wifi)){modo_config = true;} // Iniciar normalmente ou modo Config
  iniciar_display();
  Serial.begin(115200);
  delay(100);
  openFS(); delay(500);
  WiFi.disconnect(); delay(500);
  if(modo_config == true){config_rede();} else {normal();}
}

void loop() {
  if(modo_config){
    server.handleClient();
  }
  else {
    normal_loop();
  }
}
