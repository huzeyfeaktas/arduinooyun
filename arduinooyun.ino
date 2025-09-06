#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int btnUp = 2;
const int btnDown = 3;
const int btnSelect = 4;
const int btnBack = 5;

int selectedGame = 0;
String games[] = {"Pong", "Snake", "Target Practice", "Flappy"}; // Reaction Time yerine Target Practice
const int totalGames = 4;

bool inMenu = true;
unsigned long gameStartTime;

void setup() {
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);
  pinMode(btnBack, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0)); // Rastgele sayı üreteci için seed
}

void loop() {
  if (inMenu) {
    showMenu();
    handleMenuInput();
  } else {
    gameStartTime = millis(); // Oyun başlangıç zamanı
    switch (selectedGame) {
      case 0: pongGame(); break;
      case 1: snakeGame(); break;
      case 2: targetPractice(); break; // Yeni oyun
      case 3: flappyGame(); break;
    }
    if (digitalRead(btnBack) == LOW) {
      inMenu = true;
      delay(200);
    }
  }
}


void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  for (int i = 0; i < totalGames; i++) {
    if (i == selectedGame) {
      display.setCursor(0, i * 10);
      display.print("> ");
    } else {
      display.setCursor(10, i * 10);
    }
    display.println(games[i]);
  }

  display.display();
}

void handleMenuInput() {
  if (digitalRead(btnUp) == LOW) {
    selectedGame = (selectedGame - 1 + totalGames) % totalGames;
    delay(200);
  }
  if (digitalRead(btnDown) == LOW) {
    selectedGame = (selectedGame + 1) % totalGames;
    delay(200);
  }
  if (digitalRead(btnSelect) == LOW) {
    inMenu = false;
    delay(200);
  }
}


void pongGame() {
  int ballX = SCREEN_WIDTH / 2, ballY = SCREEN_HEIGHT / 2;
  int ballSpeedX = 1, ballSpeedY = 1;
  int paddle1Y = 20, paddle2Y = 20;
  const int paddleHeight = 20, paddleWidth = 2;
  int backButtonCount = 0;
  unsigned long previousMillis = 0;

  while (true) {
    unsigned long currentMillis = millis();
    float speedMultiplier = 1.0 + (currentMillis - gameStartTime) / 10000.0; // Hız artışı

    if (currentMillis - previousMillis > 10 / speedMultiplier) {
      previousMillis = currentMillis;

      if (digitalRead(btnUp) == LOW && paddle1Y > 0) paddle1Y -= 2;
      if (digitalRead(btnDown) == LOW && paddle1Y < SCREEN_HEIGHT - paddleHeight) paddle1Y += 2;
      if (digitalRead(btnSelect) == LOW && paddle2Y > 0) paddle2Y -= 2;

      if (digitalRead(btnBack) == LOW && paddle2Y < SCREEN_HEIGHT - paddleHeight) {
        paddle2Y += 2;
        backButtonCount++;
        if (backButtonCount >= 3) {
           inMenu = true;
           delay(200); // Debounce
           break;
         }
       } else {
         backButtonCount = 0; // Geri butonu bırakıldığında sayacı sıfırla.
       }


      ballX += ballSpeedX * speedMultiplier;
      ballY += ballSpeedY * speedMultiplier;

      if (ballY <= 0 || ballY >= SCREEN_HEIGHT) ballSpeedY = -ballSpeedY;

      // Topun raketlere çarpmasını kontrol et
      if ((ballX <= paddleWidth && ballY >= paddle1Y && ballY <= paddle1Y + paddleHeight) ||
          (ballX >= SCREEN_WIDTH - paddleWidth - 2 && ballY >= paddle2Y && ballY <= paddle2Y + paddleHeight)) {
        ballSpeedX = -ballSpeedX;
      }

      display.clearDisplay();
      display.fillRect(0, paddle1Y, paddleWidth, paddleHeight, SSD1306_WHITE);
      display.fillRect(SCREEN_WIDTH - paddleWidth -1, paddle2Y, paddleWidth, paddleHeight, SSD1306_WHITE);
      display.fillRect(ballX, ballY, 2, 2, SSD1306_WHITE);
      display.display();

    }
     if (digitalRead(btnBack) == LOW)
     delay(100);
  }
}



void snakeGame() {
  int x = 10, y = 10;
  int dx = 1, dy = 0;
  int snakeLength = 1;
  int foodX = random(0, SCREEN_WIDTH / 2) * 2, foodY = random(0, SCREEN_HEIGHT / 2) * 2; // Yiyecek konumunu piksel boyutuna göre ayarla
  int snake[100][2];
  unsigned long previousMillis = 0;
  int direction = 0; // 0: sağ, 1: aşağı, 2: sol, 3: yukarı



  while (true) {
    unsigned long currentMillis = millis();
    float speedMultiplier = 1.0 + (currentMillis - gameStartTime) / 5000.0; // Hız artışı
    if (currentMillis - previousMillis > 100 / speedMultiplier) {


       previousMillis = currentMillis;

      if (digitalRead(btnUp) == LOW ) {
        direction = (direction -1 + 4) % 4;
        delay(100);
      }

      if (digitalRead(btnDown) == LOW) {
         direction = (direction + 1) % 4;
         delay(100);
      }


      // Yönü güncelle
       if (direction == 0) { dx = 2; dy = 0; } // Sağ
       else if (direction == 1) { dx = 0; dy = 2; } // Aşağı
       else if (direction == 2) { dx = -2; dy = 0; } // Sol
       else if (direction == 3) { dx = 0; dy = -2; } // Yukarı



      x += dx;
      y += dy;


      // Ekran sınırlarını kontrol et
      if (x < 0) x = SCREEN_WIDTH - 2;
      if (x >= SCREEN_WIDTH) x = 0;
      if (y < 0) y = SCREEN_HEIGHT - 2;
      if (y >= SCREEN_HEIGHT) y = 0;



      if (x == foodX && y == foodY) {
        snakeLength++;
        foodX = random(0, SCREEN_WIDTH / 2) * 2;
        foodY = random(0, SCREEN_HEIGHT / 2) * 2;

      }



      for (int i = snakeLength - 2; i >= 0; i--) {
        snake[i + 1][0] = snake[i][0];
        snake[i + 1][1] = snake[i][1];
      }

      snake[0][0] = x;
      snake[0][1] = y;


      for (int i = 1; i < snakeLength; i++) {
        if (snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]) {
          // Oyunu bitir
          return;
        }
      }

      display.clearDisplay();


      for (int i = 0; i < snakeLength; i++) {
        display.fillRect(snake[i][0], snake[i][1], 2, 2, SSD1306_WHITE);
      }
      display.fillRect(foodX, foodY, 2, 2, SSD1306_WHITE);
      display.display();
    }
    if (digitalRead(btnBack) == LOW) {
      delay(100);

    }
  }
}


void targetPractice() {
  int targetX = random(10, SCREEN_WIDTH - 10);
  int targetY = random(10, SCREEN_HEIGHT - 10);
  int score = 0;
  unsigned long targetStartTime = millis();
  const int targetSize = 8;

  while (true) {

    float speedMultiplier = 1.0 + (millis() - gameStartTime) / 10000.0;
    display.clearDisplay();
    display.fillRect(targetX, targetY, targetSize, targetSize, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Score: ");
    display.println(score);
    display.display();



    if (digitalRead(btnSelect) == LOW) {
      // Hedefi vurup vurmadığını kontrol et
      int crosshairX = SCREEN_WIDTH/2 -1;
      int crosshairY = SCREEN_HEIGHT/2 -1;
      if( crosshairX >= targetX && crosshairX <= targetX + targetSize && crosshairY >= targetY && crosshairY <= targetY + targetSize){
        score++;
        targetX = random(10, SCREEN_WIDTH - 10);
        targetY = random(10, SCREEN_HEIGHT - 10);
        delay(100);
      }
    }
    if (digitalRead(btnBack) == LOW) {
       inMenu = true;
       delay(200);
       break;
    }

  }

}


void flappyGame() {
  int birdX = 30, birdY = SCREEN_HEIGHT / 2;
  int birdSpeed = 0;
  const int gravity = 1;
  const int lift = -5;
  const int pipeWidth = 10;
  int pipeX = SCREEN_WIDTH;
  int pipeHeight = random(20, SCREEN_HEIGHT - 30); // Boru yüksekliğini ayarla
  int pipeGap = 25; // Boru aralığını ayarla
  unsigned long previousMillis = 0;


  while (true) {
    unsigned long currentMillis = millis();
    float speedMultiplier = 1.0 + (currentMillis - gameStartTime) / 5000.0;


    if (currentMillis - previousMillis > 100 / speedMultiplier) {
      previousMillis = currentMillis;
      birdSpeed += gravity;
      birdY += birdSpeed;


      if (birdY < 0 || birdY > SCREEN_HEIGHT -3) return;


      display.clearDisplay();
      display.fillRect(birdX, birdY, 3, 3, SSD1306_WHITE);



      pipeX -= 2 * speedMultiplier;
      if (pipeX < -pipeWidth) {
        pipeX = SCREEN_WIDTH;
        pipeHeight = random(20, SCREEN_HEIGHT - 30);
      }
      display.fillRect(pipeX, 0, pipeWidth, pipeHeight, SSD1306_WHITE);
      display.fillRect(pipeX, pipeHeight + pipeGap, pipeWidth, SCREEN_HEIGHT - pipeHeight - pipeGap, SSD1306_WHITE);
      display.display();


      if (digitalRead(btnUp) == LOW) {
        birdSpeed = lift;
        delay(50);
      }

        // Çarpışma kontrolü

       if (birdX + 3 > pipeX && birdX < pipeX + pipeWidth && (birdY < pipeHeight || birdY + 3 > pipeHeight + pipeGap)) {
             return;  // Oyunu bitir
          }
    }

    if (digitalRead(btnBack) == LOW) {
      inMenu = true;

      break;
    }

  }
}