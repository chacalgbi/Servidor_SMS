// Variáveis da configuração WIFI - SPIFFS
String REDE="";  char ssid[30];   int tamanho_rede = 0;  String ssid_str = "";
String SENHA=""; char pw[30];     int tamanho_senha = 0; String pw_str = "";
String NOME="";  char device[30]; int tamanho_nome = 0;  String device_str = "";

String wifi = "Wifi OFF";
bool modo_config = false;
bool serial_ativo  = false;
bool TemWifi = false;
int conecta_WIFI = 0;
int long ultimo_tempo = 0;
const int intervalo = 10000;
