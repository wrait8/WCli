#include <WCli.h>

WCli cli;

WCliCommand cmd1, cmd2, cmd3, cmd4, cmd5;
WCliCommand mathCmd, stringCmd, systemCmd, debugCmd;

void cmd1Callback(WCliCommand* cmd) {
    Serial.println("Command 1 executed");
    for (int i = 0; i < cmd->getArgumentCount(); i++) {
        WCliArgument* arg = cmd->getArgument(i);
        if (arg) {
            Serial.print("Arg ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(arg->getValue());
        }
    }
}

void cmd2Callback(WCliCommand* cmd) {
    Serial.println("Command 2 executed with positional args");
}

void cmd3Callback(WCliCommand* cmd) {
    WCliArgument* nameArg = cmd->getArgument("name");
    WCliArgument* valueArg = cmd->getArgument("value");
    
    if (nameArg && nameArg->isSet()) {
        Serial.print("Name: ");
        Serial.println(nameArg->getValue());
    }
    if (valueArg && valueArg->isSet()) {
        Serial.print("Value: ");
        Serial.println(valueArg->getValue());
    }
}

void cmd4Callback(WCliCommand* cmd) {
    Serial.println("Command 4 - Flag example");
    WCliArgument* verboseArg = cmd->getArgument("verbose");
    WCliArgument* quietArg = cmd->getArgument("quiet");
    
    if (verboseArg && verboseArg->isSet()) {
        Serial.println("Verbose mode ON");
    }
    if (quietArg && quietArg->isSet()) {
        Serial.println("Quiet mode ON");
    }
}

void cmd5Callback(WCliCommand* cmd) {
    Serial.println("Command 5 - Boundless example");
    int count = cmd->getArgumentCount();
    Serial.print("Received ");
    Serial.print(count);
    Serial.println(" arguments");
}

void mathCallback(WCliCommand* cmd) {
    String operation = "";
    int result = 0;
    bool first = true;
    
    for (int i = 0; i < cmd->getArgumentCount(); i++) {
        WCliArgument* arg = cmd->getArgument(i);
        if (arg) {
            if (i == 0) {
                operation = arg->getValue();
            } else {
                int num = arg->getValue().toInt();
                if (first) {
                    result = num;
                    first = false;
                } else if (operation == "add") {
                    result += num;
                } else if (operation == "sub") {
                    result -= num;
                } else if (operation == "mul") {
                    result *= num;
                } else if (operation == "div") {
                    if (num != 0) result /= num;
                }
            }
        }
    }
    
    Serial.print("Math result: ");
    Serial.println(result);
}

void stringCallback(WCliCommand* cmd) {
    WCliArgument* textArg = cmd->getArgument("text");
    WCliArgument* countArg = cmd->getArgument("count");
    
    if (textArg && textArg->isSet()) {
        int count = countArg && countArg->isSet() ? countArg->getValue().toInt() : 1;
        for (int i = 0; i < count; i++) {
            Serial.println(textArg->getValue());
        }
    }
}

void systemCallback(WCliCommand* cmd) {
    WCliArgument* actionArg = cmd->getArgument("action");
    
    if (actionArg && actionArg->isSet()) {
        String action = actionArg->getValue();
        action.toLowerCase();
        
        if (action == "reset") {
            Serial.println("Resetting system...");
        } else if (action == "status") {
            cli.printStatus();
        } else if (action == "commands") {
            cli.printAllCommands();
        } else {
            Serial.println("Unknown action: " + action);
        }
    }
}

void debugCallback(WCliCommand* cmd) {
    WCliArgument* levelArg = cmd->getArgument("level");
    WCliArgument* moduleArg = cmd->getArgument("module");
    
    int level = levelArg && levelArg->isSet() ? levelArg->getValue().toInt() : 0;
    String module = moduleArg && moduleArg->isSet() ? moduleArg->getValue() : "all";
    
    Serial.print("Debug level: ");
    Serial.print(level);
    Serial.print(" | Module: ");
    Serial.println(module);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    Serial.println("\n=== Multiple Commands Example ===");
    Serial.println("This shows different types of commands");
    
    cmd1 = cli.addCommand("cmd1", cmd1Callback);
    cmd1.setDescription("Normal command with args");
    cmd1.addPositionalArgument("arg1", "default1");
    cmd1.addPositionalArgument("arg2", "default2");
    
    cmd2 = cli.addCommand("cmd2", cmd2Callback);
    cmd2.setDescription("Another normal command");
    cmd2.addArgument("option", "value");
    
    cmd3 = cli.addCommand("cmd3", cmd3Callback);
    cmd3.setDescription("Command with named args");
    cmd3.addArgument("name", nullptr);
    cmd3.addArgument("value", nullptr);
    
    cmd4 = cli.addCommand("cmd4", cmd4Callback);
    cmd4.setDescription("Command with flags");
    cmd4.addFlagArgument("verbose");
    cmd4.addFlagArgument("quiet");
    
    cmd5 = cli.addBoundlessCommand("cmd5", cmd5Callback);
    cmd5.setDescription("Boundless command");
    
    mathCmd = cli.addBoundlessCommand("math", mathCallback);
    mathCmd.setDescription("Math operations: math add 1 2 3");
    
    stringCmd = cli.addCommand("string", stringCallback);
    stringCmd.setDescription("String operations");
    stringCmd.addArgument("text", nullptr);
    stringCmd.addArgument("count", "1");
    
    systemCmd = cli.addCommand("system", systemCallback);
    systemCmd.setDescription("System operations");
    systemCmd.addArgument("action", "status");
    
    debugCmd = cli.addCommand("debug", debugCallback);
    debugCmd.setDescription("Debug operations");
    debugCmd.addArgument("level", "0");
    debugCmd.addArgument("module", "all");
    
    cli.addCommand("help", [](WCliCommand* cmd) {
        cli.printHelp();
    })->setDescription("Show this help");
    
    cli.addCommand("history", [](WCliCommand* cmd) {
        Serial.println("=== Command History ===");
        cli.getHistory().print();
        Serial.println("======================");
    })->setDescription("Show command history");
    
    Serial.println("\nTry these commands:");
    Serial.println("  cmd1 test1 test2");
    Serial.println("  cmd3 -name John -value Doe");
    Serial.println("  cmd4 -verbose");
    Serial.println("  cmd5 a b c d e");
    Serial.println("  math add 5 3 2");
    Serial.println("  string -text Hello -count 3");
}

void loop() {
    cli.process();
}
