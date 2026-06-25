#ifndef WCli_h
#define WCli_h

#include <Arduino.h>
#include <stddef.h>
#include <string.h>

//==============================================================================
// Configuration Constants
//==============================================================================
#define WCLI_COMMAND_QUEUE_SIZE 20
#define WCLI_ERROR_QUEUE_SIZE 15
#define WCLI_MAX_COMMAND_NAME 32
#define WCLI_MAX_ARGUMENT_NAME 32
#define WCLI_MAX_ARGUMENT_VALUE 64
#define WCLI_MAX_DESCRIPTION 128
#define WCLI_MAX_COMMANDS 30
#define WCLI_MAX_HISTORY 20

//==============================================================================
// Enums
//==============================================================================
enum class WCliCommandType : uint8_t {
    NORMAL = 0,
    BOUNDLESS = 1,
    SINGLE_ARG = 2
};

enum class WCliArgumentType : uint8_t {
    NORMAL = 0,
    POSITIONAL = 1,
    FLAG = 2
};

enum class WCliErrorType : uint8_t {
    NONE = 0,
    NULL_POINTER = 1,
    EMPTY_LINE = 2,
    PARSE_SUCCESSFUL = 3,
    COMMAND_NOT_FOUND = 4,
    UNKNOWN_ARGUMENT = 5,
    MISSING_ARGUMENT = 6,
    MISSING_ARGUMENT_VALUE = 7,
    UNCLOSED_QUOTE = 8,
    COMMAND_QUEUE_FULL = 9,
    ERROR_QUEUE_FULL = 10
};

//==============================================================================
// Forward Declarations
//==============================================================================
class WCli;
class WCliCommand;
class WCliArgument;
class WCliError;

//==============================================================================
// WCliArgument Class
//==============================================================================
class WCliArgument {
private:
    struct arg_data {
        char name[WCLI_MAX_ARGUMENT_NAME];
        char value[WCLI_MAX_ARGUMENT_VALUE];
        char defaultValue[WCLI_MAX_ARGUMENT_VALUE];
        WCliArgumentType type;
        bool isSet;
        bool isRequired;
        bool hasDefault;
    }* data;
    bool persistent;
    
public:
    WCliArgument(bool persistent = true);
    WCliArgument(const WCliArgument& other);
    ~WCliArgument();
    
    WCliArgument& operator=(const WCliArgument& other);
    bool operator==(const WCliArgument& other) const;
    bool operator!=(const WCliArgument& other) const;
    explicit operator bool() const;
    
    // Setters
    void setName(const char* name);
    void setValue(const char* value);
    void setDefaultValue(const char* value);
    void setType(WCliArgumentType type);
    void setSet(bool set);
    void setRequired(bool required);
    void setHasDefault(bool hasDefault);
    
    // Getters
    const char* getName() const;
    const char* getValue() const;
    const char* getDefaultValue() const;
    WCliArgumentType getType() const;
    bool isSet() const;
    bool isRequired() const;
    bool hasDefaultValue() const;
    
    // Utilities
    bool equals(const char* name, bool caseSensitive = false) const;
    String toString() const;
    void clear();
};

//==============================================================================
// WCliError Class
//==============================================================================
class WCliError {
private:
    struct error_data {
        WCliErrorType type;
        char commandName[WCLI_MAX_COMMAND_NAME];
        char argumentName[WCLI_MAX_ARGUMENT_NAME];
        char data[WCLI_MAX_ARGUMENT_VALUE];
        char message[WCLI_MAX_DESCRIPTION];
        bool hasCommand;
        bool hasArgument;
        bool hasData;
    }* data;
    bool persistent;
    
public:
    WCliError(WCliErrorType type = WCliErrorType::NONE, bool persistent = true);
    WCliError(const WCliError& other);
    ~WCliError();
    
    WCliError& operator=(const WCliError& other);
    bool operator==(const WCliError& other) const;
    bool operator!=(const WCliError& other) const;
    explicit operator bool() const;
    
    // Getters
    WCliErrorType getType() const;
    bool hasCommand() const;
    bool hasArgument() const;
    bool hasData() const;
    const char* getCommandName() const;
    const char* getArgumentName() const;
    const char* getData() const;
    const char* getMessage() const;
    
    // Setters
    void setType(WCliErrorType type);
    void setCommandName(const char* name);
    void setArgumentName(const char* name);
    void setData(const char* data);
    void setMessage(const char* msg);
    
    // Utilities
    String toString() const;
    void clear();
};

//==============================================================================
// WCliCommand Class
//==============================================================================
class WCliCommand {
private:
    struct cmd_data {
        char name[WCLI_MAX_COMMAND_NAME];
        char description[WCLI_MAX_DESCRIPTION];
        WCliCommandType type;
        WCliArgument* arguments[10];
        uint8_t argumentCount;
        void (*callback)(WCliCommand*);
        bool hasCallback;
    }* data;
    bool persistent;
    
public:
    WCliCommand(bool persistent = true);
    WCliCommand(const WCliCommand& other);
    ~WCliCommand();
    
    WCliCommand& operator=(const WCliCommand& other);
    bool operator==(const WCliCommand& other) const;
    bool operator!=(const WCliCommand& other) const;
    explicit operator bool() const;
    
    // Argument Management
    WCliArgument* addArgument(const char* name, const char* defaultValue = nullptr);
    WCliArgument* addPositionalArgument(const char* name, const char* defaultValue = nullptr);
    WCliArgument* addFlagArgument(const char* name);
    bool removeArgument(const char* name);
    bool removeArgument(uint8_t index);
    void clearArguments();
    
    // Setters
    void setName(const char* name);
    void setDescription(const char* description);
    void setType(WCliCommandType type);
    void setCallback(void (*callback)(WCliCommand*));
    void setHasCallback(bool hasCallback);
    
    // Getters
    const char* getName() const;
    const char* getDescription() const;
    WCliCommandType getType() const;
    uint8_t getArgumentCount() const;
    WCliArgument* getArgument(const char* name) const;
    WCliArgument* getArgument(uint8_t index) const;
    bool hasCallback() const;
    
    // Utilities
    bool equals(const char* name, bool caseSensitive = false) const;
    String toString(bool includeDescription = true) const;
    void execute();
    void clear();
};

//==============================================================================
// WCliHistory Class
//==============================================================================
class WCliHistory {
private:
    String history[WCLI_MAX_HISTORY];
    uint8_t currentIndex;
    uint8_t count;
    bool enabled;
    
public:
    WCliHistory();
    
    void add(const String& command);
    void clear();
    String get(uint8_t index) const;
    String getLast() const;
    String getFirst() const;
    uint8_t getCount() const;
    uint8_t getMaxCount() const;
    bool isEnabled() const;
    void setEnabled(bool enable);
    void print(Stream& output = Serial) const;
    String toString() const;
};

//==============================================================================
// WCli Main Class
//==============================================================================
class WCli {
private:
    // Command storage
    WCliCommand* commands[WCLI_MAX_COMMANDS];
    uint8_t commandCount;
    
    // Queues
    WCliCommand* commandQueue[WCLI_COMMAND_QUEUE_SIZE];
    uint8_t commandQueueHead;
    uint8_t commandQueueTail;
    uint8_t commandQueueCount;
    
    WCliError* errorQueue[WCLI_ERROR_QUEUE_SIZE];
    uint8_t errorQueueHead;
    uint8_t errorQueueTail;
    uint8_t errorQueueCount;
    
    // History
    WCliHistory history;
    
    // Configuration
    bool caseSensitive;
    bool paused;
    bool echoEnabled;
    char delimiter;
    char quoteChar;
    
    // Callbacks
    void (*errorCallback)(WCliError*);
    void (*commandCallback)(WCliCommand*);
    
    // Internal methods
    WCliCommand* _findCommand(const char* name) const;
    bool _addToCommandQueue(WCliCommand* cmd);
    bool _addToErrorQueue(WCliError* error);
    WCliCommand* _popCommandQueue();
    WCliError* _popErrorQueue();
    void _parseCommand(const char* input);
    bool _parseArguments(WCliCommand* cmd, char* args, uint8_t argCount);
    uint8_t _splitArguments(char* input, char** args, uint8_t maxArgs);
    void _handleError(WCliErrorType type, const char* commandName = nullptr, 
                      const char* argumentName = nullptr, const char* data = nullptr);
    
public:
    // Constructor
    WCli();
    ~WCli();
    
    // Initialization
    void begin(long baudRate = 115200);
    void end();
    
    // Configuration
    void setCaseSensitive(bool sensitive);
    void setDelimiter(char delim);
    void setQuoteChar(char quote);
    void setEchoEnabled(bool enable);
    void setPaused(bool pause);
    void setErrorCallback(void (*callback)(WCliError*));
    void setCommandCallback(void (*callback)(WCliCommand*));
    
    // Command Management - Core methods
    WCliCommand* addCommand(const char* name, 
                           void (*callback)(WCliCommand*) = nullptr,
                           WCliCommandType type = WCliCommandType::NORMAL);
    WCliCommand* addBoundlessCommand(const char* name, void (*callback)(WCliCommand*) = nullptr);
    WCliCommand* addSingleArgumentCommand(const char* name, void (*callback)(WCliCommand*) = nullptr);
    bool removeCommand(const char* name);
    bool removeCommand(WCliCommand* cmd);
    void clearCommands();
    WCliCommand* getCommand(const char* name) const;
    WCliCommand* getCommand(uint8_t index) const;
    uint8_t getCommandCount() const;
    
    // Argument Helpers
    WCliArgument* addArgument(const char* commandName, const char* argName, 
                              const char* defaultValue = nullptr);
    WCliArgument* addPositionalArgument(const char* commandName, const char* argName,
                                        const char* defaultValue = nullptr);
    WCliArgument* addFlagArgument(const char* commandName, const char* argName);
    
    // Parsing
    void parse(const char* input);
    void parse(const String& input);
    void parseFromSerial();
    
    // Processing
    void process();
    bool available() const;
    bool errored() const;
    bool paused() const;
    
    // Queue Management
    WCliCommand* getCommand();
    WCliError* getError();
    uint8_t getCommandQueueCount() const;
    uint8_t getErrorQueueCount() const;
    void clearCommandQueue();
    void clearErrorQueue();
    
    // History
    WCliHistory& getHistory();
    void clearHistory();
    
    // Utilities
    String getAllCommandsString(bool includeDescriptions = true) const;
    void printAllCommands(Stream& output = Serial) const;
    void printHelp(Stream& output = Serial) const;
    void printStatus(Stream& output = Serial) const;
    
    // Static utilities
    static String escapeString(const String& input);
    static String unescapeString(const String& input);
    static bool isValidCommandName(const char* name);
    static uint32_t getFreeRam();
};

#endif
