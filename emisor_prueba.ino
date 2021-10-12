#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10
#define pos_x A0
#define pos_y A1
 
//Variable con la direcciÃ³n del canal por donde se va a transmitir
byte direccion[5] ={'d','q','q','n','s'};

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector con los datos a enviar
const int numero_botones = 7;
int botones_estado[numero_botones];
int botones_pines[numero_botones]= {2, 3, A3 , 4, 5, 6, 7};
long unsigned int tiempo_botones[numero_botones];
long unsigned int actual_time;
long unsigned int tiempo_enviado;

float datos[2+numero_botones];

void setup()
{
  pinMode(pos_x, INPUT);
  pinMode(pos_y, INPUT);
  for (int i=0; i<numero_botones; i++){
    pinMode(botones_pines[i], INPUT_PULLUP);
    botones_estado[i] = 0;
  }
  tiempo_enviado = millis();
  //inicializamos el NRF24L01 
  radio.begin();
  //inicializamos el puerto serie
  Serial.begin(9600); 
 
//Abrimos un canal de escritura
 radio.openWritingPipe(direccion);

  // Inicializar tiempo
 for (int i=0; i<numero_botones; i++){
  tiempo_botones[i] = millis();
 }
}
/* 
 *  0. x
 *  1. Y
 *  2. Caja derecha
 *  3. Caja izquierda
 *  4. Play music
 *  5. Loco izquierda
 *  6. Loco derecha
 */

void detectar_botones(){
  for (int i=0; i<numero_botones; i++){
   if (digitalRead(botones_pines[i])==LOW && botones_estado[i]==0 && ((actual_time-tiempo_botones[i])>300)){
    datos[i+2] = 1;
    botones_estado[i] = 1;
    tiempo_botones[i] = millis();
   }
   else{
    datos[i+2] = 0;
    if ((actual_time-tiempo_botones[i]>300) && digitalRead(botones_pines[i])==HIGH){
      botones_estado[i] = 0; 
    }
   }
 }  
}

void loop()
{
 actual_time = millis(); 
 datos[0]=(float)analogRead(pos_x)*1023/880;
 datos[1]=(float)analogRead(pos_y)*1023/880;
 
 //enviamos los datos
 if (actual_time-tiempo_enviado>200){
  tiempo_enviado = millis();
  detectar_botones();
  bool ok = radio.write(datos, sizeof(datos));
  Serial.print("Datos enviados: "); 
     for (int i=0; i<2+numero_botones; i++){
       Serial.print(datos[i]);
       if (i!=1+numero_botones){
          Serial.print(" , "); 
       }
       else{
          Serial.println(" ");
       }
     }
  //reportamos por el puerto serial los datos enviados 
  if(ok)
  {
     Serial.print("Datos enviados: "); 
     for (int i=0; i<2+numero_botones; i++){
       Serial.print(datos[i]);
       if (i!=1+numero_botones){
          Serial.print(" , "); 
       }
       else{
          Serial.println(" ");
       }
     }
     
  }
  else
  {
     Serial.println("no se ha podido enviar");
  } 
 }
}
