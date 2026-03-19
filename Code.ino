/*
8x8x8 LED Cube (512 LEDs)
Controller: Arduino Uno
Driver: 74HC595 Shift Registers
Control Method: SPI Multiplexing

Features:
- Multiple 3D animations
- Rain simulation
- Moving planes
- Expanding cube effects
- Text display
*/

#include <SPI.h>   // Library for SPI communication (used to control shift registers)

// -------- Axis Definitions --------
#define X_axis 0
#define Y_axis 1
#define Z_axis 2

// -------- Shift Directions --------
#define POSITION_X 0
#define POSITION_Z 2
#define POSITION_Y 4

#define NEG_X 1
#define NEG_Z 3
#define NEG_Y 5

// Push button pin to switch between animation effects
#define BTN A5

// Total number of animation effects available
#define TOTAL 8

// Animation effect identifiers
#define RAIN 0
#define PLANE_BOING 1
#define SEND_VOXELS 2
#define WOOP_WOOP 3
#define CUBE_JUMP 4
#define GLOW 5
#define TEXT 6
#define LIT 7

// Timing parameters for each animation
#define RAIN_TIME 200
#define PLANE_BOING_TIME 300
#define SEND_VOXELS_TIME 140
#define WOOP_WOOP_TIME 350
#define CUBE_JUMP_TIME 200
#define GLOW_TIME 8
#define TEXT_TIME 300
#define CLOCK_TIME 500


// -------- Character Data for Numbers --------
// Used for displaying numbers in the cube
uint8_t characters[10][8] = {
  {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C}, //0
  {0x10, 0x18, 0x14, 0x10, 0x10, 0x10, 0x10, 0x3C}, //1
  {0x3C, 0x42, 0x40, 0x40, 0x3C, 0x02, 0x02, 0x7E}, //2
  {0x3C, 0x40, 0x40, 0x3C, 0x40, 0x40, 0x42, 0x3C}, //3
  {0x22, 0x22, 0x22, 0x22, 0x7E, 0x20, 0x20, 0x20}, //4
  {0x7E, 0x02, 0x02, 0x3E, 0x40, 0x40, 0x42, 0x3C}, //5
  {0x3C, 0x02, 0x02, 0x3E, 0x42, 0x42, 0x42, 0x3C}, //6
  {0x3C, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40}, //7
  {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C}, //8
  {0x3C, 0x42, 0x42, 0x42, 0x3C, 0x40, 0x40, 0x3C}, //9
};


// -------- Data used for drill animation patterns --------
const uint8_t DrillData[14][8] PROGMEM =
{
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01},
  {0x40, 0x20, 0x20, 0x10, 0x08, 0x04, 0x04, 0x02},
  {0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04},
  {0x10, 0x10, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08},
  {0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10},
  {0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20},
  {0x02, 0x04, 0x04, 0x08, 0x10, 0x20, 0x20, 0x40},
  {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80},
  {0x00, 0x01, 0x06, 0x08, 0x10, 0x60, 0x80, 0x00},
  {0x00, 0x00, 0x03, 0x0C, 0x30, 0xC0, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0x00},
  {0x00, 0x80, 0x60, 0x10, 0x08, 0x06, 0x01, 0x00}
};


// -------- Cube Data Storage --------
// cube[y][z] contains 8 bits representing LEDs along X axis
uint8_t CubeData[8][8];    

// Current animation effect index
uint8_t CurrentEffect;

// LED blinking counter
uint8_t Blinker;

// Step counter for cube scanning
uint8_t CurrentStep;

// Random number seed
uint64_t RandomSeed;


// Main cube state array
uint8_t cube[8][8];

// Currently running animation effect
uint8_t currentEffect;

// Timing counter used for animation speed
uint16_t timer;

// Random timer used for seed generation
uint64_t randomTimer;

// Indicates if animation just started
bool loading;



// --------------------------------------------------
// SETUP FUNCTION
// --------------------------------------------------
void setup() {

  loading = true;        // First animation frame initialization
  randomTimer = 0;
  currentEffect = RAIN;  // Start with rain animation

  SPI.begin();           // Initialize SPI communication
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));  // Configure SPI speed and mode

  pinMode(BTN, INPUT_PULLUP); // Button for switching animations

  randomSeed(analogRead(0));  // Initialize random seed using analog noise
}



// --------------------------------------------------
// MAIN LOOP
// --------------------------------------------------
void loop() {

  randomTimer++;   // Update random timer

  // If button pressed -> change animation
  if (digitalRead(BTN) == LOW) {

    clearCube();       // Clear all LEDs
    loading = true;
    timer = 0;

    currentEffect++;   // Move to next animation
    if (currentEffect == TOTAL) {
      currentEffect = 0;
    }

    randomSeed(randomTimer);  // Update random seed
    randomTimer = 0;

    delay(500); // debounce delay
  }

  // Select which animation to run
  switch (currentEffect) {
    case RAIN: rain(); break;
    case PLANE_BOING: planeBoing(); break;
    case SEND_VOXELS: sendVoxels(); break;
    case WOOP_WOOP: woopWoop(); break;
    case CUBE_JUMP: cubeJump(); break;
    case GLOW: glow(); break;
    case TEXT: text("0123456789", 10); break;
    case LIT: lit(); break;
    default: rain();
  }

  renderCube();  // Update LED cube hardware
}



// --------------------------------------------------
// RENDER CUBE
// Sends cube data to shift registers using SPI
// --------------------------------------------------
void renderCube() {

  // Scan each layer
  for (uint8_t i = 0; i < 8; i++) {

    digitalWrite(SS, LOW);   // Activate shift register

    SPI.transfer(0x01 << i); // Enable current layer

    // Send column data
    for (uint8_t j = 0; j < 8; j++) {
      SPI.transfer(cube[i][j]);
    }

    digitalWrite(SS, HIGH);  // Latch data
  }
}



// --------------------------------------------------
// RAIN ANIMATION
// Simulates rain droplets falling from top
// --------------------------------------------------
void rain() {

  if (loading) {
    clearCube();
    loading = false;
  }

  timer++;

  if (timer > RAIN_TIME) {

    timer = 0;

    shift(NEG_Y);   // Move all LEDs downward

    uint8_t numDrops = random(0, 5);

    for (uint8_t i = 0; i < numDrops; i++) {
      setVoxel(random(0, 8), 7, random(0, 8)); // Add new droplets at top
    }
  }
}



// --------------------------------------------------
// VOXEL FUNCTIONS
// --------------------------------------------------

// Turn ON a voxel (LED) at coordinate (x,y,z)
void setVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] |= (0x01 << x);
}


// Turn OFF voxel
void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] ^= (0x01 << x);
}


// Check if voxel is ON
bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
  return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}



// --------------------------------------------------
// LIGHT ENTIRE CUBE
// Turns all LEDs ON
// --------------------------------------------------
void lightCube() {

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0xFF;
    }
  }
}



// --------------------------------------------------
// CLEAR ENTIRE CUBE
// Turns all LEDs OFF
// --------------------------------------------------
void clearCube() {

  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0;
    }
  }
}
