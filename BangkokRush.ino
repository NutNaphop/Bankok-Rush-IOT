#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define sw1 D5  // Change Switch Here , first switch
#define sw2 D6 // Change Switch here , second switch 
#define SIZE 5 // Size For obstacles 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Structure using for gen
struct Obstacle {
  int x;
  int y;
  int style;
};
// Game variables
Obstacle obstacles[SIZE];
int playerX = 0;  // player Position
int score = 0;
int highscore = 0;
int isPress = 0;
int distance = 1;
String txtDis = "0";
String txtScore = "0";
bool isJump = false;
bool gameOverFlag = false; // For Other Display ,
bool menuFlag = true; // Same as top 
bool winFlag = false ;  // Same as top 
bool isReset = false ;  // Using Together 

// Custom Text
byte thaiFont[][8] = {
  { B00000,
    B01100,
    B10010,
    B00010,
    B01010,
    B00110,
    B00100,
    B00100 },

  { B00000,
    B00000,
    B00000,
    B11010,
    B01010,
    B01010,
    B01010,
    B01110 },
  { B00000,
    B00000,
    B00000,
    B00100,
    B00100,
    B00100,
    B00100,
    B00110 },

  { B00000,
    B00000,
    B00100,
    B01010,
    B11010,
    B01010,
    B01010,
    B01010 },

  { B00000,
    B00000,
    B00000,
    B11001,
    B01001,
    B01001,
    B01101,
    B11011 }
};
// Animation
byte playerCar[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11110,
  B11011,
  B11111,
  B10001
};

byte obstaclesCar1[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11110,
  B10101,
  B11111,
  B10001
};

byte obstaclesCar2[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B10101,
  B11111,
  B10001
};

// Function
void isCollision(int index); // Check Collision
void gameOver(); // Game Over
void UI(); // UI
void jump(int isPress); // Jump checking 
void menu(); // Main Menu
void win() ;  // Win if distance = 100

// Start
void setup() {

  Serial.begin(9600);
  lcd.init() ; 
  lcd.backlight();
  // Set Pinmode for switch button 
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);  

  // Set Sprite
  lcd.createChar(0, playerCar);
  lcd.createChar(1, obstaclesCar1);
  lcd.createChar(2, obstaclesCar2);

  // LOAD THAI FONT
  for (int i = 0; i < 5; i++) {
    lcd.createChar(i + 3, thaiFont[i]);
  }
  //

  // Generate For Once 
  for (int i = 0; i < SIZE; i++) {
    obstacles[i].x = 16 + (i * 5); // Start Pos 
    obstacles[i].y = random(2); // Up or down
    obstacles[i].style = random(1, 3); // Set Sprite Style 
  }
}

void loop() {
  Serial.print(digitalRead(sw2)) ; 

  // // Gen OBC
  // If gameOver
  if (gameOverFlag) {
    gameOver() ;
  } 
  // If in Menu
  else if (menuFlag) {
    menu();
  
  } 
  // If Win
  else if (winFlag){
    win() ; 
  }
  // Game Lunch
  else {
    lcd.clear();
    distance++;
    if (score >= 99) isReset = true , winFlag = true ; 
    for (int i = 0; i < SIZE; i++) {   
      obstacles[i].x--;
      if (obstacles[i].x <= -1) { // If obstacles pos is -1 
        obstacles[i].x = 16;
        obstacles[i].y = random(2);
        obstacles[i].style = random(1, 3);
      }

      //DISPLAY AND CHECK COLSION
      lcd.setCursor(obstacles[i].x, obstacles[i].y);
      lcd.write(obstacles[i].style);
      isCollision(i);  // Check Collision
    }
    // GAME LOGIC
    jump(isPress);               // Check Jump
    UI();                        // UI display
    isPress = digitalRead(sw1);  // Event Listener Button
    delay(150);                  // Smooth gameplay
  }
}


// /* =============================================================================================== */
// /* Function */
void isCollision(int index) {
  if (obstacles[index].x == playerX) {
    if (isJump == true && obstacles[index].y == 0 || isJump == false && obstacles[index].y == 1) {
      gameOverFlag = true ;
      isReset = true ; 
      lcd.clear() ; 
    }
  }
}

void gameOver() {
  // Thai Font Display
  if (isReset) lcd.clear() ,isReset = false ; 
  for (int i = 3; i < 8; i++) {
    lcd.setCursor(i - 3, 0);
    lcd.write(i);
  }
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  gameOverFlag = true;
  menuFlag = false;
  if (digitalRead(sw2)) {
    lcd.clear() ; 
    gameOverFlag = false ;
    menuFlag = true ;
    setup() ; 
  } 
}

void UI() {
  if (distance % 10 == 0) score += 1;
  if (distance > 99) distance = 1;
  // Display score
  (distance < 10) ? txtDis = "0" + (String)distance : txtDis = distance;
  (score < 10) ? txtScore = "0" + (String)score : txtScore = score;
  lcd.setCursor(14, 0);
  lcd.print(txtScore);
  lcd.setCursor(14, 1);
  lcd.print(txtDis);
}

void jump(int isPress) {
  if (isPress == 1) {
    isJump = true;
    lcd.setCursor(0, 0);
    lcd.write(0);
  } else {
    // Display game elements
    isJump = false;
    lcd.setCursor(0, 1);
    lcd.write(0);  // Display player character
  }
}

void menu() {
  playerX = 0;  // player Position
  highscore = (score > highscore) ? score : highscore;
  score = 0;
  isPress = 0;
  distance = 1;
  txtDis = "0";
  txtScore = "0";
  isJump = false;
  // DISPLAY
  lcd.setCursor(0, 0);
  lcd.print("Bangkok Rush");
  lcd.setCursor(0, 1);
  lcd.print("Hi-Score : ");
  lcd.setCursor(10, 1);
  lcd.print(highscore);
  menuFlag = true;
  gameOverFlag = false;

  if (digitalRead(sw2)) menuFlag = false ; 

}

void win() {
  if (isReset) lcd.clear() ,isReset = false ; 
  menuFlag = false;
  gameOverFlag = false;
  winFlag = true ; 
  lcd.setCursor(0, 0) ; 
  lcd.print("WIN") ; 
  if (digitalRead(sw2)) menuFlag = true , gameOverFlag = false , winFlag = false ,setup() ; 

}
