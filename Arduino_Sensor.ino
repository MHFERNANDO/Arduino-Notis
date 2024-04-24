#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
// Credenciales de la red Wi-Fi
const char* WIFI_SSID = "Cresendo";
const char* WIFI_PASSWORD = "12345678";

// Credenciales de Firebase
const char* API_KEY = "AIzaSyD_MkXPV8uwMLv2BDiIie5tRfZG7MzSqf8";
const char* DATABASE_URL = "https://termometro-d3093-default-rtdb.firebaseio.com";//URL DE LA RTDB
const char* USER_EMAIL = "esp32@test.com";
const char* USER_PASSWORD = "12345678";

int cambio;
// Objeto para manejar la comunicación con Firebase
FirebaseData fbdo;

// Objeto para la autenticación en Firebase
FirebaseAuth auth;

// Objeto de configuración de Firebase
FirebaseConfig config;

// Tiempo desde el último envío de datos a Firebase
unsigned long sendDataPrevMillis = 0;

// Configura y establece la conexión Wi-Fi
void setup_WIFI(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
// Configura Firebase con las credenciales y la URL de la base de datos
void setupFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  config.signer.tokens.legacy_token = "<database secret>";
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
}

// Configuración inicial
void setup() {
    Serial.begin(115200);
    setup_WIFI();
    setupFirebase();
}

void loop() {
  // Verifica si Firebase está listo y si ha pasado cierto tiempo desde el último envío de datos
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    int ldr = analogRead(35); // Lee el valor del potenciómetro conectado al pin A0

      // Envía el estado del botón a la base de datos en tiempo real de Firebase
      if (Firebase.RTDB.setInt(&fbdo, F("/termometro"), ldr)) {
          Serial.println(ldr);
      } else {
          Serial.print("Failed to send data: ");
          Serial.println(fbdo.errorReason().c_str());
      }
  }
}