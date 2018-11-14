//version arduino_multimeter_premium_v_1.0.1
//Desarrollado por N.E.C.O 09/03/2018
//Variables para capacimetro*******************************************************
const byte pulsePin = 2;
const float resistance = 10000.0;

volatile boolean triggered;
volatile boolean active;

volatile unsigned long timerCounts;
volatile unsigned long overflowCount;

ISR (TIMER1_OVF_vect)
{
  ++overflowCount;               // count number of Counter1 overflows  
}  // end of TIMER1_OVF_vect

ISR (ANALOG_COMP_vect)
  {
  // grab counter value before it changes any more
  unsigned int timer1CounterValue;
  timer1CounterValue = TCNT1;  // see datasheet, page 117 (accessing 16-bit registers)
  unsigned long overflowCopy = overflowCount;
  
  if (active)
    {
    // if just missed an overflow
    if ((TIFR1 & bit (TOV1)) && timer1CounterValue < 256)
      overflowCopy++;
    // calculate total count
    timerCounts = (overflowCopy << 16) + timer1CounterValue;  // each overflow is 65536 more
    triggered = true;
    digitalWrite (pulsePin, LOW);  // start discharging capacitor
    }
  }  // end of ANALOG_COMP_vect
//*************************************************************
//*************************************************************
# define pin_lectura_Temp  A0
# define pin_lectura_Voltios  A1 
# define pin_lectura_Omios   A2 
# define pin_lectura_AMP   A3                                   // F formatter tells compliler it's a floating point value

//**************************
float sensor = 0.0 ;
//Value for Ammeter
//shunt_amp put here your resistance value
//shunt_amp поставте здесь наминал вашего сопротивления для измерения ампер
//shunt_amp reescribe por valor de tu resistencia shunt
float shunt_amp=0.015f;
//************************************************************************
float vout=0.0f;
float R1=0.0f;
float R2=0.0f;
int value=0;
int indexAfterPunto;
float result=0.0f;
char ch;
float voutv;
float vinv;
//This is the values for colibrate the accuracy
//For better accuracy test with a voltmetr pin "3.3v" and pin "5v" of arduino than set the exact value here 
//This value of volt reference if your volt reference is lower or heigher change this value for your value
//For exemple if youre volt reference is 3.2v you have to change this value(3.3v) by 3.2v
float const arefVolt=3.3f; //pin "3.3v" SET EXACT VALUE HERE
float vin=4.8f;//pin "5v"  SET EXACT VALUE HERE
////For better accuracy test resistance than set the exact values 
float Rref1=220;//Resistencia de 220 para medir la resistencia de 0 a 220 (Ω)**//**Resistance for test from 0 to 220 (oM)
float Rref2=10000;//Resistencia de 10K para medir la resistencia de 220 a 10000 (Ω)**//**Resistance for test from 220 to 10000 (oM)
float Rref3=102800; //Resistencia de 100K para medir la resistencia de 10000 a 100000 (Ω)**//**Resistance for test from 10000 to 100000 (oM)
//***************************************************Volts********************************************************
//For better accuracy test resistance than set the exact values !!!
//This resistance for test Volts
float resistencia1 = 101000; //Resistencia de 100K para medir la tencion (Voltios)/Resistance of 100k for test volts
float resistencia2 = 10000; //Resistencia de 10k para medir la tencion (Voltios)/Resistance 10k for test volts

#include <SoftwareSerial.h>

int const omios_1=10;
int const omios_2=1;
int const omios_3=2;
int const voltios=3;
int const temp=4;
int const beep=5;
int const amp=6;
int const cap=7;
int const ind=8;
int  test_count_inductance = 0;
int stateGenButton=0;

int bluetoothTx = 12;//bluetooth module terminal TX
int bluetoothRx = 11;//bluetooth module terminal RX
float temperature;
double pulse, frequency, capacitance, inductance,res_inductance;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);


void setup() {
 analogReference(EXTERNAL);
//Serial.begin(9600);
     bluetooth.begin(9600);
      pinMode(10, INPUT);
  pinMode(13, OUTPUT);
  pinMode (pulsePin, OUTPUT);
  digitalWrite (pulsePin, LOW);
// Serial.println ("Started.");
  ADCSRB = 0;           // (Disable) ACME: Analog Comparator Multiplexer Enable
  ACSR =  bit (ACI)     // (Clear) Analog Comparator Interrupt Flag
        | bit (ACIE)    // Analog Comparator Interrupt Enable
        | bit (ACIS0) | bit (ACIS1);  // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on rising edge)
  

}

void loop() {
 
  if(bluetooth.available()>0){
   ch=bluetooth.read();
 //  Serial.println(ch);
   if(ch == '1'){
     stateGenButton=omios_1;
       
} 
  if(ch == '2'){
     stateGenButton=omios_2;
} 
  if(ch == '3'){
     stateGenButton=omios_3;
} 
if(ch == 'V'){
     stateGenButton=voltios;
} 
if(ch == 'T'){
     stateGenButton=temp;
} 
if(ch == 'B'){
     stateGenButton=beep;
} 
 if(ch == 'A'){
     stateGenButton=amp;
}
 if(ch == 'F'){
     stateGenButton=cap;
}
 if(ch == 'K'){
     stateGenButton=ind;
    // Serial.println("Read inductance");
}

}

 stateButtonGen();

}
void stateButtonGen()
{
  switch(stateGenButton)
{
  
  case omios_1:
  lecturaOmios(omios_1);
  // Serial.println("omios_1");
  break;
   case omios_2:
  lecturaOmios(omios_2);
  // Serial.println("omios_2");
  break;
   case omios_3:
  lecturaOmios(omios_3);
  // Serial.println("omios_3");
  break;
  case voltios:lecturaVoltios();
  break;
   case temp:readTemp();
  break;
  case beep:lecturaBeep();
  break;
  case amp:lecturaAmp();
  break;
   case cap:lecturaCap();
  break;
   case ind:readInductance();
  break;

  
}
}
//Lectura OMIOS
void lecturaOmios(int state){
  setOmValue(state);
  for(int i=0;i<50;i++){
  value=analogRead(pin_lectura_Omios);

  vout=(value*arefVolt)/1023.0f;
  R2+=(R1*vout/(vin-vout));
//  Serial.println(value);
  if(value == 1023){
    R2=0.0f;
  }//Serial.println(R2);
 

delay(2);
}
R2=R2/50;
if(R2 > 100){
  indexAfterPunto=1;
}
 if(R2 > 1000) {
   indexAfterPunto=0;
}
if(R2 < 100){
   indexAfterPunto=2;
}
//Serial.println(R2);
bluetooth.print('<');
bluetooth.print(R2,indexAfterPunto);
bluetooth.print('>');
 bluetooth.println();
R2=0.0f;
delay(50);
}
//Lectura VOLTIOS
//Функция для измерения вольт
//Volt function
void lecturaVoltios(){
  vinv=0.0f;
  voutv=0.0f;
   for (int i=0;i < 100;i++){
  
   voutv = (analogRead(pin_lectura_Voltios) * arefVolt) / 1023;  //Lee el voltaje de entrada
  vinv += voutv / (resistencia2 / (resistencia1 + resistencia2));  //Fórmula del divisor resistivo para el voltaje final
  if(vinv < 0.9){
  vinv=0.0f;
  }
delay(1);

}
vinv=vinv/100;
  if(R2 > 100){
  indexAfterPunto=1;
}
 if(R2 > 1000) {
   indexAfterPunto=0;
}
if(R2 < 100){
   indexAfterPunto=2;
}
//Serial.println(vinv);
//Send volt value in this format example:<19.0>
bluetooth.print('<');
bluetooth.print(vinv,indexAfterPunto);
bluetooth.print('>');
 bluetooth.println();
vinv=0.0f;
delay(50);
}
//Lectura Inductance
 void readInductance()
{
 digitalWrite(13, HIGH);
  delay(5);//give some time to charge inductor.
  digitalWrite(13,LOW);
  delayMicroseconds(100); //make sure resination is measured
  
      pulse = pulseIn(10,HIGH,5000);//returns 0 if timeout
  
 // Serial.println(pulse);
 

  if(pulse > 0.1){ //if a timeout did not occur and it took a reading:
    
    
  //#error insert your used capacitance value here. Currently using 2uF. Delete this line after that
  capacitance = 1.E-6; // - insert value here
  
  
  frequency = 1.E6/(2*pulse);
  inductance = 1./(capacitance*frequency*frequency*4.*3.14159*3.14159);//one of my profs told me just do squares like this
  inductance *= 1E6; //note that this is the same as saying inductance = inductance*1E6
// Serial.println( inductance );
  if(test_count_inductance == 50){
//  Serial.print("\tinductance uH:");
res_inductance = res_inductance / 50;
//  Serial.println( res_inductance );
      bluetooth.print('<');
           bluetooth.print(res_inductance,0);
            bluetooth.print('>');
            bluetooth.println();
            test_count_inductance=0;
  delay(10);
  
}else{
  res_inductance += inductance;
  test_count_inductance++;
}


  }  else{
inductance = 0;
      bluetooth.print('<');
           bluetooth.print(inductance,0);
            bluetooth.print('>');
            bluetooth.println();
  delay(10);

  }  
           
} 
//Lectura TEMPERATURA
 void readTemp()
{
 
   int lectura = 0;
  for (int i=0; i<100; i++){ lectura= analogRead(pin_lectura_Temp);
  
            float voltaje = arefVolt /1023 * lectura ; // Atencion aquí
            // float temp = voltaje * 100 ; para el LM35DZ 
            temperature+= voltaje * 100-50 ;
            delay(2);
          }
          temperature=temperature/100;
          //Send temperature value in this format example:<17.0>
            bluetooth.print('<');
           bluetooth.print(temperature,0);
            bluetooth.print('>');
            bluetooth.println();
           // Serial.println(temperature,0);
            delay(50);       
           
} 
//Lectura BEEP
void lecturaBeep(){
  for(int i=0;i<5;i++){
  value=analogRead(pin_lectura_Omios);
 // Serial.println(value);
  vout=(value*arefVolt)/1023.0f;
  R2+=(R1*vout/(vin-vout));
 
 

delay(2);
}
R2=R2/5;
if(R2 > 100){
  indexAfterPunto=1;
}
 if(R2 > 1000) {
   indexAfterPunto=0;
}
if(R2 < 100){
   indexAfterPunto=2;
}
//Serial.println(R2);
// Sending to the app format message <value>
bluetooth.print('<');
bluetooth.print(R2,indexAfterPunto);
bluetooth.print('>');
 bluetooth.println();
R2=0.0f;
delay(50);
}
//***************************Amper***************************************
//Lectura Amper
//Считываем данные с аналогового входа ардуино и расчитываем по формуле ома миллиамперы
//На приложение отправляються данные в ввиде миллиампер
void lecturaAmp(){
   for (int i=0;i < 200;i++){
   voutv = (analogRead(pin_lectura_AMP) * arefVolt) / 1023.0f;  
  vinv += voutv;  
delay(1);

}

vinv=vinv/200.0f;
 vinv= vinv/shunt_amp; 
vinv= vinv * 1000; 
//Send amp value in this format example:<500>
//Enviamos datos en milliamperios ejemplo:<500>
//Ардуино отправляет на приложение данные в таком формате, Пример: <500>
bluetooth.print('<');
bluetooth.print(vinv);
bluetooth.print('>');
 bluetooth.println();
vinv=0.0f;
voutv=0.0f;
delay(50);
}
//*****************************************************************************
//***************************Condensadorу (Capasitor) Измерение Конденсаторов (В процессе разработки)(In development)(En desarrollo)****************************************
//Lectura Amper
//Считываем данные с аналогового входа ардуино и расчитываем по формуле ома миллиамперы
//На приложение отправляються данные в ввиде миллиампер

//*****************************************************************************
//************************************Capacictance Meter************************
void lecturaCap(){
      // start another test?
  if (!active)
    startTiming ();
    
  // if the ISR noted the time interval is up, display results
  if (active && triggered)
    finishTiming ();
}
//******************************************************************************
void setOmValue(int data){
  if(data == omios_1){
    R1=Rref1;
  }
  if(data == omios_2){
    R1=Rref2;
}
 if(data == omios_3){
    R1=Rref3;
}
}
void startTiming ()
  {
  active = true;
  triggered = false;

  // prepare timer
  overflowCount = 0;            // no overflows yet
  // reset Timer 1
  TCCR1A = 0;             
  TCCR1B = 0;   
  TCNT1 = 0;      // Counter to zero
  TIFR1 = bit (TOV1);  // clear overflow bit
  // Timer 1 - counts clock pulses
  TIMSK1 = bit (TOIE1);   // interrupt on Timer 1 overflow

  // get on with it
  digitalWrite (pulsePin, HIGH);  // start charging capacitor
  // start Timer 1, no prescaler
  TCCR1B =  bit (CS10);
    
  } // end of startTiming
  void finishTiming ()
  {
  active = false;
 // Serial.print ("Capacitance = ");
  float capacitance = (float) timerCounts * 1000.0 / 16.0 / resistance;
  bluetooth.print('<');
bluetooth.print(capacitance);
bluetooth.print('>');
 bluetooth.println();
 // Serial.print (capacitance);
 // Serial.println (" nF");
  triggered = false;
  delay (3000);
  }  // end of finishTiming



