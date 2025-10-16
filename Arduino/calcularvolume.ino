#include <WiFi.h> // importa a lib WiFi

// Replace with your network credentials

const char *ssid = "projetolegal";  // nome da rede
const char *password = "123456789"; // senha da rede

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

void setup()
{
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
double medirdist()
{
    digitalWrite(trig, HIGH);
    delay(5);
    digitalWrite(trig, LOW);

    int tempo = pulseIn(echo, HIGH);
    double distancia = tempo * 0.034 / 2.0;
    return distancia;
}

// Função para calcular o volume do líquido no recipiente
double calcularvolume(double distancia)
{ // Não sei se esse calculo está correto
    double volume = areabase * (altura - distancia);
    return volume;
}

// Função para achar o que falta para encher o recipiente
double calcularfaltante(double distancia)
{
    double volume = areabase * (altura - distancia);
    double faltante = (areabase * altura) - volume;
    return faltante;
}

void loop()
{
    double distancia = medirdist();
    double volume = calcularvolume(distancia);
    double faltante = calcularfaltante(distancia);
    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    {                                  // If a new client connects,
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta http-equiv=" refresh " content=" 1 " /> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");

                        client.println("<style>html, body { background-color: #000000; margin: 0; padding: 0; font-family: " Poppins ", Cambria, Cochin, Georgia, Times, 'Times New Roman', serif; color: #f1f1f1;}");
                        client.println("h1 { font-size: 2.5rem; font-weight: 700; color: #3b82f6; margin-top: 40px; margin-bottom: 20px; text-align: center; }");
                        client.println(".descricao { background-color: #1a1a1a; max-width: 800px; margin: 0 auto 30px auto; padding: 20px; border-radius: 16px; line-height: 1.6; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.5); }");
                        client.println(".descricao p { margin: 10px 0; font-size: 1rem; text-align: left; }");
                        client.println(".medidas-container { display: flex; justify-content: center; flex-wrap: wrap; gap: 15px; max-width: 900px; margin: 0 auto 30px auto; }");
                        client.println(".medidas { background-color: #1a1a1a; flex: 1 1 250px; padding: 15px; border-radius: 12px; text-align: center; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.4); transition: transform 0.2s ease, box-shadow 0.2s ease; }");
                        client.println(".medidas h3 { margin: 0; font-size: 1.1rem; font-weight: 500; }");
                        client.println(".medidas span { font-weight: 700; font-size: 1.3rem; color: #f6f8fc; }");
                        client.println(".creditos { text-align: center; padding: 20px; font-size: 0.9rem; border-top: 1px solid #222; color: #bbb; }");

                        client.println("<body> <h1>Cálculo de volume da água</h1>");
                        client.println("<div class="descricao"><p>O projeto foi feito com o intuito de calcular o volume da água dentro de um recipiente geométrico.</p>");
                        client.println("<p>O cálculo é feito através de um sensor ultrassônico que mede a distância entre o sensor e a superfície da água.</p> </div>");

                        client.println("<div class="medidas-container"> <div class="medidas"> <h3>Distância = <span id="distancia">)"
                        client.println(distancia);
                        client.println("</span> cm</h3> </div>");
                        client.println("</h1>");
                        client.println("<div class="medidas"> <h3>Volume no recipiente = <span id="volume">");
                        client.println(volume);
                        client.println("</span> cm³</h3> </div>");
                        client.println("<div class="medidas"><h3>Volume para encher = <span id="falta">--</span> cm³</h3></div></div>");
                        client.println("<div class="creditos"> <p>Feito por: <strong>Allyson e Gustavo</strong></p>");
                        client.println("<p>Com o apoio e auxílio do professor Antonio</p></div></body></html>")
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
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