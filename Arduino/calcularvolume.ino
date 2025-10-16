#include <WiFi.h> // importa a lib WiFi

// Replace with your network credentials

const char* ssid     = "projetolegal"; // nome da rede
const char* password = "123456789"; // senha da rede

// Declaração de variáveis e pinos
int echo = 26;
int trig = 27;
int areabase = 106.53;
int altura = 4.5;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins


void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  // Set outputs to LOW
  

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}
// Funções do código

// Função para medir a distância
double medirdist(){
    digitalWrite(trig, HIGH);
    delay(5);
    digitalWrite(trig, LOW);
 
  
    int tempo = pulseIn(echo, HIGH);
    double distancia = tempo * 0.034 / 2.0;
    return distancia;
}

// Função para calcular o volume do líquido no recipiente
double calcularvolume(double distancia){ // Não sei se esse calculo está correto
    double volume = areabase * (altura - distancia);
    return volume;
}

void loop(){
  double distancia = medirdist();
  double volume = calcularvolume(distancia);
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
           
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #0; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
             

            client.println("<body><h1>distancia = ");
            client.println(distancia);
            client.println("</h1>");

            
           
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

    
  }
  Serial.println(distancia);
  delay(300);
}