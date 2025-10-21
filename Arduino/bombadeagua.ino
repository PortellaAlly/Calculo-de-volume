int botao = 2;
int bomba = 4;
int liga = 0;

void setup()
{
  pinMode(botao, INPUT);
  pinMode(bomba, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
 int pressionado = digitalRead(botao);
 
 
 
  if((pressionado == HIGH)&& (liga == 0)){
  digitalWrite(bomba, HIGH);
    liga = 1;
    Serial.println("Ligado");
    delay(500);
  }
  else if((pressionado == HIGH)&& (liga == 1)){
  digitalWrite(bomba, LOW);
    liga = 0;
    Serial.println("Desligado");
    delay(500);
  }
}