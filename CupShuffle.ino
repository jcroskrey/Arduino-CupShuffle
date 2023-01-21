
const int buttonPin = A0;
int keyVal = 0;

const int buttonOne = 4;
const int buttonTwo = 5;
const int buttonThree = 6;
const int buttonDelta = 3;
const int successLight = 2;
const int failLight = 7;

void setup() {
  // put your setup code here, to run once:
  
  // Setup console to be written to
  Serial.begin(9600);
  
  // These are the blue lights
  pinMode(buttonOne, OUTPUT); 
  pinMode(buttonTwo, OUTPUT); 
  pinMode(buttonThree, OUTPUT); 

  // success and fail lights
  pinMode(successLight, OUTPUT);
  pinMode(failLight, OUTPUT);

  // the button input
  pinMode(buttonPin, INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  short level = 1;        // start at level 1
  bool correct = true;
  short scrambles;
  short speed;
  
  while (correct) {
    if (level == 1) {
      scrambles = 25;   // 25 iterations to start
      speed = 150;      // milliseconds
    }
    else {
      scrambles = (4 * scrambles) / 3;   // increase by 1/3
      speed = (2 * speed) / 3;           // decrease by 1/3
    }

    
    Serial.print("LEVEL: ");
    Serial.println(level);
    int final_light = scramble(scrambles, speed);
    int player_guess = get_answer();

    flash_answer(player_guess, final_light);
    
    correct = (player_guess == final_light);

    if (correct) {
      level++;
      delay(5000);
    }    
  }

  // calculate and print final scores
  short finalSpeed = speed + (speed / 2);
  short finalLevel = level - 1;
  Serial.print("Final score: ");
  Serial.println(finalLevel);
  if (finalLevel != 0) {
    Serial.print("Max shuffle speed: ");
    Serial.print(finalSpeed);
    Serial.println("ms");
    Serial.print("Lost on shuffle speed: ");
    Serial.print(speed);
    Serial.println("ms");
    Serial.println("");
  }
  

  game_reset_blink();

}

void game_reset_blink() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(buttonOne, HIGH);
    digitalWrite(buttonTwo, HIGH);
    digitalWrite(buttonThree, HIGH);
    digitalWrite(successLight, HIGH);
    digitalWrite(failLight, HIGH);
    delay(500);
    digitalWrite(buttonOne, LOW);
    digitalWrite(buttonTwo, LOW);
    digitalWrite(buttonThree, LOW);
    digitalWrite(successLight, LOW);
    digitalWrite(failLight, LOW);
    delay(500);
  }
}

int scramble(int scrambles, int interval) {
  // determine final light to be displayed
  short prevPin = random(1, 4);

  // scramble the lights
  while (scrambles > 0) {
    // flash the prevPin
    digitalWrite(prevPin + buttonDelta, HIGH);
    delay(interval);
    digitalWrite(prevPin + buttonDelta, LOW);

    // Generate list that contains the two options that are not the previous number
    int options[2];
    short optionsIndex = 0;
    for (int i = 0; i < 3; i++) {
      if (i + 1 != prevPin) {
        options[optionsIndex] = i + 1;
        optionsIndex++;
      }
    }
    // select a random index
    int randomIndex = random(0, 2);
    prevPin = options[randomIndex];

    // decrement scramble iterator
    scrambles--;
  }
  // flash the last light (light pins are 3 through 6 so add buttonDelta to prevPin)
  digitalWrite(prevPin + buttonDelta, HIGH);
  delay(interval);
  digitalWrite(prevPin + buttonDelta, LOW);

  return prevPin;
}

int get_answer() {
  // button 1 avg is 1000 (bottom of board)
  // button 2 avg is 850 (middle of board)
  // button 3 avg is 700 (top of board)
  // range is +- 75 for each avg
  
  // initialize answer to 0
  int answer = 0;

  while (answer == 0) {
    keyVal = analogRead(buttonPin);

    if (600 < keyVal && keyVal <= 775) {
      // buttonThree
      digitalWrite(buttonThree, HIGH);
      answer = 3;
    }
    else if (775 < keyVal && keyVal <= 925) {
      // buttonTwo
      digitalWrite(buttonTwo, HIGH);
      answer = 2;
    }
    else if (925 < keyVal && keyVal <= 1075) {
      // buttonOne
      digitalWrite(buttonOne, HIGH);
      answer = 1;
    }
    else {
      digitalWrite(buttonOne, LOW);
      digitalWrite(buttonTwo, LOW);
      digitalWrite(buttonThree, LOW);
    }
  }
  // answer selected.
  delay(1000);
  digitalWrite(answer + buttonDelta, LOW); // turn off answer light
  return answer;
}

void flash_answer(int guess, int final) {
  int light;
  if (guess == final) {
    light = successLight;
  }
  else {
    light = failLight;
  }
  
  // flash the light
  for (int i = 0; i < 5; i++){
    digitalWrite(light, HIGH);
    delay(250);
    digitalWrite(light, LOW);
    delay(250);
  }
}
