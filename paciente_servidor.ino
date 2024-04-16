#include <stdio.h>
#include <stdlib.h>
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

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

// Prototipo de la función login
struct Paciente* login(const char* idPaciente) {
    if (numPacientes == 0) return NULL;

    for (int i = 0; i < numPacientes; i++) {
        if (strcmp(idPaciente, pacientes[i].idPaciente) == 0) {
            return &pacientes[i];
        }
    }
    return NULL;
}


void setup() {
    Serial.begin(115200);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    loadPacientes("pacientes.bin");

    server.on("/", handle_OnConnect);
    server.on("/login", handle_login);
    server.on("/admin", handle_admin);
    // server.on("/camara", handle_camera);  // Comentado o eliminado
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
        enteredPacienteId = server.arg("pacienteId");
        struct Paciente* loggedInPaciente = login(enteredPacienteId.c_str());

        if (loggedInPaciente != NULL) {
            String redirectUrl = "/paciente/" + enteredPacienteId;
            server.sendHeader("Location", redirectUrl);
            server.send(302, "text/plain", "");
        } else {
            server.send(200, "text/html", "Patient doesn't exist.");
        }
    } else {
        String html = "<html><body>";
        html += "<form method='POST' action='/login'>";
        html += "Paciente ID: <input type='text' name='pacienteId'><br>";
        html += "<input type='submit' value='Login'>";
        html += "</form>";
        html += "</body></html>";
        server.send(200, "text/html", html);
    }
}

void handle_admin() {
    // Implementación de la función admin
}

void handle_camara() {
    // Implementación de la función camara
}

void handle_paciente() {
    String pacienteId = server.uri().substring(9);
    struct Paciente* paciente = login(pacienteId.c_str());
  
    if (paciente != NULL) {
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

void handle_NotFound() {
    server.send(404, "text/plain", "Not found");
}

String SendHTML() {
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="</html>\n";
    return ptr;
}

void loadPacientes(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    struct Paciente paciente;
    while (numPacientes < MAX_PACIENTES && fread(&paciente, sizeof(struct Paciente), 1, file) == 1) {
        pacientes[numPacientes++] = paciente;
    }

    fclose(file);
}

struct Paciente* login(char idPaciente[100]) {
    if (numPacientes == 0) return NULL;

    for (int i = 0; i < numPacientes; i++) {
        if (strcmp(idPaciente, pacientes[i].idPaciente) == 0) {
            return &pacientes[i];
        }
    }
    return NULL;
}
