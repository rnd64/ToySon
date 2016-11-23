/*
 *  Simple ultrasonic MIDI controller
 *  Version 0.1
 *  http://RnD64.com, 2016
 */

// 
// Set MIDI_ENABLED to generate MIDI output instead of Serial debug messages
// This program uses hardware Serial port as output MIDI (115200, 8, n, 1)
// See for reference HS-SR04 http://www.micropik.com/PDF/HCSR04.pdf

#define MIDI_ENABLED 1

#if (MIDI_ENABLED)
  #include <MIDI.h>
#endif  

// 
// Global constants and parameters
//

const byte echoPin = 8; // Digital input pin to received echo from HS-SR04
const byte trigPin = 9; // Digital output pin to set trigger signal to HS-SR04
const byte TRIG_DURATION = 10; // usec, duration of control trigger signal in accordingly with HS-SR04 technical documatation
const int ECHO_TIMEOUT = 8000; // usec, timeout to wait of echo back pulse 
const unsigned int MIN_ECHO_DURATION = 200; // echo from closer objects will be ignored
const unsigned int MAX_ECHO_DURATION = 2200; // echo from more far objects will be ignored. In general, this is a maximum distance limitation. Use suitable values to avoid unwanted echo noise
const byte MEASUREMENT_COUNT = 10; // count of echo probes for each position to encrease accuracy of measurement

//
// MIDI parameters and configuration
//

#if (MIDI_ENABLED)
MIDI_CREATE_DEFAULT_INSTANCE();
const byte MIDI_CHANNEL = 1; // MIDI channel to send all notes to PC
const byte MAX_MIDI_VELOCITY = 127; // Default velocity (loudness) for notes pitch
#endif
const unsigned int LOW_NOTE = 44; // Lowest note pitch to generate, 64 = E3 (1 quitar open string), 44 = E1 (6 quitar open string)   
const unsigned int HIGH_NOTE = 64+12; // Highest note pitch to generate, first string, 12th quitar fret 
const unsigned long MIN_NOTE_INTERVAL = 5000; //usec, minimum interval between two notes folowing each other, just additional protection from overloading of MIDI receiver

//
// Global variables
//

float noteStep; // calculated range of echo duration per one note from notes range
unsigned int prevNote = 0; // stored active note pitch, note is off if value is 0
unsigned long lastNoteTime = 0; // last note switching on event, usec (Arduino micros() is used to control)


/*
 *  Setup of controller
 */
void setup()
{
    #if (MIDI_ENABLED)
    MIDI.begin(MIDI_CHANNEL);          // Launch default instance of MIDI on choosed channel
    #endif
    Serial.begin(115200); // 115200 is maximum rate for Hireless Midi Serial middleware
    // setup HS-SR04 pins
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);
    // calculate default duration rate per note, will used to converting the duration into note pitch
    noteStep = float(MAX_ECHO_DURATION-MIN_ECHO_DURATION)/float(HIGH_NOTE-LOW_NOTE);
    #if (!MIDI_ENABLED)    
      Serial.println("Simple MIDI controller started");
      Serial.println("Step="+String(noteStep)+" duration per one note");
    #endif  
} // Setup end



/*
 *  get_echo_duration() try to measure echo time (MEASUREMENT_COUNT times) by HS-SR04 and return avarage duration value
 */
 unsigned long get_echo_duration() {
    
  unsigned long echoDuration = 0;
  
  for (int count = 0; count < MEASUREMENT_COUNT; count++) {
      // setup trigger of HS-SR04 to start measuring
      digitalWrite(trigPin, HIGH); 
      delayMicroseconds(TRIG_DURATION); 
      digitalWrite(trigPin, LOW); 
      // wait until timeout and get echo duration from sensor for current measuring if success
      echoDuration += pulseIn(echoPin, HIGH, ECHO_TIMEOUT); 
  }   
  // calc avarage value
  echoDuration =+ echoDuration / MEASUREMENT_COUNT;
  // in accordingly with HS-SR04 docs if echo back > 38 ms there is no obstacle
  if (echoDuration > 37000) echoDuration = 0; 
  return echoDuration;
  
 } // end get_echo_duration()



/*
 *  Loop procedure
 */
 
void loop() {

  unsigned long echoDuration;
  unsigned int newNote = 0;

  // get avarage echo duration from HS-SR04
  echoDuration = get_echo_duration();
  
  if (echoDuration) {
    // if value isn't zerro check the min and max distance and limit range     
    echoDuration = min(MAX_ECHO_DURATION, max(MIN_ECHO_DURATION, echoDuration));
    // calc new note pitch basing on duration rate and notes scale
    newNote = LOW_NOTE + int( (echoDuration - MIN_ECHO_DURATION) / noteStep);
    
    if ( (newNote != prevNote) and (abs(micros()-lastNoteTime) > MIN_NOTE_INTERVAL) ) {
          // react only if note is changed and requered time interval is elapsed after previouse note
           #if (MIDI_ENABLED)
              if (prevNote) {
                  // if previouse note is active - switch off it to avoid MIDI receiver overloading
                  MIDI.sendNoteOff(prevNote,0, MIDI_CHANNEL);  
              }
              // start playing of new note with maximum velocity 
              MIDI.sendNoteOn(newNote,MAX_MIDI_VELOCITY,MIDI_CHANNEL);
           #else 
              // MIDI isn't enabled so just debug messages
              Serial.println("Start note="+String(newNote)+ ", echo duration="+String(echoDuration));
           #endif
           // update last event time and active note info
           lastNoteTime = micros();           
           prevNote = newNote;
    }  
    
  } else if (prevNote) {
    // we have received duration=0, so currently no hand over device OR hand is very far,
    // in this case ust stop current active note
    
    #if (MIDI_ENABLED)
         MIDI.sendNoteOff(prevNote,0, MIDI_CHANNEL);
    #else
        Serial.println("Stop note="+String(prevNote));
    #endif
         
    prevNote = 0;
  }
  
 
} // loop END