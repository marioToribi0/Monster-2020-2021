#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
#include <Servo.h>
#define rightServoPin 2
#define leftServoPin 3

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
//Declaremos los pines CE y el CSN
#define CE_PIN 9
#define CSN_PIN 10
 
//Variable con la dirección del canal que se va a leer
byte direccion[5] ={'c','a','n','a','l'}; 

//creamos el objeto radio (NRF24L01)
RF24 radio(CE_PIN, CSN_PIN);

//vector para los datos recibidos
const int numero_botones = 7;
float datos[2+numero_botones];

// Include the servo library:
#include <Servo.h>
// Create a new servo object:
Servo rightServo;
Servo leftServo;
int time_servo = 200;

#define pinNano A5

// Define the servo pin:
#define rightServoPin 2
#define leftServoPin 3
#define pausePin 28      // Este pin no hace nada, solo para detener el movimiento del servo

// Pines motores
const int velocity[4] = {5,6,7,8};
const int motores_high[4] = {46,42,45,48};
const int motores_low[4] = {47,43,44,49};
const int start_button = 26;
const int colores[3] = {13, 12, 11};
int position_color = 0;
const int stop_time = 800;

/*
 * Leds
 * 0. Azul
 * 1. Verde 
 * 2. Rojo
 ***************
 * Trasera 1. 46,47
 * Delantera 1. 42, 43
 * Delantera 2. 45, 44
 * Trasera 2. 48, 49
 * 
  */


void setup()
{
  pinMode(pinNano, OUTPUT);
   //inicializamos el NRF24L01 
  radio.begin();
  //inicializamos el puerto serie
  Serial.begin(9600); 

  //Abrimos el canal de Lectura
  radio.openReadingPipe(1, direccion);
  
    //empezamos a escuchar por el canal
  radio.startListening();
  
  for (int i=0; i<4; i++){
    pinMode(motores_high[i], OUTPUT);
    pinMode(motores_low[i], OUTPUT);
    pinMode(velocity[i], OUTPUT);
    analogWrite(velocity[i], 200);
  }
  
  // Comunicación
  Serial.begin(9600);   // Inicializamos  el puerto serie  
  ////Serial.println("Inicio");

  //....
  pinMode(start_button, INPUT_PULLUP);
  for (int i=0; i<3; i++){
    pinMode(colores[i], OUTPUT);
    digitalWrite(colores[i], LOW);
  }
  
  for (int i=0; i<3; i++){
    digitalWrite(colores[i], LOW);
  }
  digitalWrite(colores[position_color], HIGH);
  position_color += 1;
  if (position_color>=3){
    position_color = 0; 
  }
}
 
 void cambiar_color(){
 for (int i=0; i<3; i++){
      digitalWrite(colores[i], LOW);
    }
    digitalWrite(colores[position_color], HIGH);
    position_color += 1;
    if (position_color>=3){
      position_color = 0; 
    }
}

void girar_goma(int numero, String direccion){
  if (direccion == "delante"){
    digitalWrite(motores_high[numero], LOW);
    digitalWrite(motores_low[numero], HIGH);
  }
  else if(direccion == "atras"){
    digitalWrite(motores_high[numero], HIGH);
    digitalWrite(motores_low[numero], LOW);    
  }
}

void delante(){
  detener();
 for (int i=0; i<4; i++){
    girar_goma(i, "delante");
  }
}
void atras(){
  detener();
  for (int i=0; i<4; i++){
    girar_goma(i, "atras");
  }
}
void giro_izquierdo(){
  detener();
  for (int i=0; i<2; i++){
    girar_goma(i, "delante");
  }
  for (int i=2; i<4; i++){
    girar_goma(i, "atras");
  }
}
void giro_derecho(){
  detener();
  for (int i=0; i<2; i++){
    girar_goma(i, "atras");
  }
  for (int i=2; i<4; i++){
    girar_goma(i, "delante");
  }
}
void izquierda(){
  detener();
  girar_goma(0, "delante");
  girar_goma(1, "atras");
  girar_goma(2, "delante");
  girar_goma(3, "atras");
}
 
void izquierda_arriba(){
  detener();
  girar_goma(0, "delante");
  girar_goma(2, "delante");
}
void izquierda_abajo(){
  detener();
  girar_goma(0, "atras");
  girar_goma(2, "atras");
}
void derecha(){
  detener();
  girar_goma(0, "atras");
  girar_goma(1, "delante");
  girar_goma(2, "atras");
  girar_goma(3, "delante");  
}
void derecha_arriba(){
  detener();
  girar_goma(1, "delante");
  girar_goma(3, "delante");
}
void derecha_abajo(){
  detener();
  girar_goma(1, "atras");
  girar_goma(3, "atras");  
}

void detener(){
  for (int i=0; i<4; i++){
    digitalWrite(motores_high[i], LOW);
    digitalWrite(motores_low[i], LOW);
  }
}

char convertir_valor(float value_x, float value_y){
  if (value_x<300 && (value_y>300 && value_y<600)){
        return 'd'; //Izquierda
      }
      else if (value_x>600 && (value_y>400 && value_y<600)){
        return 'b'; // Derecha
      }
      else if (value_y<400 && (value_x>400 && value_x<600)){
        return 'a'; // Arriba

      }
      else if (value_y>600 && (value_x>400 && value_x<600)){
        return 'c';  // Abajo
      } 
      else if (value_x>600 && value_y>600){
        // Diagonal inferior derecha
        return '1';
      }
      else if (value_x>600 && value_y<400){
        // Diagonal superior derecha
        return 'k';
      }
      else if (value_x<400 && value_y<400){
        // Diagonal superior izquierda
        return 'n';
      }
      else if (value_x<400 && value_y>600){
        // Diagonal izquierda inferior
        return 'm';
      }
      else{
        return '0';
      }
}

void modo_loco_izquierdo(){
   giro_izquierdo();
   delay(500);
   detener();
}

void modo_loco_derecho(){
   giro_derecho();
   delay(500);
   detener();
}

// Esta funcion identifica la direccion
char direccion_anterior = ' ';

void mover_servo(String tipo, int direccion){
  if (tipo=="right"){
   if (direccion==1){
      rightServo.attach(rightServoPin);
      rightServo.write(0);
      digitalWrite(rightServoPin, 1);
      delay(time_servo);
      // Apagar
      rightServo.attach(pausePin);
      digitalWrite(rightServoPin, 0);
   }
   else{
      rightServo.attach(rightServoPin);
      rightServo.write(180);
      digitalWrite(rightServoPin, 1);
      delay(time_servo);
      // Apagar
      rightServo.attach(pausePin);
      digitalWrite(rightServoPin, 0);
   }
  }
  else{
    leftServo.attach(leftServoPin);
    leftServo.write(0);
    delay(time_servo);
    leftServo.attach(pausePin);
  }
}

void play_music(){
  //////Serial.println("Musica");
  Serial.print("1"); 
}

void locura_derecha(){
  giro_derecho();
  play_music();
  delay(5000);
  detener();
}

void locura_izquierda(){
  giro_izquierdo();
  play_music();
  delay(5000);
  detener();
}

void detectar_botones(){
  
/* 
 *  0. x
 *  1. Y
 *  2. Caja derecha
 *  3. Caja izquierda
 *  4. Play music
 *  5. Loco izquierda
 *  6. Loco derecha
 */
 
 if (datos[2]==1.0){
   play_music();
   
 }
 if (datos[3]==1.0){
   mover_servo("right", 1);  
 }

 if (datos[4]==1.0){
  mover_servo("left", 1);  
 }
 
 if (datos[5]==1.0){
   modo_loco_izquierdo();
 }
 
 if (datos[6]==1.0){
   modo_loco_derecho(); 
 }
 if (datos[7]==1.0){
   locura_izquierda(); 
 }
 if (datos[8]==1.0){
   locura_derecha(); 
 }
 
}

void identificar_direccion(){
  char direccion;
  radio.read(datos,sizeof(datos));
  direccion = convertir_valor(float(datos[0]), float(datos[1]));
  
  //Serial.println(direccion);
  
  if (direccion_anterior!=direccion){
    direccion_anterior = direccion;
    // Detectar conexion
    if(direccion == 'a'){
      delante();
    }
    else if(direccion == 'c'){
      atras();
    }
    else if (direccion == 'k'){
      derecha_arriba();
    }
    else if (direccion == '1'){
      izquierda_abajo();
    }
    else if (direccion == 'n'){
      izquierda_arriba();
    }
    else if (direccion == 'd'){
      izquierda();
    }
    else if (direccion == 'b'){
      derecha();
    }
    else if (direccion == 'm'){
      derecha_abajo();
    }
    else if (direccion == '0'){
      detener();
      delay(400);
    }
    else if (direccion == 'z'){
      giro_izquierdo();
    }
    else if (direccion == 'x'){
      giro_derecho();
    }
    if (direccion!='0'){
      cambiar_color();
    }
  }
}

bool mode_monster = false;
void loop()
{
  /*
  if (digitalRead(start_button)==LOW && mode_monster==false){
    //Serial.println("Modo monster");
    delay(5000);
    delante();
    unsigned long int last_time = millis();
    unsigned long int actual_time = millis();
    while(actual_time - last_time < stop_time){
     actual_time = millis();  
    }
    detener();
    mode_monster = true;
  }
  if (digitalRead(start_button)==HIGH && mode_monster==true){
    //Serial.println("Reiniciar");
    mode_monster = false;
    delay(300);
  }
  // Si se puede leer
  if(BT.available())
  {
    identificar_direccion();
    
    for (int i=0; i<3; i++){
      digitalWrite(colores[i], LOW);
    }
    digitalWrite(colores[position_color], HIGH);
    position_color += 1;
    if (position_color>=3){
      position_color = 0; 
    }
  }
  */
    // Si se puede leer
  if(radio.available())
  {
    identificar_direccion();
    detectar_botones();
      //Serial.print("Datos enviados: "); 
     for (int i=0; i<2+numero_botones; i++){
       //Serial.print(datos[i]);
       if (i!=1+numero_botones){
          //Serial.print(" , "); 
       }
       else{
          //Serial.println(" ");
       }
     }
   
  }
/*
  if (Serial.available()){
   int var = int(char(Serial.read())-'0');
   //Serial.println(var);
   girar_goma(var, "delante");
   delay(4000);
   detener();
  Serial.read();
  }
  */

}
