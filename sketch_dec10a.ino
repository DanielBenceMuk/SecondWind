const int button2Pin = 2;
const int button1Pin = 3;
const int button3Pin = 7;
const int timeToFull = 10;
const int motorspeed = 105; //105
int timeToPullFull = 1000; //in here we store the setup phase
int releaseTimeDifference = 0;

void setup() {
  pinMode(4, OUTPUT); //motor1
  pinMode(5, OUTPUT); //motor1
  pinMode(6, OUTPUT); //motor1
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.write("Start\n");

  //digitalWrite(4, 255);
  //digitalWrite(5, 255);
  //attachInterrupt(digitalPinToInterrupt(2), triggerRelease, RISING);
}

bool tightened = false;

//in seconds
void triggerTight(int time) {
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  int mstime = time * 1000;
  int mstime2 = timeToPullFull;
  int n = mstime2 / 100;
  int per_increment = mstime / n; //delay per increment, n increments
  Serial.println(n);
  Serial.write("Start tighten\n");
   for(int i = 0; i < n; ++i) {
    Serial.write("Phase ");
    Serial.write(n);
    Serial.write("\n");
    analogWrite(6, motorspeed);
    delay(100);
    analogWrite(6, 0);
    delay(per_increment);
  }
  Serial.write("Tighten complete\n");
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  analogWrite(6, 0);
  tightened = true;
}

void triggerRelease() {
  Serial.write("Start release\n");
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  analogWrite(6, motorspeed);
  delay(timeToPullFull / 10 * 10 - releaseTimeDifference);
  analogWrite(6, 0);
  Serial.write("Release complete\n");
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  tightened = false;
}

int oldState[5] = {HIGH, HIGH, HIGH};

int setupPressed = 0;
int setupReleased = 0;
int releasePressed = 0;
int releaseReleased = 0;

void loop() {
  int buttonState[5] = {digitalRead(button1Pin), digitalRead(button2Pin), digitalRead(button3Pin)};
  if(oldState[1] != buttonState[1]) {
    if(buttonState[1] == HIGH) Serial.write("High\n");
    else if(buttonState[1] == LOW) Serial.write("Low\n");
    oldState[1] = buttonState[1];
    if(buttonState[1] == LOW) {
      if(tightened) {
        triggerRelease();
      } else {
        triggerTight(timeToFull);
      }
    }
  }

  if(oldState[0] != buttonState[0]) {
    oldState[0] = buttonState[0];
    if(buttonState[0] == LOW && !tightened) {
      Serial.println("pressed");
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      analogWrite(6, motorspeed);
      setupPressed = millis();
    }
    else if(buttonState[0] == HIGH && !tightened) {
      Serial.println("releaed");
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      analogWrite(6, 0);
      setupReleased = millis();

      timeToPullFull = (setupReleased - setupPressed);

      Serial.println(timeToPullFull);

      tightened = true;
    }
  }

  if(oldState[2] != buttonState[2]) {
    oldState[2] = buttonState[2];
    if(buttonState[2] == LOW && tightened) {
      Serial.println("pressed 2");
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      analogWrite(6, motorspeed);
      releasePressed = millis();
    }
    else if(buttonState[2] == HIGH && tightened) {
      Serial.println("released 2");
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      analogWrite(6, 0);
      releaseReleased = millis();

      releaseTimeDifference = releaseReleased - releasePressed;
    }
  }
}
