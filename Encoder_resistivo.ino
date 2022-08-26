/*

                                               ,#@@@@@@@@@  
                      ./@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
                                                       @@   
                .   (///////////,                     /@    
               ,   /@@@@@@@@@@@@@@@@@@@@(             @     
@@@@@@@@@@@@@@  @@ @@@@@@@@@@@@@@@@@@@@@@@@          %@     
@@@@@@@@@@@@@(@ @@(@@@@@@           @@@@@@@@@        @      
@@@@@@@@@@@@@  @@ @@@@@@@             @@@@@@@@      @(      
             @   %@@@@@@               @@@@@@@ @@@@ @ @@@@@@
            @    @@@@@@@               @@@@@@@ @@@.@ @@@@@@@
            @   #@@@@@@                @@@@@@@ @@@ @,@@@@@@@
@@@@@@@@@@.@ @@ @@@@@@@               @@@@@@@     @         
@@@@@@@@@@*@ @@@@@@@@@              @@@@@@@@      .         
@@@@@@@@@ @ @@ @@@@@@@           @@@@@@@@@@      @          
         @@   &@@@@@@@@@@@@@@@@@@@@@@@@@                    
         @    @@@@@@@@@@@@@@@@@@@@@@            /           
        @@                                                  
       &@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&(.                 
       @@@@@@@@@%.                                          

       
  |D2: Encoder_DT|
  |D3: Encoder CLK|
  |D4: X9C103 CS|
  |D5: X9C103 U/D|
  |D6: X9C103 INC|

*/

#include <LapX9C10X.h>
#include <EEPROM.h>

#define CSPIN 4
#define ud_outPin 5
#define inc_outPin 6

LapX9C10X led(inc_outPin, ud_outPin, CSPIN, LAPX9C10X_X9C103);

int A = 2;      //variable A a pin digital 2 (DT en modulo)
int B = 3;      //variable B a pin digital 3 (CLK en modulo)

const int loopPeriod = 50;  //Loop period equals three times of this value, ex) 50 * 3 = 150ms

int ANTERIOR = 0;    // almacena valor anterior de la variable POSICION

int POSICION = 0; // variable POSICION con valor inicial de 50 y definida
        // como global al ser usada en loop e ISR (encoder)
void setup() {
  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada

  pinMode(inc_outPin, OUTPUT);
  pinMode(ud_outPin, OUTPUT);

  digitalWrite(inc_outPin, HIGH);
  digitalWrite(ud_outPin, HIGH);
  
  Serial.begin(9600);   // incializacion de comunicacion serie a 9600 bps
  Serial.println("Starting, set to minimum resistance");  

  EEPROM.get(0, POSICION);

  led.begin(-1);

  attachInterrupt(digitalPinToInterrupt(A), encoder, LOW);// interrupcion sobre pin A con
                // funcion ISR encoder y modo LOW
  Serial.println("Listo");  // imprime en monitor serial Listo
}


void loop() {
  
  if (POSICION != ANTERIOR) { // si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION); // imprime valor de POSICION
    ANTERIOR = POSICION ; // asigna a ANTERIOR el valor actualizado de POSICION
  }

  led.set(POSICION);
  led.writeNVM();
}

void encoder()  {
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
            // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
              // pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
    {
    POSICION++ ;
    EEPROM.put(0, POSICION);
    digitalWrite(ud_outPin, HIGH);
    delay(loopPeriod);
    digitalWrite(inc_outPin, LOW);
    delay(loopPeriod);
    digitalWrite(inc_outPin, HIGH);
    delay(loopPeriod);
    }
    else if (digitalRead(B) == LOW){          // si B es LOW, senti anti horario
    POSICION-- ;
    EEPROM.put(0, POSICION);
    digitalWrite(ud_outPin, LOW);
    delay(loopPeriod);
    digitalWrite(inc_outPin, LOW);
    delay(loopPeriod);
    digitalWrite(inc_outPin, HIGH);
    delay(loopPeriod);
    }
    else{
      delay(loopPeriod*3);
    }

    POSICION = min(100, max(0, POSICION));  // establece limite inferior de 0 y
            // superior de 100 para POSICION
    ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
  }           // de la interrupcion en variable static
}
