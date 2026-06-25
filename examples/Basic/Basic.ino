#include <WCli.h>

// Create CLI Object
WCli cli;

// Commands
WCliCommand ping;
WCliCommand led;
WCliCommand sum;
WCliCommand helpCmd;

// Callback functions
void pingCallback(WCliCommand* cmd) {
    Serial.println("PONG!");
    
    if (cmd->getArgumentCount() > 0) {
        WCliArgument* arg = cmd->getArgument(0);
        if (arg) {
            Serial.print("With message: ");
            Serial.println(arg->getValue());
        }
    }
}

void ledCallback(WCliCommand* cmd) {
    WCliArgument* stateArg = cmd->getArgument("state");
    
    if (stateArg && stateArg->isSet()) {
        String state = stateArg->getValue();
        state.toLowerCase();
        
        if (state == "on") {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("LED turned ON");
        } else if (state == "off") {
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("LED turned OFF");
        } else {
            Serial.println("Usage: led -state on|off");
        }
    } else {
        Serial.println("Usage: led -state on|off");
    }
}

void sumCallback(WCliCommand* cmd) {
    int total = 0;
    int count = cmd->getArgumentCount();
    
    if (count == 0) {
        Serial.println("Usage: sum <num1> [num2] [num3] ...");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        WCliArgument* arg = cmd->getArgument(i);
        if (arg) {
            total += arg->getValue().toInt();
        }
    }
    
    Serial.print("Sum = ");
    Serial.println(total);
}

void helpCallback(WCliCommand* cmd) {
    cli.printHelp();
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.println("\n=== Basic Example ===");
    Serial.println("Type 'help' for commands");
    
    ping = cli.addCommand("ping", pingCallback);
    ping.setDescription("Responds with PONG!");
    ping.addPositionalArgument("message", "Hello");
    
    led = cli.addCommand("led", ledCallback);
    led.setDescription("Control built-in LED");
    led.addArgument("state", nullptr);
    
    sum = cli.addBoundlessCommand("sum", sumCallback);
    sum.setDescription("Sum all numbers");
    
    helpCmd = cli.addCommand("help", helpCallback);
    helpCmd.setDescription("Show this help message");
    
    cli.setErrorCallback([](WCliError* error) {
        Serial.print("[ERROR] ");
        Serial.println(error->toString());
    });
}

void loop() {
    cli.process();
}
