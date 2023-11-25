#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

const int matrixSizeX = 12;
const int matrixSizeY = 8;

const int joystickXPin = A0;  // Analog pin for X-axis of the joystick
const int joystickYPin = A1;  // Analog pin for Y-axis of the joystick
// const int jowstickSwPin = D2;  // Digital pin for Switch of the joystick

int snakeSpeed = 170;
int currentSpeed = snakeSpeed;
int direction = 0;
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



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  Serial.println("Snake game on Arduino R4 LED Matrix with a Dual Axis Joystick");
  matrix.begin();
  
  const String snakeASCII = "    o=.__-SNAKE-R4-___--'    ";
  printText(snakeASCII, 35);
  pinMode(LED_BUILTIN, OUTPUT);

  initializeGame();
}



void loop() {
  if (!isGameOver) {
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
  } else {
    displayScore();
    delay(500);
    resetGrid();
    delay(500);
  }
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


struct Point {
  int x;
  int y;
};

Point snake[100];  // Maximum length of the snake
Point food;
int snakeLength = 1;


void initializeGame() {
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
  int xValue = analogRead(joystickXPin);
  int yValue = analogRead(joystickYPin);

  if (xValue == 1023) {
    direction = 1; // Right
  } else if (yValue == 0) {
    direction = 2; // Up
  } else if (xValue == 0) {
    direction = 3; // Left
  } else if (yValue == 1023) {
    direction = 4; // Down
  }

  Serial.println(direction);
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

  // matrix.update();
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

    currentSpeed -= 5;
  }
}


void gameOver() {
  resetGrid();
  Point snake[100];
  snakeLength = 1;
  printText("    Game Over    ", 50);
  isGameOver = true;
}


void displayScore() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(String(score)); // Convert score to string
  matrix.endText();

  matrix.endDraw();
}