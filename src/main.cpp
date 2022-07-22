#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "uart.h"
#include <stdlib.h>
#include "DHT.h"

enum {automatico, manual} estado;

//FUNCIONES
void interrupt_INT0_Init(){
 cli(); //DESH INT
 EICRA = 0b11; // INT FLANCO DE SUBIDA
 EIMSK = 1; // HAB PIN0, para habilitar pin2 ---> 2
 sei(); //HAB INT
}



ISR(INT0_vect){
 if (estado==automatico){
    estado=manual;
 }
 else{
    estado=automatico;
 }
}
#define DHTPIN 8

#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);


char* mensaje = "";

void setup() {
  // put your setup code here, to run once:
  DDRD = 0b11111000;
 DDRB = 0b111;
 PORTD |= (1<<PD2); //Activar resistencia de PULLUP en PIN0 del puerto D
 interrupt_INT0_Init();
 serial_begin();
 dht.begin();
 estado = automatico;
 
}

void loop() {
  // put your main code here, to run repeatedly:
  float t = dht.readTemperature();
switch (estado)
{
case automatico:
   PORTB &= (1<<PB1);
   if((t>28)&&(t<34)){
      PORTD &= (1<<PD4); 
   }
   else if(t<=28){
      PORTD &= (1<<PD6);
   }
   else if(t>=34){
      PORTD |= (1<<PD7);
   }
   break;

case manual:
   PORTB &= (1<<PB2);
   serial_println_str("Bienvenido a Sistemas Embebidos");
   serial_println_str("Ingresa una opcion");
   serial_println_str("azul, verde, rojo");
   while (!is_data_ready()){_delay_ms(20);}
   mensaje = get_RX_buffer();

   if (!strcmp(mensaje,"azul")){
      PORTD |= (1<<PD7);
   }
   else if (!strcmp(mensaje,"verde")){
      PORTD |= (1<<PD4);
   }
   else if (!strcmp(mensaje,"rojo")){
      PORTD |= (1<<PD6);
   }
   break;
}
}