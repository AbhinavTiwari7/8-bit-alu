#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char* ssid = "8bit_ALU";
const char* password = "ALU_12345";
const int ledPins[9] = {D0, D1, D2, D3, D4, D5, D6, D7, D8}; // 0: Carry, 1-8: Bit 7 to Bit 0

ESP8266WebServer server(80);
DNSServer dns;

struct Example { const char* a; const char* b; const char* op; const char* name; };
Example examples[] = {
  {"00001101", "00000111", "ADD", "ADD"},
  {"10000000", "01000000", "SUB", "SUB"},
  {"00010010", "00000101", "MUL", "MUL"},
  {"00101000", "00000110", "DIV", "DIV"}
};

void ledAnimation() {
  digitalWrite(ledPins[0], HIGH); delay(40); digitalWrite(ledPins[0], LOW);
  for (int i = 1; i < 9; i++) { digitalWrite(ledPins[i], HIGH); delay(40); digitalWrite(ledPins[i], LOW); }
}

String makePage(String a, String b, String op, int result, int carry) {
  String binResult = "";
  for (int i = 7; i >= 0; i--) binResult += String((result >> i) & 1);

  String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<title>NodeMCU 8-bit ALU</title>";
  page += "<style>";
  page += "body { font-family: Arial; text-align:center; background:#121212; color:#fff; margin:0; }";
  page += "h2 { color:#00eaff; margin-top:32px; }";
  page += "form { margin:18px; padding:18px 10px; background:#1e1e1e; border-radius:12px; display:inline-block; box-shadow:0 2px 18px #1115; }";
  page += "input, select { padding:8px; margin:5px 0; border-radius:6px; border:none; background:#252525; color:#00eaff; font-size:1.12em; }";
  page += "input[type=submit] { background:#00eaff; color:#121212; font-weight:bold; border-radius:6px; border:none; margin-top:8px; }";
  page += "input[type=submit]:hover { filter: brightness(1.1); cursor:pointer; }";
  page += "hr { margin:28px 0; border:0; border-top:1.5px solid #222; }";
  page += ".led-panel { margin:20px auto 16px; display:flex; justify-content:center; gap:10px; }";
  page += ".led { width:32px; height:32px; border-radius:50%; background:#222; border:2.5px solid #00eaff; display:inline-flex; align-items:center; justify-content:center; color:#00eaff; font-size:1.2em; font-weight:700; box-shadow:0 0 10px #00eaff44; }";
  page += ".led.on { background:#00eaff; color:#191919; box-shadow:0 0 16px #00eaff; }";
  page += ".carry { border:2.5px solid #ffe800; background:#ffe800; color:#121212; font-weight:bold; box-shadow:0 0 16px #ffe800aa;}";
  page += ".carry.off { background:#222; color:#ffd600; border:2.5px solid #ebe976; box-shadow:none;}";
  page += ".control-bar{ margin-bottom:20px; }";
  page += ".btnex { font-family: Arial; font-weight:bold; padding:8px 18px; margin:4px; background:#00eaff; color:#121212; border:none; border-radius:6px; transition:.13s; box-shadow:0 2px 8px #00eaff44;}";
  page += ".btnex:hover { background:#ffe800; color:#222; box-shadow:0 2px 18px #ffe80088;}";
  page += ".btnreset { font-family: Arial; font-weight:bold; padding:8px 18px; margin:4px; background:#ff6b6b; color:#fff; border:none; border-radius:6px; box-shadow:0 2px 8px #d43c3c44;}";
  page += ".btnreset:hover { background:#c81b1b; }";
  page += "@media(max-width:600px){form{width:97vw;padding:8vw 1vw;}}";
  page += "</style></head><body>";

  page += "<h2>NodeMCU 8-bit ALU</h2>";

  // Example and Reset bar
  page += "<div class='control-bar'>";
  for (int i=0;i<4;i++)
    page += "<form style='display:inline;' action='/example" + String(i+1) + "' method='GET'><button class='btnex' type='submit'>" + String(examples[i].name) + "</button></form>";
  page += "<form style='display:inline;' action='/reset' method='GET'><button class='btnreset' type='submit'>Reset</button></form>";
  page += "</div>";

  // Input Form
  page += "<form action='/' method='GET'>";
  page += "A (bin 00000000-11111111): <input type='text' name='a' pattern='[0-1]{1,8}' maxlength='8' value='" + a + "'><br>";
  page += "B (bin 00000000-11111111): <input type='text' name='b' pattern='[0-1]{1,8}' maxlength='8' value='" + b + "'><br>";
  page += "Operation: <select name='op'>";
  String ops[4] = {"ADD","SUB","MUL","DIV"};
  for (int i=0;i<4;i++) {
    page += "<option value='" + ops[i] + "'";
    if (op == ops[i]) page += " selected";
    page += ">" + ops[i] + "</option>";
  }
  page += "</select><br><input type='submit' value='Compute'></form>";

  page += "<hr>";
  page += "<h3>Result: " + String(result) + " (decimal)</h3>";

  // LEDs (Carry + Bits 7..0)
  page += "<div class='led-panel'>";
  page += "<div class='led carry " + String(carry ? "" : "off") + "'>C</div>";
  for (int i=7;i>=0;i--) {
    bool bitOn = ((result >> i) & 1);
    page += "<div class='led " + String(bitOn ? "on" : "") + "'>" + String((result >> i)&1) + "</div>";
  }
  page += "</div>";

  // REMOVING: bit number labels under the LEDs
  // [NO .led-labels line as you requested]

  page += "<h3>Binary: " + binResult + "</h3>";
  page += "<p>Wi-Fi SSID: <b>" + String(ssid) + "</b><br>Password: <b>" + String(password) + "</b></p>";
  page += "</body></html>";
  return page;
}

void displayResult(String a, String b, String op) {
  uint8_t aval = 0, bval = 0; uint16_t wideResult = 0; int result = 0, carry = 0;
  if (a.length() > 0 && b.length() > 0) {
    aval = strtol(a.c_str(), NULL, 2);
    bval = strtol(b.c_str(), NULL, 2);
    if (op == "ADD") { wideResult = (uint16_t)aval + (uint16_t)bval;
      carry = (wideResult & 0x100) ? 1 : 0; result = wideResult & 0xFF;
    } else if (op == "SUB") {
      if (aval < bval) { carry = 1; result = 0; }
      else { result = aval - bval; carry = 0; }
    } else if (op == "MUL") { result = aval * bval; carry = 0; }
    else if (op == "DIV") { result = (bval == 0) ? 0 : (aval / bval); carry = 0; }
  }
  digitalWrite(ledPins[0], carry); // Carry LED
  for (int i = 1; i < 9; i++) digitalWrite(ledPins[i], (result >> (8-i)) & 1);
  String binResult = "";
  for (int i = 7; i >= 0; i--) binResult += String((result >> i) & 1);
  server.send(200, "text/html", makePage(a, b, op, result, carry));
}

void handleRoot() {
  String avalStr = server.arg("a"), bvalStr = server.arg("b"), op = server.arg("op");
  if (avalStr.length() > 0 && bvalStr.length() > 0 && op.length() > 0) displayResult(avalStr, bvalStr, op);
  else displayResult("", "", "ADD");
}

void handleExample(int idx) { displayResult(examples[idx].a, examples[idx].b, examples[idx].op); }

void handleReset() {
  ledAnimation();
  for (int i = 0; i < 9; i++) digitalWrite(ledPins[i], LOW);
  server.send(200, "text/html", makePage("", "", "ADD", 0, 0));
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 9; i++) { pinMode(ledPins[i], OUTPUT); digitalWrite(ledPins[i], LOW); }
  ledAnimation();
  WiFi.softAP(ssid, password);
  dns.start(53, "*", WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  for (int i = 1; i <= 4; i++) server.on("/example" + String(i), [i]() { handleExample(i - 1); });
  server.begin();
}
void loop(){ server.handleClient(); dns.processNextRequest(); }
