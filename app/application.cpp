#include <SmingCore/SmingCore.h>

#include "user_config.h"

void init() {
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true);
	Serial.printf("Sming app skeleton ready\n");
}
