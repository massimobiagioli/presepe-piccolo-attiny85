/*
 * ---PRESEPE PICCOLO---
 * Circuito con ATTINY85
 * Massimo Biagioli 05/12/2015
 */
 
#define PIN_CALIB 0         // Pin calibrazione
#define PIN_BUTTON 1        // Pin switch button per cambio programma
#define PIN_PHOTORES A2     // Pin fotoresistenza 

#define THRESHOLD_OFF 100   // Valore di soglia per luce spenta

#define PGM_FOTORES 0       // Programma #1: luce in funzione della fotoresistenza
#define PGM_ALWAYS_ON 1     // Programma #2: luce sempre accesa
#define PGM_ALWAYS_OFF 2    // Programma #3: luce sempre spenta
#define PGM_INTERM 3        // Programma #4: luce a intermittenza

int v_photores;
int vmax_photores;
int vmin_photores;
int v_calib;
int v_set;
int v_old;
int button_state_old;
int current_pgm;

void setup() {
  pinMode(PIN_CALIB, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_PHOTORES, INPUT);
  
  vmax_photores = 0;
  vmin_photores = 1024;
  v_old = 0;

  button_state_old = LOW;  
}

void loop() {
  int button_state = digitalRead(PIN_BUTTON);

  if (button_state == LOW) {
    if (button_state_old == HIGH) {
      
      // Cambia programma alla pressione e successivo rilascio del pulsante
      change_pgm();      
    }

    // Imposta programma
    switch(current_pgm) {
      case PGM_FOTORES:
        pgm_fotores();
        break;
      case PGM_ALWAYS_ON:
        pgm_always_on();
        break;
      case PGM_ALWAYS_OFF:
        pgm_always_off();
        break;
      case PGM_INTERM:
        pgm_interm();
        break;
    }
  }

  button_state_old = button_state;  
}

/*
 * Cambia programma
 */
void change_pgm() {
  switch(current_pgm) {
    case PGM_FOTORES:
      current_pgm = PGM_ALWAYS_ON;            
      break;
    case PGM_ALWAYS_ON:
      current_pgm = PGM_ALWAYS_OFF;
      break;
    case PGM_ALWAYS_OFF:
      current_pgm = PGM_INTERM;
      break;
    case PGM_INTERM:
      current_pgm = PGM_FOTORES;
      v_set = 0;
      break;
  }
}

/*
 * Programma #1: luce in funzione della fotoresistenza
 */
void pgm_fotores() {  
  v_photores = analogRead(PIN_PHOTORES);
  
  if (v_photores > vmax_photores) vmax_photores = v_photores;
  if (v_photores < vmin_photores) vmin_photores = v_photores;  
  
  v_calib = map(v_photores, vmax_photores, vmin_photores, 0, 255);      
  v_set = regola(v_calib);
  analogWrite(PIN_CALIB, v_set);
  v_old = v_set;    

  delay(50);
}

/*
 * Programma #2: luce sempre accesa
 */
void pgm_always_on() {
  analogWrite(PIN_CALIB, 255);
}

/*
 * Programma #3: luce sempre spenta
 */
void pgm_always_off() {
  analogWrite(PIN_CALIB, 0);
}

/*
 * Programma #4: luce a intermittenza
 */
void pgm_interm() {
  v_set = (v_set == 0 ? 255 : 0);
  analogWrite(PIN_CALIB, v_set);  
  delay(100);
}

/*
 * Effettua regolazione della luce in funzione del valore di soglia impostato
 * e del valore letto dalla resistenza
 */
int regola(int v) {
  return (v > THRESHOLD_OFF) ? 255 : 0; 
}

