# WCli - Arduino Command Line Interface Library

A powerful, flexible, and easy-to-use command line interface library for Arduino projects. WCli provides a complete command parsing and execution system with support for named arguments, positional arguments, flags, and more.

## Features

- **Multiple Command Types**
  - Normal commands with named and positional arguments
  - Boundless commands (variable number of arguments)
  - Single-argument commands (everything after command name is one argument)

- **Rich Argument Support**
  - Named arguments (`-name value`)
  - Positional arguments (order-based)
  - Flag arguments (`-flag` - no value needed)
  - Default values for arguments
  - Required and optional arguments

- **Advanced Features**
  - Command history (stores last 20 commands)
  - Command and error queues
  - Customizable error handling
  - Command echo toggle
  - Case sensitivity toggle
  - Built-in help system
  - Command descriptions
  - Multiple command templates

- **Lightweight & Efficient**
  - Minimal memory footprint
  - Optimized for Arduino/Embedded systems
  - No external dependencies
  - Configurable queue sizes

## Installation

### Arduino IDE
1. Download the library as a ZIP file
2. In Arduino IDE: `Sketch → Include Library → Add .ZIP Library`
3. Select the downloaded ZIP file

### Manual Installation
1. Copy the `WCli` folder to your Arduino libraries directory:
   - Windows: `Documents/Arduino/libraries/`
   - Mac: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`
2. Restart Arduino IDE

## Quick Start

```cpp
#include <WCli.h>

// Create CLI instance
WCli cli;

// Command callback
void pingCallback(WCliCommand* cmd) {
    Serial.println("PONG!");
}

void setup() {
    Serial.begin(115200);
    
    // Add a command
    WCliCommand ping = cli.addCommand("ping", pingCallback);
    ping.setDescription("Responds with PONG!");
    
    // Add help command
    cli.addCommand("help", [](WCliCommand* cmd) {
        cli.printHelp();
    });
}

void loop() {
    cli.process();  // Process commands
}
```

## Usage Examples

### Basic Commands

```cpp
// Simple command without arguments
WCliCommand status = cli.addCommand("status", [](WCliCommand* cmd) {
    Serial.println("System is running");
});

// Command with positional arguments
WCliCommand blink = cli.addCommand("blink", [](WCliCommand* cmd) {
    WCliArgument* countArg = cmd->getArgument(0);
    if (countArg) {
        int count = countArg->getValue().toInt();
        for (int i = 0; i < count; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            if (i < count - 1) delay(500);
        }
    }
});
blink.addPositionalArgument("count", "5");  // Default: 5
```

### Named Arguments

```cpp
WCliCommand set = cli.addCommand("set", [](WCliCommand* cmd) {
    WCliArgument* pinArg = cmd->getArgument("pin");
    WCliArgument* stateArg = cmd->getArgument("state");
    
    if (pinArg && stateArg && pinArg->isSet() && stateArg->isSet()) {
        int pin = pinArg->getValue().toInt();
        int state = stateArg->getValue().toInt();
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);
        Serial.println("Pin set");
    }
});
set.addArgument("pin", nullptr);     // Required
set.addArgument("state", nullptr);   // Required
// Usage: set -pin 13 -state 1
```

### Flag Arguments

```cpp
WCliCommand debug = cli.addCommand("debug", [](WCliCommand* cmd) {
    WCliArgument* verboseArg = cmd->getArgument("verbose");
    if (verboseArg && verboseArg->isSet()) {
        Serial.println("Verbose mode ON");
    }
});
debug.addFlagArgument("verbose");
// Usage: debug -verbose
```

### Boundless Commands

```cpp
WCliCommand sum = cli.addBoundlessCommand("sum", [](WCliCommand* cmd) {
    int total = 0;
    for (int i = 0; i < cmd->getArgumentCount(); i++) {
        WCliArgument* arg = cmd->getArgument(i);
        if (arg) total += arg->getValue().toInt();
    }
    Serial.print("Sum = ");
    Serial.println(total);
});
// Usage: sum 1 2 3 4
```

### Single-Argument Commands

```cpp
WCliCommand echo = cli.addSingleArgumentCommand("echo", [](WCliCommand* cmd) {
    WCliArgument* textArg = cmd->getArgument(0);
    if (textArg) {
        Serial.println(textArg->getValue());
    }
});
// Usage: echo Hello World! (everything after "echo" is one argument)
```

## Advanced Features

### Command History

```cpp
// Access command history
WCliHistory& history = cli.getHistory();
history.print(Serial);  // Print all history

// Add to history manually
history.add("custom command");

// Clear history
cli.clearHistory();
```

### Error Handling

```cpp
// Global error handler
cli.setErrorCallback([](WCliError* error) {
    Serial.print("ERROR: ");
    Serial.println(error->toString());
    
    if (error->hasCommand()) {
        Serial.print("Command: ");
        Serial.println(error->getCommandName());
    }
});
```

### Command Callbacks

```cpp
// Global callback for all commands
cli.setCommandCallback([](WCliCommand* cmd) {
    Serial.print("Executing: ");
    Serial.println(cmd->getName());
});
```

### Queue Management

```cpp
// Check queue status
if (cli.available()) {
    WCliCommand* cmd = cli.getCommand();
    // Process command
}

// Check for errors
if (cli.errored()) {
    WCliError* error = cli.getError();
    Serial.println(error->toString());
}
```

## API Reference

### WCli Class

| Method | Description |
|--------|-------------|
| `begin(baudRate)` | Initialize CLI with baud rate |
| `addCommand(name, callback)` | Add a normal command |
| `addBoundlessCommand(name, callback)` | Add a boundless command |
| `addSingleArgumentCommand(name, callback)` | Add a single-argument command |
| `process()` | Main processing loop |
| `parse(input)` | Parse a command string |
| `printHelp()` | Print help information |
| `printStatus()` | Print system status |
| `setEchoEnabled(bool)` | Enable/disable command echo |
| `setCaseSensitive(bool)` | Set case sensitivity |
| `setErrorCallback(callback)` | Set global error handler |
| `getHistory()` | Get history manager |

### WCliCommand Class

| Method | Description |
|--------|-------------|
| `addArgument(name, defaultValue)` | Add a named argument |
| `addPositionalArgument(name, defaultValue)` | Add a positional argument |
| `addFlagArgument(name)` | Add a flag argument |
| `setDescription(text)` | Set command description |
| `getArgument(name/index)` | Get argument by name or index |
| `getArgumentCount()` | Get number of arguments |
| `getName()` | Get command name |
| `execute()` | Execute command callback |

### WCliArgument Class

| Method | Description |
|--------|-------------|
| `getName()` | Get argument name |
| `getValue()` | Get argument value |
| `isSet()` | Check if argument was provided |
| `isRequired()` | Check if argument is required |
| `hasDefaultValue()` | Check if argument has default |
| `getType()` | Get argument type |

## Examples

Check the `examples` folder for complete working examples:

- **Basic**: Simple command setup
- **Advanced**: Complex commands with various argument types
- **MultipleCommands**: Multiple command types and features

## Configuration

You can customize WCli by modifying the defines in `WCli.h`:

```cpp
#define WCLI_COMMAND_QUEUE_SIZE 20   // Max pending commands
#define WCLI_ERROR_QUEUE_SIZE 15     // Max pending errors
#define WCLI_MAX_COMMAND_NAME 32     // Max command name length
#define WCLI_MAX_ARGUMENT_NAME 32    // Max argument name length
#define WCLI_MAX_ARGUMENT_VALUE 64   // Max argument value length
#define WCLI_MAX_DESCRIPTION 128     // Max description length
#define WCLI_MAX_COMMANDS 30         // Max registered commands
#define WCLI_MAX_HISTORY 20          // Max command history
```

## Contributing

Contributions are welcome! Please submit pull requests or open issues on GitHub.

## Support

For issues and questions, please open an issue on GitHub or contact the maintainer.

---
