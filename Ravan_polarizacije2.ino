/* Eksperiment: Merenje ravni polarizacije lasera
 * Autor:       Milos Gojkovic 
 * Datum:       26.jul 2021.
 * Mesto:       Beograd
 *  
 */
 //deklaracija ulaznih pinova
#define t1 PB1
#define t2 PB0
#define vcSense PA0
#define cSense PA1
#define vdSense PA2
#define R PB15
#define G PB14
#define B PB13
//deklaracija izlaznih pinova
#define igbt PB3
#define relay PB4
#define cState PB5
#define hvled PA8
#define ts 100          //vreme uzorkovanja u us
#define td 500          //vreme praznjenja u ms
#define samples 4000    //broj uzoraka
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
unsigned short isamp[samples],usamp[samples];
int led=0;
bool PUNI=false, SNIMAJ=false;
LiquidCrystal_I2C lcd(0x3f,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void setup() {
 lcd_init();
  //inicijalizacija pinova
 pin_init();
 //inicijalizacija COM porta
  Serial.begin(115200);
}
unsigned long ctd,cts,test;
void loop(){
  prazan();
  ceka_dugme1();
  puni_se();
  ceka_dugme2();
  pocinje_praznjenje();
  snimanje();
  praznjenje();
  slanje();
}
void lcd_init(){
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Fizicki fakultet");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("  Beograd,2022");
  delay(5000);
  lcd.clear();
}
void pin_init(){
  pinMode(t1,INPUT_PULLUP);
  pinMode(t2,INPUT_PULLUP);
  pinMode(vcSense,INPUT);
  pinMode(cSense,INPUT);
  pinMode(vdSense,INPUT);
  pinMode(igbt,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(cState,OUTPUT);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);

  digitalWrite(R,LOW);
  digitalWrite(G,LOW);
  digitalWrite(B,LOW);
  digitalWrite(igbt,LOW);
  digitalWrite(relay,LOW);
  digitalWrite(cState,LOW);
  
}
void prazan(){
  lcd.setCursor(0,1);
  lcd.print("Prazan   ");
  lcd.setCursor(11,0);
  lcd.print("D=OFF");
}
void ceka_dugme1()
{
    
  while(digitalRead(t1)==HIGH && PUNI==false)
  {
    if(Serial.available())
  {
    citaj();
  }
    float u=analogRead(vcSense)*332.0/4096.0;
    lcd.setCursor(0,0);
     lcd.print("Uc=");
      if(u<100)
    lcd.print(" ");
    if(u<10)
    lcd.print(" ");
    lcd.print(u);
    lcd.print("V ");
    float ud=analogRead(vdSense)*3.320/(4096.0*0.33);
    lcd.setCursor(7,1);
    lcd.print("Ud=");
    lcd.print(ud,3);
    lcd.print("V");
    
    if(digitalRead(t2)==LOW){
      led++;
      if(led==4)
      led=0;
      promeni_led();
     delay(100);
    }
  }
  PUNI=false;
}
void ceka_dugme2()
{
  while(digitalRead(t1)==HIGH && SNIMAJ==false)
  {
    if(Serial.available())
  {
    citaj();
  }
    float u=analogRead(vcSense)*332.0/4096.0;
    lcd.setCursor(0,0);
    lcd.print("Uc=");
    if(u<100)
    lcd.print(" ");
    if(u<10)
    lcd.print(" ");
    lcd.print(u);
    lcd.print("V ");
  
    if(digitalRead(t2)==LOW){
      led++;
      if(led==4)
      led=0;
      promeni_led();
     delay(100);
    }
  }
   SNIMAJ=false;
}
void puni_se()
{
  float u;
  digitalWrite(relay,HIGH);
  digitalWrite(cState,HIGH);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Punjenje        ");
  Serial.println("Punjenje u toku");
  do{
    u=analogRead(vcSense)*332.0/4096.0;
    lcd.setCursor(0,0);
    lcd.print("Uc=");
    lcd.print(u);
    lcd.print("V ");
    Serial.print(u,2);
    Serial.println("V");
     delayMicroseconds(10);
    
  }
  while (u<300);
  lcd.setCursor(0,1);
  lcd.print("Napunjen!       ");
  Serial.println("Napunjen!");
   delayMicroseconds(10);
  
}
int pom;
void snimanje(){
  test=micros();
  cts=micros();
  pom=cts;
  for(int i=0;i<samples;i++)
  {
    pom+=(ts/2);
    usamp[i]=analogRead(vdSense);
    while(micros()<pom)
    continue;
    
    pom+=(ts/2);
    isamp[i]=analogRead(cSense);
    while(micros()<pom)
    continue;
    
  } 
  test=micros()-test;
}
void slanje(){
   Serial.println(led);
    delayMicroseconds(10);
   Serial.print("Vreme uzorkovanja[us]:");
    delayMicroseconds(10);
   Serial.println(test);
    delayMicroseconds(10);
   float samp;
  for(int i=0;i<samples;i++){
    samp=(usamp[i])*3.32/4096;
  Serial.println(samp, 3);
  delayMicroseconds(10);
  }
  Serial.println("*******************************************************");
   for(int i=0;i<samples;i++){
   samp=(isamp[i]-2048)*3.320/4096;
   samp=samp/0.1016;
  Serial.println(samp,2);
  delayMicroseconds(10);
  }
 
delay(10);
}
void pocinje_praznjenje(){
  digitalWrite(relay,LOW);
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Merenje u toku  ");
  Serial.println("Merenje....");
  delayMicroseconds(10);
  ctd=millis();
  digitalWrite(igbt,HIGH);
}
void praznjenje(){
  while((millis()-ctd)<td)
  {
    continue;
  }
  digitalWrite(igbt,LOW);
  digitalWrite(cState,LOW);
}
void promeni_led()
{
  digitalWrite(R,LOW);
      digitalWrite(G,LOW);
      digitalWrite(B,LOW);
      lcd.setCursor(11,0);
      switch(led){
        case 0:  lcd.print("D=OFF");Serial.println("D=OFF");{break;}
        case 1:  {digitalWrite(R,HIGH); lcd.print("D=R  ");Serial.println("D=R");break;}
        case 2:  {digitalWrite(G,HIGH); lcd.print("D=G  ");Serial.println("D=G");break;}
        case 3:  {digitalWrite(B,HIGH); lcd.print("D=B  ");Serial.println("D=B");break;}
      }
       delayMicroseconds(10);
}
void citaj(){
     String text;
     text=Serial.readString();    
     if(text=="R\n"){led=1;promeni_led();}
     if(text=="G\n"){led=2;promeni_led();}
     if(text=="B\n"){led=3;promeni_led();} 
     if(text=="OFF\n"){led=0;promeni_led();}
     if(text=="NAPUNI\n"){PUNI=true;} 
     if(text=="START\n"){SNIMAJ=true;}
      delayMicroseconds(10);
      }
    
