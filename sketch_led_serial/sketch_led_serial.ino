int buttonPin = 13;
// setup initializes serial and the button pin
void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, OUTPUT);
}
// loop checks the button pin each time,
// and will send serial if it is pressed
void loop()
{
  digitalWrite(buttonPin, HIGH);
  Serial.println('H');
  delay(5000);
  digitalWrite(buttonPin, LOW);
  Serial.println('L');
  delay(5000);
}
