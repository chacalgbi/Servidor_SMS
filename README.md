# Servidor_SMS
Sistema com ESP8266 + Módulo SIM800L + API para enviar MSGs em massa pra clientes. Usando apenas um chip de SMS ilimitado.

Exemplo de requisição com NODEjs:

var axios = require("axios").default;

var options = {
  method: 'POST',
  url: 'http://172.17.1.187/teste', // IP local/teste
  headers: {'Content-Type': 'application/json'},
  data: {
    key: 'QMDCGQ889B', // Senha que está no código do microcontrolador ESP8266
    numero: '77988188514', // Número que receberar o SMS
    msg: 'SMS te teste!' // Corpo da mensagem. Não deve conter acentos e max 160 caracteres.
  }
};

axios.request(options).then(function (response) {
  console.log(response.data);
}).catch(function (error) {
  console.error(error);
});
