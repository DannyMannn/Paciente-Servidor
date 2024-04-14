#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <WiFi.h>
#include <WebServer.h>


struct Paciente
{
    char nombre[100];
    char cuarto[10];
    char idPaciente[10];
};

#define MAX_PACIENTES 100

struct Paciente pacientes[MAX_PACIENTES];
int numPacientes = 0;

const char* ssid = "ESP32";  
const char* password = "12345678"; 


/* Asignación de IP para el ESP32*/
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);


WebServer server(80);


void setup() {
  Serial.begin(115200);
 


  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // cargar pacientes

  loadPacientes("pacientes.bin");
  /*
  struct Paciente *paciente = login("123456");

  if (paciente != NULL) {
      printf("Nombre: %s\n", paciente->nombre);
      printf("Cuarto: %s\n", paciente->cuarto);
  } else {
      printf("Paciente no encontrado.\n");
  }
  */
 
  server.on("/", handle_OnConnect);
  server.on("/login", handle_login);
  server.on("/admin", handle_admin);
  server.on("/camara", handle_camera);
  server.on("/paciente", handle_paciente);


  server.onNotFound(handle_NotFound);
 
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
}


void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}


void handle_login() {
  String enteredPacienteId;
  if (server.method() == HTTP_POST) {
    // Process the login form submission
    enteredPacienteId = server.arg("pacienteId");


    struct Paciente* loggedInPaciente = login(enteredPacienteId.c_str());

    if (loggedInPaciente != NULL) {
      // Redirect to /paciente/<pacienteId>
      String redirectUrl = "/paciente/" + enteredPacienteId;
      server.sendHeader("Location", redirectUrl);
      server.send(302, "text/plain", ""); // 302 status code for redirect
    } else {
      // Patient doesn't exist, stay on the login page
      server.send(200, "text/html", "Patient doesn't exist.");
    }
  } else {
    // Show the login form
    String html = "<html><body>";
    html += "<form method='POST' action='/login'>";
    html += "Paciente ID: <input type='text' name='pacienteId'><br>";
    html += "<input type='submit' value='Login'>";
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  }
}


void handle_admin(){

}

void handle_camara(){

}

void handle_paciente() {
  String pacienteId = server.uri().substring(9); // Extract the pacienteId from the URI (/paciente/<pacienteId>)
  
  struct Paciente* paciente = login(pacienteId.c_str()); // Retrieve the paciente based on the pacienteId
  
  if (paciente != NULL) {
    // Generate HTML page with paciente information
    String html = "<html><body>";
    html += "<h1>Paciente Information</h1>";
    html += "<p>Nombre: " + String(paciente->nombre) + "</p>";
    html += "<p>Cuarto: " + String(paciente->cuarto) + "</p>";
    html += "<p>Paciente ID: " + String(paciente->idPaciente) + "</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
  } else {
    server.send(404, "text/html", "Paciente doesn't exist.");
  }
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}


String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="</html>\n";
  return ptr;
}


void loadPacientes(const char* filename) {
    // Open the file in binary mode for reading
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    // Read the structs from the file
    struct Paciente paciente;
    while (numPacientes < MAX_PACIENTES &&
           fread(&paciente, sizeof(struct Paciente), 1, file) == 1) {
        // Save the loaded struct into the global array
        pacientes[numPacientes++] = paciente;
    }

    // Close the file
    fclose(file);
}

void addPaciente(char nombre[100], char cuarto[100], char idPaciente[100]) {
    if (numPacientes < MAX_PACIENTES) {
        // Create a new struct object
        struct Paciente nuevoPaciente;

        // Copy the provided data into the new struct object
        snprintf(nuevoPaciente.nombre, sizeof(nuevoPaciente.nombre), "%s", nombre);
        snprintf(nuevoPaciente.cuarto, sizeof(nuevoPaciente.cuarto), "%s", cuarto);
        snprintf(nuevoPaciente.idPaciente, sizeof(nuevoPaciente.idPaciente), "%s", idPaciente);

        // Add the new struct object to the array
        pacientes[numPacientes++] = nuevoPaciente;

        printf("Paciente agregado exitosamente.\n");
    } else {
        printf("Ya no hay cupo para más pacientes.\n");
    }
}

void writePacientes(const char* filename) {
    // Open the file in binary mode for writing
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    // Write the structs to the file
    fwrite(pacientes, sizeof(struct Paciente), numPacientes, file);

    // Close the file
    fclose(file);

    printf("Pacientes written to file successfully.\n");
}


struct Paciente* login(char idPaciente[100]);

struct Paciente* login(char idPaciente[100]) {
    if(numPacientes == 0) return NULL;

    for (int i = 0; i < numPacientes; i++) {
        if (strcmp(idPaciente, pacientes[i].idPaciente) == 0) {
            return &pacientes[i];
        }
    }
    return NULL;
}