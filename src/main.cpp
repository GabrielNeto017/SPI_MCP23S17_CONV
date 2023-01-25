#include <Arduino.h>
#include<SPI.h>



#define CS 53
#define IODIRA 0x00
#define IODIRB 0x01
#define GPIOA  0x12
#define GPIOB  0x13
#define IORDIRB 0x01

const uint8_t INITIAL_ADDRESS= 0x20;
const uint8_t READ_OPERATION = (INITIAL_ADDRESS <<1 | 0x01);
const uint8_t WRITE_OPERATION = (INITIAL_ADDRESS <<1 | 0x00);
uint8_t portBState = 0;
uint8_t portAState = 0; //bit wise




void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  pinMode(CS, OUTPUT);                
  digitalWrite(CS, HIGH);            
  SPI.begin();

  //configure portB as output
  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0));
  digitalWrite(CS,LOW);
  SPI.transfer(WRITE_OPERATION);
  SPI.transfer(IODIRB);
  SPI.transfer(0x00);
  digitalWrite(CS,HIGH);
  delay(0.1);
    
  //configure portA as 00001111
  digitalWrite(CS,LOW);
  SPI.transfer(WRITE_OPERATION);
  SPI.transfer(IODIRA);
  SPI.transfer(0xF0);
  digitalWrite(CS,HIGH);
  SPI.endTransaction(); 
}


void loop() 
{
  // put your main code here, to run repeatedly:

  //Leitura expansor 4 chaves
 
  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0));
  digitalWrite(CS,LOW);
  SPI.transfer(READ_OPERATION);
  SPI.transfer(GPIOA);
  portAState=SPI.transfer(0);
  portAState &= 0xF0; 
  digitalWrite(CS,HIGH);
  SPI.endTransaction(); 
  delay(10);


  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0));
  digitalWrite(CS,LOW);
  SPI.transfer(WRITE_OPERATION);
  SPI.transfer(GPIOA);
  //uint8_t bcd = (portAState);  
  SPI.transfer(portAState>>4);
  digitalWrite(CS,HIGH);
  SPI.endTransaction(); 
  delay(10);

  float leituraAD = analogRead(A0);
  delay(100);
  int corrente= map(leituraAD, 208,1023,4,20);
  float tensao= (((5*leituraAD)-1040)/815.0);
  Serial.print("O valor da corrente e: ");
  Serial.print(corrente);
  Serial.println("mA");

  
  int leds= tensao/0.625;
  uint8_t leds_write = 0x00;

  for(int x=0;x<=leds;x++){
    leds_write=leds_write>>1;
    leds_write =(leds_write|0x80);   
  }
  //Serial.print("palavra esrtia foi: ");
  //Serial.println(leds_write,BIN);

  //write portB 
  SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0));
  digitalWrite(CS,LOW);
  SPI.transfer(WRITE_OPERATION);
  SPI.transfer(GPIOB);
  SPI.transfer(leds_write);
  digitalWrite(CS,HIGH);
  SPI.endTransaction(); 
}