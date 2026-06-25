#include <WCli.h>

WCli cli;

WCliCommand config;
WCliCommand blink;
WCliCommand timer;
WCliCommand info;

int blinkDelay = 500;
bool blinkEnabled = false;
unsigned long lastBlink = 0;
bool ledState = false;

void configCallback(WCliCommand* cmd) {
    WCliArgument* paramArg = cmd->getArgument("param");
    WCliArgument* valueArg = cmd->getArgument("value");
    
    if (!paramArg || !paramArg->isSet()) {
        Serial.println("Usage: config -param <parameter> -value <value>");
        Serial.println("Parameters: delay, mode");
        return;
    }
    
    String param = paramArg->getValue();
    param.toLowerCase();
    
    if (param == "delay") {
        if (valueArg && valueArg->isSet()) {
            blinkDelay = valueArg->getValue().toInt();
            Serial.print("Blink delay set to ");
            Serial.print(blinkDelay);
            Serial.println("ms");
        } else {
            Serial.println("Please specify a value for delay");
        }
    } else if (param == "mode") {
        if (valueArg && valueArg->isSet()) {
            String mode = valueArg->getValue();
            mode.toLowerCase();
            blinkEnabled = (mode == "on" || mode == "enable" || mode == "1");
            Serial.print("Blink mode set to ");
            Serial.println(blinkEnabled ? "ON" : "OFF");
        } else {
            Serial.println("Please specify a value for mode (on/off)");
        }
    } else {
        Serial.print("Unknown parameter: ");
        Serial.println(param);
    }
}

void blinkCallback(WCliCommand* cmd) {
    WCliArgument* countArg = cmd->getArgument("count");
    WCliArgument* delayArg = cmd->getArgument("delay");
    
    int count = 5;
    int delayMs = 200;
    
    if (countArg && countArg->isSet()) {
        count = countArg->getValue().toInt();
    }
    if (delayArg && delayArg->isSet()) {
        delayMs = delayArg->getValue().toInt();
    }
    
    Serial.print("Blinking LED ");
    Serial.print(count);
    Serial.print(" times with ");
    Serial.print(delayMs);
    Serial.println("ms delay");
    
    for (int i = 0; i < count; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_BUILTIN, LOW);
        if (i < count - 1) delay(delayMs);
    }
}

void timerCallback(WCliCommand* cmd) {
    WCliArgument* secondsArg = cmd->getArgument("seconds");
    
    if (secondsArg && secondsArg->isSet()) {
        int seconds = secondsArg->getValue().toInt();
        Serial.print("Starting timer for ");
        Serial.print(seconds);
        Serial.println(" seconds...");
        
        for (int i = seconds; i > 0; i--) {
            Serial.print(i);
            Serial.print("... ");
            delay(1000);
        }
        Serial.println("DONE!");
    } else {
        Serial.println("Usage: timer -seconds <number>");
    }
}

void infoCallback(WCliCommand* cmd) {
    Serial.println("\n=== System Information ===");
    Serial.print("Free RAM: ");
    Serial.println(WCli::getFreeRam());
    Serial.print("Blink Delay: ");
    Serial.println(blinkDelay);
    Serial.print("Blink Enabled: ");
    Serial.println(blinkEnabled ? "Yes" : "No");
    Serial.print("Commands Registered: ");
    Serial.println(cli.getCommandCount());
    cli.printStatus();
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    pinMode(LED_BUILTIN, OUTPUT);
    
    Serial.println("\n=== Advanced Example ===");
    Serial.println("Type 'help' for available commands");
    
    config = cli.addCommand("config", configCallback);
    config.setDescription("Configure system parameters");
    config.addArgument("param", nullptr);
    config.addArgument("value", nullptr);
    
    blink = cli.addCommand("blink", blinkCallback);
    blink.setDescription("Blink LED specified times");
    blink.addPositionalArgument("count", "5");
    blink.addArgument("delay", "200");
    
    timer = cli.addSingleArgumentCommand("timer", timerCallback);
    timer.setDescription("Start a countdown timer");
    timer.addPositionalArgument("seconds", "10");
    
    info = cli.addCommand("info", infoCallback);
    info.setDescription("Show system information");
    
    cli.addCommand("help", [](WCliCommand* cmd) {
        cli.printHelp();
    })->setDescription("Show this help");
    
    cli.setEchoEnabled(true);
    cli.setCaseSensitive(false);
}

void loop() {
    cli.process();
    
    if (blinkEnabled) {
        if (millis() - lastBlink > blinkDelay) {
            lastBlink = millis();
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
        }
    }
}
