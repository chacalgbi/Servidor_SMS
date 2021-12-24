void restart_(){
  yield();      //Alimenta o Watchdog de Software. 3 Segundos.
  ESP.wdtFeed();//Alimenta o Watchdog de Hardware. 8 Segundos.
}

void iniciar_PINs(){
  pinMode(reset_sms,  OUTPUT); digitalWrite(reset_sms,  LOW);
  
  pinMode(config_wifi, INPUT);
}

void barra_progresso(int p1, int p2, int p3, int p4, int p5, int tempo){
  for (int x=0; x<p5; x++){ // p5 é o valor total da barra
    display1.drawProgressBar(p1, p2, p3, p4, x); // Onde começa a barra a esquerda
                                                 // Linha onde a barra vai ficar
                                                 // Onde finaliza a barra a Direita
                                                 // Largura da Barra
                                                 // Valor a ser mostrado na progressao da barra
    display1.display();
    delay(tempo);
  }
}

void iniciar_display(){
  display1.init();
  display1.flipScreenVertically();
  display1.setTextAlignment(TEXT_ALIGN_LEFT);
  display1.setFont(ArialMT_Plain_16);
  display1.drawString(0, 0, "Iniciando...");
  display1.display();
  barra_progresso(0, 30, 127, 10, 100, 10);
  display1.clear();
  display1.display();
}

void mostrar_display(String t1, String t2, String t3, String t4, byte fonte, float segundos){
  int tempo = segundos * 1000;
  display1.clear();
  display1.setTextAlignment(TEXT_ALIGN_CENTER);
  if(fonte == 10){display1.setFont(ArialMT_Plain_10);}
  if(fonte == 16){display1.setFont(ArialMT_Plain_16);}
  display1.drawString(64,  0, t1);
  display1.drawString(64, 16, t2);
  display1.drawString(64, 32, t3);
  display1.drawString(64, 48, t4);
  display1.display();
  delay(tempo);
}

void converter_strings(){
  tamanho_rede  = REDE.length();
  tamanho_senha = SENHA.length();
  tamanho_nome  = NOME.length();
  
  REDE.toCharArray (ssid,  tamanho_rede);
  SENHA.toCharArray(pw,    tamanho_senha);
  NOME.toCharArray(device, tamanho_nome);
  
}
 
void formatFS(void){
  LittleFS.format();
}

void createFile(void){
  File wFile;
 
  //Cria o arquivo se ele não existir
  if(LittleFS.exists("/log.txt")){
    mostrar_display("Arquivo", "já", "existe", "", 16, 1);
  } else {
    mostrar_display("Criando", "Arquivo", "", "", 16, 1);
    wFile = LittleFS.open("/log.txt","w+");
 
    //Verifica a criação do arquivo
    if(!wFile){
      mostrar_display("Erro ao", "criar arquivo", "", "", 16, 1);
    } else {
      mostrar_display("Arquivo", "criado", "com", "sucesso", 16, 1);
    }
  }
  wFile.close();
}
 
void deleteFile(void) {
  //Remove o arquivo
  if(!LittleFS.remove("/log.txt")){
    mostrar_display("Erro ao", "remover", "arquivo", "", 16, 1);
    
  } else {
    mostrar_display("Arquivo", "removido", "com", "sucesso", 16, 1);
  }
}
 
void writeFile(String msg) {
 
  //Abre o arquivo para adição (append)
  //Inclue sempre a escrita na ultima linha do arquivo
  File rFile = LittleFS.open("/log.txt","a+");
 
  if(!rFile){
    mostrar_display("Erro ao", "abrir", "arquivo", "", 16, 1);
  } else {
    rFile.println(msg);
  }
  rFile.close();
}
 
void readFile(void) {
  byte cont = 1;
  File rFile = LittleFS.open("/log.txt","r");
  mostrar_display("Lendo", "Arquivo", "SPIFFS", "", 16, 0.1);
  while(rFile.available()) {
    if(cont == 1){REDE  = rFile.readStringUntil('\n'); }
    if(cont == 2){SENHA = rFile.readStringUntil('\n'); }
    if(cont == 3){NOME  = rFile.readStringUntil('\n'); }
    cont++;
    delay(10);
  }
  rFile.close();
}
 
void closeFS(void){
  LittleFS.end();
}
 
void openFS(void){
  //Abre o sistema de arquivos
  if(!LittleFS.begin()){
    mostrar_display("Erro ao", "abrir", "sistema de", "Arquivos", 16, 1);
  } else {
    mostrar_display("Sistema de", "arquivos", "aberto com", "sucesso", 16, 0.1);
  }
}

void handleRoot() {
  //Go to http://192.168.4.1 in a web browser
String html  = "<!DOCTYPE html><head><html lang='pt-br'><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Configurar WIFI</title>";
       html += "</head><body align='center'>";
       html += "<h3>CONFIGURAÇÃO<br> WIFI</h3>";
       html += "<form method='POST' action='/form'>";
       html += "<input type=text placeholder='DIGITE O NOME DA REDE'  name=txt_ssid /><br><br>";
       html += "<input type=text placeholder='DIGITE A SENHA DA REDE' name=txt_pw   /><br><br>";
       html += "<input type=text placeholder='NOME DO LOCAL'          name=txt_nome /><br><br>";
       html += "<input type=submit name=botao value=Enviar />";
       html += "</form>";
       html += "</body></html>";
server.send(200, "text/html", html);
}

void formularioEnviado() {
String html =  "<!DOCTYPE html><head><html lang='pt-br'><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1'><title>WIFI Configurado</title></head><body align='center'><h3>Seu dispositivo foi<br> configurado</h3>";
       html += "<h1><font color='red'>SSID: ";
       html += server.arg("txt_ssid");
       html += "</h1></font><br><h1><font color='green'>SENHA: ";
       html += server.arg("txt_pw");
       html += "</font></h1></body></html>";
       server.send(200, "text/html", html);

  deleteFile();
  delay(100);
  createFile();
  delay(100);     

  REDE  = server.arg("txt_ssid"); REDE.trim();  writeFile(REDE);
  SENHA = server.arg("txt_pw");   SENHA.trim(); writeFile(SENHA);
  NOME  = server.arg("txt_nome"); NOME.trim();  writeFile(NOME);

  delay(1000);
  ESP.restart();
  delay(1000);
}

bool conecta_wifi_blynk(char* ssid_, char* pw_){
  if(WiFi.status() != WL_CONNECTED){
    int contador = 0;
    WiFi.disconnect(); 
    delay(500);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(ssid_, pw_);
    delay(100);
    String x = "";
    while ( contador <= 20) {
      contador++;
      x = String(contador) + " de 20";
      mostrar_display("Conectando", "WIFI", "", x, 16, 0);
      restart_();
      if (WiFi.status() == WL_CONNECTED){ contador = 50; break; }
      delay(1000);
    }
    if (WiFi.status() == WL_CONNECTED){
      mostrar_display("WIFI", "", "Conectado", "", 16, 0.5);
      return true;
    }
    else{
      mostrar_display("Erro", "", "ao conectar", "WIFI", 16, 2);
      return false;
    }
  }
  return true;
}

void chama_conecta_wifi_blynk(){
  TemWifi = conecta_wifi_blynk(ssid, pw);
}

String IntensidadeSinalWifi(){
  int sinal = WiFi.RSSI();
  String sinal_str = "";

  if (sinal >= -65) {
    sinal_str = "Excelente";
  } else if (sinal >= -67) {
    sinal_str = "Muito Bom";
  } else if (sinal >= -70) {
    sinal_str = "Aceitavel";
  } else if (sinal >= -80) {
    sinal_str = "Ruim";
  } else if (sinal >= -99) {
    sinal_str = "Muito ruim";
  } else {
    sinal_str = "Extr. ruim";
  }

  return String(sinal) + "dBm " + sinal_str; 
}

String IP(){
  String ip = WiFi.localIP().toString().c_str();
  return ip; 
}
