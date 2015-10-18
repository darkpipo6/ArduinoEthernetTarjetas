#include <SPI.h>
#include <String.h>
#include <Ethernet.h>
#include <RFID.h>
#include <SD.h> 
#define SS_PIN 53
#define RST_PIN 5
#define fila 200
#define contenido 50
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>

LiquidCrystal_I2C lcd(0x27,16,2); 

RFID rfid(53, 5);
RFID rfid2(22, 24);
int numero_serie[5];

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Mac del servidor
byte ip[] = { 192,168,1,34}; // Ip del servidor
EthernetServer server(80); // Puerto de servicio 

int led = 8; // Led de prueba
String readString = ""; // string para recibir datos de la web
boolean statusLed = false; // estatus del led
char Fichero[fila][contenido];



void setup(){
//  almacenar();
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  Ethernet.begin(mac, ip);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  rfid.init();
  delay(50);
  rfid2.init();
  lcd.init();
  lcd.backlight();
  delay(50);
  Serial.print("Initializing SD card...");
  if (!SD.begin(53)) {
         Serial.println("initialization failed!");
         lcd.setCursor(0,0);
         lcd.print("Iniciando..."); 
         lcd.setCursor(0,1);
         lcd.print("Estado: Error");
         delay(3000);
         lcd.clear();       
       return;  
                    }
       else{
       Serial.println("initialization done.");
       lcd.setCursor(0,0);
       lcd.print("Iniciando..."); 
       lcd.setCursor(0,1);
       lcd.print("Estado : OK");
       delay(3000);
       lcd.clear();
       return;     
           }          
}

void loop(){
  mensaje(); 
  leer_usuario();
           
 EthernetClient client = server.available();
  
  if (client) {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        // ler caractere por caractere vindo do HTTP
        if (readString.length() < 40)
        {
          // armazena os caracteres para string
          readString += (c);
        }
        
        //se o pedido HTTP terminou
        if (c == '\n')
          {
          // se verifica que el led este encendido
          if(readString.indexOf("Onled")>=0)
          {
            // Encender Led
            digitalWrite(led, HIGH);
            Serial.println(readString);
            Serial.println(readString);
            statusLed = true;
          }
          // Led apagado
          if(readString.indexOf("Offled")>=0)
          {
            // O Led vai ser desligado
            digitalWrite(led, LOW);
            Serial.println(readString);
            statusLed = false;
          }
          if(readString.indexOf("BorrarRegistros")>=0)
          {
           borrar();
          }
          if(readString.indexOf("Registrar")>=0)
          {
           Serial.println(readString);
           readString.replace("GET /:80/?","");
           readString.replace(" HTTP/1.1","");
           readString.replace("Registrar","");
           readString.replace("\n","");
           Serial.println(readString);
           Serial.println("Acerque su tarjeta");
           String aux = verificar();
           digitalWrite(26,LOW);
           if(aux=="1"){
           Serial.println("Este usuario no puede registrarse!");                                
           }else{
           Serial.println("Este usuario puede registrarse!");
           Serial.println(aux);
           readString = readString + aux;
           readString.replace("\n","");
           readString.replace(" ","");
           Serial.println(readString);
           File myFile;
           myFile = SD.open("test.txt", FILE_WRITE);
           if (myFile) {
              myFile.println(readString);
              myFile.println("=========================");
              myFile.close();
              digitalWrite(27,HIGH);
              delay(500);
              digitalWrite(27,LOW);
              delay(500);
              digitalWrite(27,HIGH);
              delay(500);
              digitalWrite(27,LOW);
              delay(500);
              digitalWrite(27,HIGH);
              delay(500);
              digitalWrite(27,LOW);             
              Serial.println("Se ah registrado al usuario");
              Serial.println("done.");              
                      } else {
              Serial.println("error opening test.txt");
              Serial.println("No se ah registrado al usuario"); 
                             }
           
           }
          Serial.println("Salio del Loop");                      
          readString="";
          }                 
         
        // llamada html
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();       
        client.print("<font size='20'>");
        if (statusLed) {
          client.print("Led encendido");
        } else {
          client.print("Led apagado");
        }
        
        //limpiar string para la proxima lectura

        readString="";
        
        // parar cliente
        client.stop();
        }
      }
    }
  }
}

void leer_usuario()
{
    if (rfid.isCard())
    {
           if (rfid.readCardSerial())
            {
                for(int i=0; i<4 ; i++)
                {
                  numero_serie[i] = rfid.serNum[i];
                }
                String myString = String(rfid.serNum[0]) +String(rfid.serNum[1])+String(rfid.serNum[2])+String(rfid.serNum[3]) ;                        
                
                String datos;
                datos = LeerRegistros(myString);
                                             
                if (datos.indexOf(myString) >= 0) {
                 lcd.setCursor(0,0);
                 lcd.print("Usuario"); 
                 lcd.setCursor(0,1);
                 lcd.print("Registrado");
                 Serial.println("Este Usuario esta registrado 1 ");
                 Serial.println(myString);                
                 delay(1000);
                 lcd.clear();
                 lcd.print("Bienvenido"); 
                 lcd.setCursor(0,1);
                 lcd.print(myString);
                 delay(1000);
                 lcd.clear();
                 rfid.halt();                                                                             
                }               
                lcd.setCursor(0,0);
                lcd.print("Usuario"); 
                lcd.setCursor(0,1);
                lcd.print("No registrado");
                Serial.println("Este Usuario no esta registrado 1");
                Serial.println(myString);
                delay(1000);
                lcd.clear();
                rfid.halt();                    
            }
    }
            rfid.halt();
}

String leer_usuario2()
{
    if (rfid2.isCard())
    {
      
           if (rfid2.readCardSerial())
            {
                for(int i=0; i<4 ; i++)
                {
                  numero_serie[i] = rfid2.serNum[i];
                }
                String myString = String(rfid2.serNum[0]) +String(rfid2.serNum[1])+String(rfid2.serNum[2])+String(rfid2.serNum[3]) ;                           

                String datos;
                datos = LeerRegistros(myString);
 
                if (datos.indexOf(myString) >= 0) {
                Serial.println("Este Usuario esta registrado 2");
                Serial.println(myString);
                rfid2.halt();
                return "1";               
                }                               
                Serial.println("Este Usuario no esta registrado 2");
                Serial.println(myString);              
                rfid2.halt();
                return myString;                                    
             }
    }
            rfid2.halt();
            return "2";
}

void mensaje(){
       lcd.setCursor(0,0);
       lcd.print("Esperando"); 
       lcd.setCursor(0,1);
       lcd.print("Tarjeta...");
}


void borrar(){
SD.remove("test.txt");
delay(50);
File myFile;
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("Nombre Alumno: Felipe Flores");
    myFile.println("Universidad: Universidad San Sebastian");    
    myFile.println("=================== Tesis de grado ===============");   
    myFile.println("=========================");   
    myFile.println("llavero143107270");
    myFile.println("=========================");
    myFile.println("tarjeta2351349469");
    myFile.println("=========================");
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }   
  
}
String LeerRegistros(String tag){
   File myFile;
   char dato;
   String Fila ="";
   myFile = SD.open("test.txt");  
   if (myFile){  
   while (myFile.available()) {
   dato = char(myFile.read());
   if(dato!='\n'){
   if (Fila.indexOf(tag) >= 0){
   return Fila; 
   }else{
   Fila = "";
   }
   }else{
   Fila +=dato;
   }    
   }
   return "0";
   myFile.close();
   }else{
   Serial.println("error opening test.txt");
   }
}
String verificar (){ //depurador
int x=0;
digitalWrite(26,HIGH); //led que indica que la tarjeta esta esperando datos
String variable; 
  while (x=!0){
  variable = leer_usuario2();
  if(variable =="1"){
  Serial.println("Se salio en el 1");
  return "1";
  }
  if(variable != "1" && variable !="2"){
  Serial.println("Se salio en el 2");
  return variable;
  }
  Serial.println("Acerque su tarjeta");
  }
}

