#include <Arduino.h>

#define PWM_PIN 9  // PWM output pin

volatile boolean sendSample = false;
uint32_t phAcc = 0;

// Initial frequency setup
uint32_t fOut = 100;  // Default frequency in Hz
uint32_t tuningWord;

#define SAMPLE_RATE 9091.0  // Approximate sample rate from Timer2 configuration

// 8-bit sine wave LUT (256 samples)
uint8_t LUT[256] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
  176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
  218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
  245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
  255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246,
  245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
  218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
  176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
  128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97,  93,  90,  88,  85,  82,
  79,  76,  73,  70,  67,  65,  62,  59,  57,  54,  52,  49,  47,  44,  42,  40,
  37,  35,  33,  31,  29,  27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,
  10,  9,   7,   6,   5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,
  0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,
  10,  11,  12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,
  37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,  73,  76,
  79,  82,  85,  88,  90,  93,  97,  100, 103, 106, 109, 112, 115, 118, 121, 124
};

// Function to update frequency and recalc the tuning word
void updateFrequency(uint32_t newFreq) {
  fOut = newFreq;
  // Calculate tuning word: (2^32 * frequency) / SAMPLE_RATE
  tuningWord = (uint32_t)((4294967296.0 * fOut) / SAMPLE_RATE);
}

void setup() {
  // Set PWM_PIN as output
  pinMode(PWM_PIN, OUTPUT);
  Serial.begin(9600);

  // Set up Timer1 for 8-bit Fast PWM generation on PWM_PIN
  cli();  // Disable interrupts
  TCCR1A = (1 << COM1A1) | (1 << WGM10);  // Fast PWM 8-bit, non-inverting on OC1A (pin 9)
  TCCR1B = (1 << WGM12) | (1 << CS10);     // No prescaler (16 MHz clock)
  // Start with mid-point duty cycle
  OCR1A = 128;
  sei();  // Enable interrupts

  // Set the default frequency and tuning word
  updateFrequency(fOut);

  // Set up Timer2 to trigger an interrupt at approximately 9091 Hz
  cli();  // Disable interrupts while configuring Timer2
  // Set Timer2 to CTC mode (Clear Timer on Compare Match)
  TCCR2A = (1 << WGM21);
  // Set prescaler to 8 (CS21 = 1)
  TCCR2B = (1 << CS21);
  // Calculate OCR2A: f = F_CPU / (prescaler * (OCR2A + 1))
  // With F_CPU = 16 MHz and prescaler = 8, setting OCR2A to 220 gives f ~ 16e6 / (8 * 221) ≈ 9091 Hz.
  OCR2A = 220;
  // Enable Timer2 Compare Match A interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();  // Enable interrupts
}

void loop() {
  // Check if new frequency is sent over Serial (e.g., from a Python script)
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();  // Remove extra whitespace/newlines
    uint32_t newFreq = data.toInt();
    
    // Validate frequency range (e.g., 100 Hz to 1000 Hz)
    if (newFreq >= 100 && newFreq <= 1000) {
      updateFrequency(newFreq);
      Serial.print("Updated Frequency: ");
      Serial.println(fOut);
    }
  }

  // When the Timer2 ISR sets the flag, update the PWM duty cycle
  if (sendSample) {
    // The upper 8 bits of the 32-bit phase accumulator form the index for the sine LUT
    uint8_t index = (phAcc >> 24);
    // Update PWM duty cycle using the sine lookup table
    OCR1A = LUT[index];
    // Increment phase accumulator by the tuning word
    phAcc += tuningWord;
    // Clear flag until the next interrupt
    sendSample = false;
  }
}

// Timer2 Compare Match A Interrupt Service Routine
ISR(TIMER2_COMPA_vect) {
  sendSample = true;
}