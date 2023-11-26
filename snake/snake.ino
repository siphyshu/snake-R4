#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

#include "continue_playing.h"
#include "yes_option.h"
#include "no_option.h"

ArduinoLEDMatrix matrix;

const int matrixSizeX = 12;
const int matrixSizeY = 8;

const int joystickXPin = A0;  // Analog pin for X-axis of the joystick
const int joystickYPin = A1;  // Analog pin for Y-axis of the joystick
const int joystickSwPin = D13;  // Digital pin for Switch of the joystick

int xValue, yValue, xMap, yMap, xPrev, yPrev = 0;
boolean swState, swPrev = 0;

int snakeSpeed = 170;
int maxSpeed = 70;
int currentSpeed = snakeSpeed;
int direction, directionPrev = 0;
int score = 0;
bool isGameOver = false;

byte grid[matrixSizeY][matrixSizeX] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

struct Point {
  int x;
  int y;
};

Point snake[100];  // Maximum length of the snake
Point food;
int snakeLength = 1;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Snake game on Arduino R4 LED Matrix with a Dual Axis Joystick");
  matrix.begin();
  
  const String snakeASCII = "    o=.__-SNAKE-R4-___--'    ";
  printText(snakeASCII, 35);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(joystickSwPin, INPUT_PULLUP);

  initializeGame();
}



void loop() {
  // Handle joystick input
  handleJoystick();

  // Move the snake
  moveSnake();

  // Check for collisions
  checkCollisions();

  // Update the LED matrix
  updateMatrix();

  // Delay based on the snake's speed
  delay(currentSpeed);
}



void printText(String displayText, int speed) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(speed);

  // const char text[] = "    -.__/ SNAKE-R4 \___,--=e    ";
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(displayText);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}



void initializeGame() {
  resetGrid();

  Point snake[100];
  snakeLength = 1;
  snakeSpeed = 170;
  currentSpeed = snakeSpeed;
  score = 0;
  direction, directionPrev = 0;
  isGameOver = false;

  // Initialize snake
  snake[0].x = 0;
  snake[0].y = 7;

  grid[snake[0].y][snake[0].x] = 1;
  matrix.renderBitmap(grid, matrixSizeY, matrixSizeX);
  // Initialize food
  generateFood();
}



void generateFood() {
  food.x = random(matrixSizeX);
  food.y = random(matrixSizeY);

  // Make sure the food does not overlap with the snake
  for (int i = 0; i < snakeLength; i++) {
    if (food.x == snake[i].x && food.y == snake[i].y) {
      generateFood();
      return;
    }
  }

  grid[food.y][food.x] = 1;
  matrix.renderBitmap(grid, matrixSizeY, matrixSizeX);
}



void handleJoystick() {
  xValue = analogRead(joystickXPin);
  yValue = analogRead(joystickYPin);
  swState = not digitalRead(joystickSwPin);

  xMap = map(xValue, 0, 1023, -512, 512);
  yMap = map(yValue, 0, 1023, 512, -512);

  // disallow moving in the opposite direction of current direction while the game is running
  if (xMap <= 512 && xMap >= 10 && yMap <= 511 && yMap >= -511) {
    if ((!isGameOver && directionPrev != 3) || isGameOver) { direction = 1; } // Right 
  } else if (xMap >= -512 && xMap <= -10 && yMap <= 511 && yMap >= -511) {
    if ((!isGameOver && directionPrev != 1) || isGameOver) { direction = 3; } // Left
  } else if (yMap <= 512 && yMap >= 10 && xMap <= 511 && xMap >= -511) {
    if ((!isGameOver && directionPrev != 4) || isGameOver) { direction = 2; } // Up
  } else if (yMap >= -512 && yMap <= -10 && xMap <= 511 && xMap >= -511) {
    if ((!isGameOver && directionPrev != 2) || isGameOver) { direction = 4; } // Down
  }

  if (!isGameOver) {
    directionPrev = direction;
  }

  Serial.println(String(direction) + " | " + String(directionPrev));
}



void moveSnake() {
  // Move the body of the snake
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Move the head of the snake
  switch (direction) {
    case 1:  // Right
      snake[0].x = (snake[0].x + 1) % matrixSizeX;
      break;
    case 2:  // Up
      snake[0].y = (snake[0].y - 1 + matrixSizeY) % matrixSizeY;
      break;
    case 3:  // Left
      snake[0].x = (snake[0].x - 1 + matrixSizeX) % matrixSizeX;
      break;
    case 4:  // Down
      snake[0].y = (snake[0].y + 1) % matrixSizeY;
      break;
  }
}



void updateMatrix() {
  resetGrid();

  for (int i = 0; i < snakeLength; i++) {
    grid[snake[i].y][snake[i].x] = 1;
  }
  grid[food.y][food.x] = 1;
  matrix.renderBitmap(grid, matrixSizeY, matrixSizeX);
}



void resetGrid() {
  for (int y = 0; y < matrixSizeY; y++) {
    for (int x = 0; x < matrixSizeX; x++) {
      grid[y][x] = 0;
    }
  }
}



void checkCollisions() {
  // Check for collision with self
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      // Game over, restart
      gameOver();
    }
  }

  // check if collides with 
  if (snake[0].x == food.x && snake[0].y == food.y) {
    snakeLength++;
    score++;

    generateFood();

    if (currentSpeed > maxSpeed) {
      currentSpeed -= 5;
    }
  }
}



void gameOver() {
  isGameOver = true;
  resetGrid();
  
  printText("    Game Over    ", 35);
  for (int i = 0; i < 4; i++) {
    displayScore();
  }

  continuePlaying();
  initializeGame();
}



void displayScore() { 
  matrix.beginDraw(); 
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(String(score));
  matrix.endText();  
  matrix.endDraw();

  delay(500);
  matrix.renderBitmap(grid, matrixSizeY, matrixSizeX);
  delay(500);
}



void continuePlaying() {
  matrix.loadSequence(continue_playing);
  matrix.renderFrame(2);
  String selectedOption = "yes";

  while (!swState) {
    handleJoystick();
    if (direction == 3) {
      matrix.renderFrame(0);
      selectedOption = "yes";
    } else if (direction == 1) {
      matrix.renderFrame(1);
      selectedOption = "no";
    }
    delay(100);
  }

  if (selectedOption == "no") {
    matrix.loadSequence(no_option);
    matrix.play();
    delay(1000);
    while (true) {
      displayScore();
    }
  } else if (selectedOption == "yes") {
    matrix.loadSequence(yes_option);
    matrix.play();
    delay(1000);
  }
}