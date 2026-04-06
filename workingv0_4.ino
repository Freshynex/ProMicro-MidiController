#include "MIDIUSB.h"

const int NUM_KEYS = 12;

const int NUM_POTS = 1;

const byte keyPins[NUM_KEYS] =   { 2,  3,  4,  5,  6,  7,  8,  9,  21, 20, 19, 15 };
const byte midiNotes[NUM_KEYS] = { 48, 50, 51, 52, 53, 55, 54, 57, 49, 56, 59, 58 };
//                                 C   D   D#  E   F   G   F#  A   C#  G#  B   A#

const int potPins[NUM_POTS] = { A0 };

const int octKeyPins[2] = { 10, 16 };

// 48 = middle C

// update when key is pressed or released
int lastKeyState[NUM_KEYS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int lastPotState[NUM_POTS] = { 127 };

int lastOctKeyState[2] = { 0, 0 };

byte octave = 0;

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}

void sendHiResCC(byte channel, byte control, int value) {
  byte lsb = value & 0x7F;
  byte msb = (value >> 7) & 0x7F;

  controlChange(channel, control, msb);
  controlChange(channel, control + 32, lsb);
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_KEYS; i++) {
    pinMode(keyPins[i], INPUT_PULLUP);
    digitalWrite(i, 1);
  }

  pinMode(10, INPUT);
  pinMode(16, INPUT);
}

int num = 0;

void loop() {

  bool anyPotChanged = false;
  bool anyKeyChanged = false;  // A flag to tell us if we need to flush the USB

  // Loop through all 12 keys
  for (int i = 0; i < NUM_KEYS; i++) {
    int currentState = digitalRead(keyPins[i]);

    // Check if this specific key changed state
    if (currentState != lastKeyState[i]) {
      Serial.print(i);
      Serial.println(". key changed");
      Serial.print("which is pin ");
      Serial.println(keyPins[i]);
      int pitchValue = midiNotes[i] + (octave * 12);

      if (currentState == 0) {
        // Key was PRESSED

        noteOn(0, pitchValue, 64);
        //Serial.print(i); Serial.println(". on");
        Serial.print("sending ");
        Serial.println(midiNotes[i]);
      } else {
        // Key was RELEASED
        noteOff(0, pitchValue, 64);
        //Serial.print(i); Serial.println(". off");
      }

      lastKeyState[i] = currentState;

      anyKeyChanged = true;
    }

    if (anyKeyChanged) {

      MidiUSB.flush();
      //delay(15);
    }
  }

  // same as key for pots
  for (int i = 0; i < NUM_POTS; i++) {

    int currentPotState = analogRead(potPins[i]);

    if (abs(currentPotState - lastPotState[i]) >= 2) {
      Serial.println(currentPotState);
      int midiValue = map(currentPotState, 0, 1023, 0, 16383);  // scale up 10 bit analog input to 14 bit for MIDI CC message MSB and LSB
      Serial.println(midiValue);
      sendHiResCC(0, 16, midiValue);
      lastPotState[i] = currentPotState;

      anyPotChanged = 1;
    }
  }

  if (anyPotChanged) {
    MidiUSB.flush();
    //delay(2);  // A single 15ms debounce delay covers all keys at once
  }

  for (int i = 0; i < 2; i++) {
    int currentState = digitalRead(octKeyPins[i]);
    if (currentState != lastOctKeyState[i]) {
      lastOctKeyState[i] = currentState;
      if (currentState == 1) {
        if (i == 0 && octave > -4) {
          octave -= 1;
          Serial.println("Decreasing octave");
        } else {
          if (i == 1 && octave < 6) {
            octave += 1;
            Serial.println("Increasing octave");
          }
        }
        Serial.print("Octave: ");
        Serial.println(octave);
      }
    }
  }
}
