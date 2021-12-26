#include <Preferences.h>

Preferences preferences;

const float EW = 0;
const float FW = 5;

void setup() {
  Serial.begin(115200);
  Serial.println();

  preferences.begin("weight", false);
  preferences.putFloat("EW", EW); 
  preferences.putFloat("FW", FW);

  Serial.println("Default Weight Saved using Preferences");

  preferences.end();
}

void loop() {

}
