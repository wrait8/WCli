#include "WCli.h"

//==============================================================================
// WCliArgument Implementation
//==============================================================================
WCliArgument::WCliArgument(bool persistent) : data(nullptr), persistent(persistent) {
    if (persistent) {
        data = new arg_data;
        if (data) {
            memset(data, 0, sizeof(arg_data));
            data->type = WCliArgumentType::NORMAL;
            data->isRequired = false;
            data->hasDefault = false;
            data->isSet = false;
        }
    }
}

WCliArgument::WCliArgument(const WCliArgument& other) {
    persistent = other.persistent;
    if (persistent && other.data) {
        data = new arg_data;
        if (data) {
            memcpy(data, other.data, sizeof(arg_data));
        }
    } else {
        data = other.data;
    }
}

WCliArgument::~WCliArgument() {
    if (persistent && data) {
        delete data;
        data = nullptr;
    }
}

WCliArgument& WCliArgument::operator=(const WCliArgument& other) {
    if (this != &other) {
        if (persistent && data) {
            delete data;
            data = nullptr;
        }
        persistent = other.persistent;
        if (persistent && other.data) {
            data = new arg_data;
            if (data) {
                memcpy(data, other.data, sizeof(arg_data));
            }
        } else {
            data = other.data;
        }
    }
    return *this;
}

bool WCliArgument::operator==(const WCliArgument& other) const {
    if (!data || !other.data) return false;
    return strcmp(data->name, other.data->name) == 0 && data->type == other.data->type;
}

bool WCliArgument::operator!=(const WCliArgument& other) const {
    return !(*this == other);
}

WCliArgument::operator bool() const {
    return data != nullptr && strlen(data->name) > 0;
}

void WCliArgument::setName(const char* name) {
    if (data && name) {
        strncpy(data->name, name, WCLI_MAX_ARGUMENT_NAME - 1);
        data->name[WCLI_MAX_ARGUMENT_NAME - 1] = '\0';
    }
}

void WCliArgument::setValue(const char* value) {
    if (data && value) {
        strncpy(data->value, value, WCLI_MAX_ARGUMENT_VALUE - 1);
        data->value[WCLI_MAX_ARGUMENT_VALUE - 1] = '\0';
        data->isSet = true;
    }
}

void WCliArgument::setDefaultValue(const char* value) {
    if (data && value) {
        strncpy(data->defaultValue, value, WCLI_MAX_ARGUMENT_VALUE - 1);
        data->defaultValue[WCLI_MAX_ARGUMENT_VALUE - 1] = '\0';
        data->hasDefault = true;
    }
}

void WCliArgument::setType(WCliArgumentType type) {
    if (data) data->type = type;
}

void WCliArgument::setSet(bool set) {
    if (data) data->isSet = set;
}

void WCliArgument::setRequired(bool required) {
    if (data) data->isRequired = required;
}

void WCliArgument::setHasDefault(bool hasDefault) {
    if (data) data->hasDefault = hasDefault;
}

const char* WCliArgument::getName() const {
    return data ? data->name : "";
}

const char* WCliArgument::getValue() const {
    if (!data) return "";
    return data->isSet ? data->value : (data->hasDefault ? data->defaultValue : "");
}

const char* WCliArgument::getDefaultValue() const {
    return data ? data->defaultValue : "";
}

WCliArgumentType WCliArgument::getType() const {
    return data ? data->type : WCliArgumentType::NORMAL;
}

bool WCliArgument::isSet() const {
    return data && data->isSet;
}

bool WCliArgument::isRequired() const {
    return data && data->isRequired;
}

bool WCliArgument::hasDefaultValue() const {
    return data && data->hasDefault;
}

bool WCliArgument::equals(const char* name, bool caseSensitive) const {
    if (!data || !name) return false;
    if (caseSensitive) {
        return strcmp(data->name, name) == 0;
    } else {
        return strcasecmp(data->name, name) == 0;
    }
}

String WCliArgument::toString() const {
    if (!data) return "Invalid Argument";
    String result = data->name;
    result += " (";
    switch (data->type) {
        case WCliArgumentType::NORMAL: result += "Named"; break;
        case WCliArgumentType::POSITIONAL: result += "Positional"; break;
        case WCliArgumentType::FLAG: result += "Flag"; break;
    }
    result += ")";
    if (data->isSet) {
        result += " = \"";
        result += data->value;
        result += "\"";
    } else if (data->hasDefault) {
        result += " [default: \"";
        result += data->defaultValue;
        result += "\"]";
    }
    if (data->isRequired) {
        result += " [required]";
    }
    return result;
}

void WCliArgument::clear() {
    if (data) {
        memset(data, 0, sizeof(arg_data));
        data->type = WCliArgumentType::NORMAL;
        data->isRequired = false;
        data->hasDefault = false;
        data->isSet = false;
    }
}

//==============================================================================
// WCliError Implementation
//==============================================================================
WCliError::WCliError(WCliErrorType type, bool persistent) 
    : data(nullptr), persistent(persistent) {
    if (persistent) {
        data = new error_data;
        if (data) {
            memset(data, 0, sizeof(error_data));
            data->type = type;
        }
    }
}

WCliError::WCliError(const WCliError& other) {
    persistent = other.persistent;
    if (persistent && other.data) {
        data = new error_data;
        if (data) {
            memcpy(data, other.data, sizeof(error_data));
        }
    } else {
        data = other.data;
    }
}

WCliError::~WCliError() {
    if (persistent && data) {
        delete data;
        data = nullptr;
    }
}

WCliError& WCliError::operator=(const WCliError& other) {
    if (this != &other) {
        if (persistent && data) {
            delete data;
            data = nullptr;
        }
        persistent = other.persistent;
        if (persistent && other.data) {
            data = new error_data;
            if (data) {
                memcpy(data, other.data, sizeof(error_data));
            }
        } else {
            data = other.data;
        }
    }
    return *this;
}

bool WCliError::operator==(const WCliError& other) const {
    if (!data || !other.data) return false;
    return data->type == other.data->type &&
           strcmp(data->commandName, other.data->commandName) == 0;
}

bool WCliError::operator!=(const WCliError& other) const {
    return !(*this == other);
}

WCliError::operator bool() const {
    return data != nullptr && data->type != WCliErrorType::NONE;
}

WCliErrorType WCliError::getType() const {
    return data ? data->type : WCliErrorType::NONE;
}

bool WCliError::hasCommand() const {
    return data && data->hasCommand;
}

bool WCliError::hasArgument() const {
    return data && data->hasArgument;
}

bool WCliError::hasData() const {
    return data && data->hasData;
}

const char* WCliError::getCommandName() const {
    return data ? data->commandName : "";
}

const char* WCliError::getArgumentName() const {
    return data ? data->argumentName : "";
}

const char* WCliError::getData() const {
    return data ? data->data : "";
}

const char* WCliError::getMessage() const {
    return data ? data->message : "";
}

void WCliError::setType(WCliErrorType type) {
    if (data) data->type = type;
}

void WCliError::setCommandName(const char* name) {
    if (data && name) {
        strncpy(data->commandName, name, WCLI_MAX_COMMAND_NAME - 1);
        data->commandName[WCLI_MAX_COMMAND_NAME - 1] = '\0';
        data->hasCommand = true;
    }
}

void WCliError::setArgumentName(const char* name) {
    if (data && name) {
        strncpy(data->argumentName, name, WCLI_MAX_ARGUMENT_NAME - 1);
        data->argumentName[WCLI_MAX_ARGUMENT_NAME - 1] = '\0';
        data->hasArgument = true;
    }
}

void WCliError::setData(const char* dataStr) {
    if (data && dataStr) {
        strncpy(data->data, dataStr, WCLI_MAX_ARGUMENT_VALUE - 1);
        data->data[WCLI_MAX_ARGUMENT_VALUE - 1] = '\0';
        data->hasData = true;
    }
}

void WCliError::setMessage(const char* msg) {
    if (data && msg) {
        strncpy(data->message, msg, WCLI_MAX_DESCRIPTION - 1);
        data->message[WCLI_MAX_DESCRIPTION - 1] = '\0';
    }
}

String WCliError::toString() const {
    if (!data) return "No error";
    
    String result = "ERROR: ";
    switch (data->type) {
        case WCliErrorType::NONE: result += "None"; break;
        case WCliErrorType::NULL_POINTER: result += "Null Pointer"; break;
        case WCliErrorType::EMPTY_LINE: result += "Empty Line"; break;
        case WCliErrorType::PARSE_SUCCESSFUL: result += "Parse Successful"; break;
        case WCliErrorType::COMMAND_NOT_FOUND: result += "Command Not Found"; break;
        case WCliErrorType::UNKNOWN_ARGUMENT: result += "Unknown Argument"; break;
        case WCliErrorType::MISSING_ARGUMENT: result += "Missing Argument"; break;
        case WCliErrorType::MISSING_ARGUMENT_VALUE: result += "Missing Argument Value"; break;
        case WCliErrorType::UNCLOSED_QUOTE: result += "Unclosed Quote"; break;
        case WCliErrorType::COMMAND_QUEUE_FULL: result += "Command Queue Full"; break;
        case WCliErrorType::ERROR_QUEUE_FULL: result += "Error Queue Full"; break;
        default: result += "Unknown";
    }
    
    if (data->hasCommand) {
        result += " | Command: ";
        result += data->commandName;
    }
    if (data->hasArgument) {
        result += " | Argument: ";
        result += data->argumentName;
    }
    if (data->hasData) {
        result += " | Data: ";
        result += data->data;
    }
    if (strlen(data->message) > 0) {
        result += " | Message: ";
        result += data->message;
    }
    return result;
}

void WCliError::clear() {
    if (data) {
        memset(data, 0, sizeof(error_data));
        data->type = WCliErrorType::NONE;
    }
}

//==============================================================================
// WCliCommand Implementation
//==============================================================================
WCliCommand::WCliCommand(bool persistent) : data(nullptr), persistent(persistent) {
    if (persistent) {
        data = new cmd_data;
        if (data) {
            memset(data, 0, sizeof(cmd_data));
            data->type = WCliCommandType::NORMAL;
            data->hasCallback = false;
            data->callback = nullptr;
            data->argumentCount = 0;
        }
    }
}

WCliCommand::WCliCommand(const WCliCommand& other) {
    persistent = other.persistent;
    if (persistent && other.data) {
        data = new cmd_data;
        if (data) {
            memcpy(data, other.data, sizeof(cmd_data));
            for (uint8_t i = 0; i < data->argumentCount; i++) {
                if (data->arguments[i]) {
                    data->arguments[i] = new WCliArgument(*other.data->arguments[i]);
                }
            }
        }
    } else {
        data = other.data;
    }
}

WCliCommand::~WCliCommand() {
    if (persistent && data) {
        for (uint8_t i = 0; i < data->argumentCount; i++) {
            delete data->arguments[i];
            data->arguments[i] = nullptr;
        }
        delete data;
        data = nullptr;
    }
}

WCliCommand& WCliCommand::operator=(const WCliCommand& other) {
    if (this != &other) {
        if (persistent && data) {
            for (uint8_t i = 0; i < data->argumentCount; i++) {
                delete data->arguments[i];
                data->arguments[i] = nullptr;
            }
            delete data;
            data = nullptr;
        }
        persistent = other.persistent;
        if (persistent && other.data) {
            data = new cmd_data;
            if (data) {
                memcpy(data, other.data, sizeof(cmd_data));
                for (uint8_t i = 0; i < data->argumentCount; i++) {
                    if (data->arguments[i]) {
                        data->arguments[i] = new WCliArgument(*other.data->arguments[i]);
                    }
                }
            }
        } else {
            data = other.data;
        }
    }
    return *this;
}

bool WCliCommand::operator==(const WCliCommand& other) const {
    if (!data || !other.data) return false;
    return strcmp(data->name, other.data->name) == 0 && data->type == other.data->type;
}

bool WCliCommand::operator!=(const WCliCommand& other) const {
    return !(*this == other);
}

WCliCommand::operator bool() const {
    return data != nullptr && strlen(data->name) > 0;
}

WCliArgument* WCliCommand::addArgument(const char* name, const char* defaultValue) {
    if (!data || !name || data->argumentCount >= 10) return nullptr;
    
    WCliArgument* arg = new WCliArgument(true);
    if (!arg) return nullptr;
    
    arg->setName(name);
    arg->setType(WCliArgumentType::NORMAL);
    if (defaultValue) {
        arg->setDefaultValue(defaultValue);
        arg->setHasDefault(true);
    }
    arg->setRequired(false);
    
    data->arguments[data->argumentCount++] = arg;
    return arg;
}

WCliArgument* WCliCommand::addPositionalArgument(const char* name, const char* defaultValue) {
    if (!data || !name || data->argumentCount >= 10) return nullptr;
    
    WCliArgument* arg = new WCliArgument(true);
    if (!arg) return nullptr;
    
    arg->setName(name);
    arg->setType(WCliArgumentType::POSITIONAL);
    if (defaultValue) {
        arg->setDefaultValue(defaultValue);
        arg->setHasDefault(true);
    }
    arg->setRequired(true);
    
    data->arguments[data->argumentCount++] = arg;
    return arg;
}

WCliArgument* WCliCommand::addFlagArgument(const char* name) {
    if (!data || !name || data->argumentCount >= 10) return nullptr;
    
    WCliArgument* arg = new WCliArgument(true);
    if (!arg) return nullptr;
    
    arg->setName(name);
    arg->setType(WCliArgumentType::FLAG);
    arg->setDefaultValue("false");
    arg->setHasDefault(true);
    arg->setRequired(false);
    
    data->arguments[data->argumentCount++] = arg;
    return arg;
}

bool WCliCommand::removeArgument(const char* name) {
    if (!data || !name) return false;
    
    for (uint8_t i = 0; i < data->argumentCount; i++) {
        if (data->arguments[i] && data->arguments[i]->equals(name, false)) {
            delete data->arguments[i];
            for (uint8_t j = i; j < data->argumentCount - 1; j++) {
                data->arguments[j] = data->arguments[j + 1];
            }
            data->argumentCount--;
            return true;
        }
    }
    return false;
}

bool WCliCommand::removeArgument(uint8_t index) {
    if (!data || index >= data->argumentCount) return false;
    
    delete data->arguments[index];
    for (uint8_t i = index; i < data->argumentCount - 1; i++) {
        data->arguments[i] = data->arguments[i + 1];
    }
    data->argumentCount--;
    return true;
}

void WCliCommand::clearArguments() {
    if (!data) return;
    for (uint8_t i = 0; i < data->argumentCount; i++) {
        delete data->arguments[i];
        data->arguments[i] = nullptr;
    }
    data->argumentCount = 0;
}

void WCliCommand::setName(const char* name) {
    if (data && name) {
        strncpy(data->name, name, WCLI_MAX_COMMAND_NAME - 1);
        data->name[WCLI_MAX_COMMAND_NAME - 1] = '\0';
    }
}

void WCliCommand::setDescription(const char* description) {
    if (data && description) {
        strncpy(data->description, description, WCLI_MAX_DESCRIPTION - 1);
        data->description[WCLI_MAX_DESCRIPTION - 1] = '\0';
    }
}

void WCliCommand::setType(WCliCommandType type) {
    if (data) data->type = type;
}

void WCliCommand::setCallback(void (*callback)(WCliCommand*)) {
    if (data) {
        data->callback = callback;
        data->hasCallback = (callback != nullptr);
    }
}

void WCliCommand::setHasCallback(bool hasCallback) {
    if (data) data->hasCallback = hasCallback;
}

const char* WCliCommand::getName() const {
    return data ? data->name : "";
}

const char* WCliCommand::getDescription() const {
    return data ? data->description : "";
}

WCliCommandType WCliCommand::getType() const {
    return data ? data->type : WCliCommandType::NORMAL;
}

uint8_t WCliCommand::getArgumentCount() const {
    return data ? data->argumentCount : 0;
}

WCliArgument* WCliCommand::getArgument(const char* name) const {
    if (!data || !name) return nullptr;
    for (uint8_t i = 0; i < data->argumentCount; i++) {
        if (data->arguments[i] && data->arguments[i]->equals(name, false)) {
            return data->arguments[i];
        }
    }
    return nullptr;
}

WCliArgument* WCliCommand::getArgument(uint8_t index) const {
    if (!data || index >= data->argumentCount) return nullptr;
    return data->arguments[index];
}

bool WCliCommand::hasCallback() const {
    return data && data->hasCallback;
}

bool WCliCommand::equals(const char* name, bool caseSensitive) const {
    if (!data || !name) return false;
    if (caseSensitive) {
        return strcmp(data->name, name) == 0;
    } else {
        return strcasecmp(data->name, name) == 0;
    }
}

String WCliCommand::toString(bool includeDescription) const {
    if (!data) return "Invalid Command";
    String result = data->name;
    if (includeDescription && strlen(data->description) > 0) {
        result += " - ";
        result += data->description;
    }
    return result;
}

void WCliCommand::execute() {
    if (data && data->hasCallback && data->callback) {
        data->callback(this);
    }
}

void WCliCommand::clear() {
    if (data) {
        clearArguments();
        memset(data, 0, sizeof(cmd_data));
        data->type = WCliCommandType::NORMAL;
        data->hasCallback = false;
        data->callback = nullptr;
        data->argumentCount = 0;
    }
}

//==============================================================================
// WCliHistory Implementation
//==============================================================================
WCliHistory::WCliHistory() : currentIndex(0), count(0), enabled(true) {
    for (uint8_t i = 0; i < WCLI_MAX_HISTORY; i++) {
        history[i] = "";
    }
}

void WCliHistory::add(const String& command) {
    if (!enabled || command.length() == 0) return;
    
    if (count > 0 && history[(currentIndex - 1 + WCLI_MAX_HISTORY) % WCLI_MAX_HISTORY] == command) {
        return;
    }
    
    history[currentIndex] = command;
    currentIndex = (currentIndex + 1) % WCLI_MAX_HISTORY;
    if (count < WCLI_MAX_HISTORY) {
        count++;
    }
}

void WCliHistory::clear() {
    for (uint8_t i = 0; i < WCLI_MAX_HISTORY; i++) {
        history[i] = "";
    }
    currentIndex = 0;
    count = 0;
}

String WCliHistory::get(uint8_t index) const {
    if (index >= count) return "";
    uint8_t realIndex = (currentIndex - count + index + WCLI_MAX_HISTORY) % WCLI_MAX_HISTORY;
    return history[realIndex];
}

String WCliHistory::getLast() const {
    if (count == 0) return "";
    uint8_t lastIndex = (currentIndex - 1 + WCLI_MAX_HISTORY) % WCLI_MAX_HISTORY;
    return history[lastIndex];
}

String WCliHistory::getFirst() const {
    if (count == 0) return "";
    uint8_t firstIndex = (currentIndex - count + WCLI_MAX_HISTORY) % WCLI_MAX_HISTORY;
    return history[firstIndex];
}

uint8_t WCliHistory::getCount() const {
    return count;
}

uint8_t WCliHistory::getMaxCount() const {
    return WCLI_MAX_HISTORY;
}

bool WCliHistory::isEnabled() const {
    return enabled;
}

void WCliHistory::setEnabled(bool enable) {
    enabled = enable;
}

void WCliHistory::print(Stream& output) const {
    if (count == 0) {
        output.println("(empty)");
        return;
    }
    for (uint8_t i = 0; i < count; i++) {
        output.print(i + 1);
        output.print(": ");
        output.println(get(i));
    }
}

String WCliHistory::toString() const {
    if (count == 0) return "(empty)";
    String result = "";
    for (uint8_t i = 0; i < count; i++) {
        result += String(i + 1) + ": " + get(i) + "\n";
    }
    return result;
}

//==============================================================================
// WCli Implementation
//==============================================================================
WCli::WCli() 
    : commandCount(0), 
      commandQueueHead(0), commandQueueTail(0), commandQueueCount(0),
      errorQueueHead(0), errorQueueTail(0), errorQueueCount(0),
      caseSensitive(false), paused(false), echoEnabled(true),
      delimiter(' '), quoteChar('"'),
      errorCallback(nullptr), commandCallback(nullptr) {
    
    memset(commands, 0, sizeof(commands));
    memset(commandQueue, 0, sizeof(commandQueue));
    memset(errorQueue, 0, sizeof(errorQueue));
}

WCli::~WCli() {
    clearCommands();
    clearCommandQueue();
    clearErrorQueue();
}

//==============================================================================
// Initialization
//==============================================================================
void WCli::begin(long baudRate) {
    Serial.begin(baudRate);
    while (!Serial) { delay(10); }
}

void WCli::end() {
    clearCommands();
    clearCommandQueue();
    clearErrorQueue();
}

//==============================================================================
// Configuration
//==============================================================================
void WCli::setCaseSensitive(bool sensitive) {
    caseSensitive = sensitive;
}

void WCli::setDelimiter(char delim) {
    delimiter = delim;
}

void WCli::setQuoteChar(char quote) {
    quoteChar = quote;
}

void WCli::setEchoEnabled(bool enable) {
    echoEnabled = enable;
}

void WCli::setPaused(bool pause) {
    paused = pause;
}

void WCli::setErrorCallback(void (*callback)(WCliError*)) {
    errorCallback = callback;
}

void WCli::setCommandCallback(void (*callback)(WCliCommand*)) {
    commandCallback = callback;
}

//==============================================================================
// Command Management
//==============================================================================
WCliCommand* WCli::addCommand(const char* name, void (*callback)(WCliCommand*), WCliCommandType type) {
    if (!name || commandCount >= WCLI_MAX_COMMANDS) return nullptr;
    if (!isValidCommandName(name)) return nullptr;
    if (_findCommand(name)) return nullptr;
    
    WCliCommand* cmd = new WCliCommand(true);
    if (!cmd) return nullptr;
    
    cmd->setName(name);
    cmd->setType(type);
    cmd->setCallback(callback);
    
    commands[commandCount++] = cmd;
    return cmd;
}

WCliCommand* WCli::addBoundlessCommand(const char* name, void (*callback)(WCliCommand*)) {
    return addCommand(name, callback, WCliCommandType::BOUNDLESS);
}

WCliCommand* WCli::addSingleArgumentCommand(const char* name, void (*callback)(WCliCommand*)) {
    return addCommand(name, callback, WCliCommandType::SINGLE_ARG);
}

bool WCli::removeCommand(const char* name) {
    if (!name) return false;
    
    for (uint8_t i = 0; i < commandCount; i++) {
        if (commands[i] && commands[i]->equals(name, caseSensitive)) {
            delete commands[i];
            commands[i] = nullptr;
            for (uint8_t j = i; j < commandCount - 1; j++) {
                commands[j] = commands[j + 1];
            }
            commandCount--;
            return true;
        }
    }
    return false;
}

bool WCli::removeCommand(WCliCommand* cmd) {
    if (!cmd) return false;
    return removeCommand(cmd->getName());
}

void WCli::clearCommands() {
    for (uint8_t i = 0; i < commandCount; i++) {
        delete commands[i];
        commands[i] = nullptr;
    }
    commandCount = 0;
}

WCliCommand* WCli::getCommand(const char* name) const {
    return _findCommand(name);
}

WCliCommand* WCli::getCommand(uint8_t index) const {
    if (index >= commandCount) return nullptr;
    return commands[index];
}

uint8_t WCli::getCommandCount() const {
    return commandCount;
}

//==============================================================================
// Argument Helpers
//==============================================================================
WCliArgument* WCli::addArgument(const char* commandName, const char* argName, 
                                const char* defaultValue) {
    WCliCommand* cmd = getCommand(commandName);
    if (!cmd) return nullptr;
    return cmd->addArgument(argName, defaultValue);
}

WCliArgument* WCli::addPositionalArgument(const char* commandName, const char* argName,
                                          const char* defaultValue) {
    WCliCommand* cmd = getCommand(commandName);
    if (!cmd) return nullptr;
    return cmd->addPositionalArgument(argName, defaultValue);
}

WCliArgument* WCli::addFlagArgument(const char* commandName, const char* argName) {
    WCliCommand* cmd = getCommand(commandName);
    if (!cmd) return nullptr;
    return cmd->addFlagArgument(argName);
}

//==============================================================================
// Parsing
//==============================================================================
void WCli::parse(const char* input) {
    if (paused || !input || strlen(input) == 0) {
        if (input && strlen(input) == 0) {
            _handleError(WCliErrorType::EMPTY_LINE);
        }
        return;
    }
    
    _parseCommand(input);
}

void WCli::parse(const String& input) {
    parse(input.c_str());
}

void WCli::parseFromSerial() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            if (echoEnabled) {
                Serial.print("> ");
                Serial.println(input);
            }
            history.add(input);
            parse(input);
        }
    }
}

//==============================================================================
// Processing
//==============================================================================
void WCli::process() {
    parseFromSerial();
    
    while (available()) {
        WCliCommand* cmd = getCommand();
        if (cmd) {
            if (commandCallback) {
                commandCallback(cmd);
            }
            cmd->execute();
        }
    }
    
    while (errored()) {
        WCliError* error = getError();
        if (error) {
            if (errorCallback) {
                errorCallback(error);
            } else {
                Serial.println(error->toString());
            }
        }
    }
}

bool WCli::available() const {
    return commandQueueCount > 0;
}

bool WCli::errored() const {
    return errorQueueCount > 0;
}

bool WCli::paused() const {
    return paused;
}

//==============================================================================
// Queue Management
//==============================================================================
WCliCommand* WCli::getCommand() {
    return _popCommandQueue();
}

WCliError* WCli::getError() {
    return _popErrorQueue();
}

uint8_t WCli::getCommandQueueCount() const {
    return commandQueueCount;
}

uint8_t WCli::getErrorQueueCount() const {
    return errorQueueCount;
}

void WCli::clearCommandQueue() {
    while (commandQueueCount > 0) {
        _popCommandQueue();
    }
}

void WCli::clearErrorQueue() {
    while (errorQueueCount > 0) {
        WCliError* error = _popErrorQueue();
        delete error;
    }
}

//==============================================================================
// History
//==============================================================================
WCliHistory& WCli::getHistory() {
    return history;
}

void WCli::clearHistory() {
    history.clear();
}

//==============================================================================
// Utilities
//==============================================================================
String WCli::getAllCommandsString(bool includeDescriptions) const {
    if (commandCount == 0) return "No commands registered";
    
    String result = "";
    for (uint8_t i = 0; i < commandCount; i++) {
        if (commands[i]) {
            result += commands[i]->toString(includeDescriptions);
            if (i < commandCount - 1) result += "\n";
        }
    }
    return result;
}

void WCli::printAllCommands(Stream& output) const {
    output.println("=== Registered Commands ===");
    if (commandCount == 0) {
        output.println("(none)");
    } else {
        for (uint8_t i = 0; i < commandCount; i++) {
            if (commands[i]) {
                output.print("  ");
                output.println(commands[i]->toString(true));
                for (uint8_t j = 0; j < commands[i]->getArgumentCount(); j++) {
                    WCliArgument* arg = commands[i]->getArgument(j);
                    if (arg) {
                        output.print("    ");
                        output.println(arg->toString());
                    }
                }
            }
        }
    }
    output.println("===========================");
}

void WCli::printHelp(Stream& output) const {
    output.println("\n=== WCli Help ===");
    output.println("Available Commands:");
    printAllCommands(output);
    output.println("\nUsage:");
    output.println("  <command> [arguments]");
    output.println("  -argument value  : Named argument");
    output.println("  <value>          : Positional argument");
    output.println("  -flag            : Flag argument");
    output.println("  \"quoted value\"   : Value with spaces");
    output.println("================\n");
}

void WCli::printStatus(Stream& output) const {
    output.println("\n=== WCli Status ===");
    output.print("Commands: ");
    output.print(commandCount);
    output.print("/");
    output.println(WCLI_MAX_COMMANDS);
    output.print("Command Queue: ");
    output.print(commandQueueCount);
    output.print("/");
    output.println(WCLI_COMMAND_QUEUE_SIZE);
    output.print("Error Queue: ");
    output.print(errorQueueCount);
    output.print("/");
    output.println(WCLI_ERROR_QUEUE_SIZE);
    output.print("History: ");
    output.print(history.getCount());
    output.print("/");
    output.println(WCLI_MAX_HISTORY);
    output.print("Paused: ");
    output.println(paused ? "Yes" : "No");
    output.print("Case Sensitive: ");
    output.println(caseSensitive ? "Yes" : "No");
    output.print("Echo: ");
    output.println(echoEnabled ? "On" : "Off");
    output.print("Free RAM: ");
    output.println(getFreeRam());
    output.println("==================\n");
}

//==============================================================================
// Static Utilities
//==============================================================================
String WCli::escapeString(const String& input) {
    String result = "";
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '"' || c == '\\') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

String WCli::unescapeString(const String& input) {
    String result = "";
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '\\' && i < input.length() - 1) {
            i++;
            c = input[i];
            if (c == 'n') {
                result += '\n';
            } else if (c == 't') {
                result += '\t';
            } else if (c == 'r') {
                result += '\r';
            } else {
                result += c;
            }
        } else {
            result += c;
        }
    }
    return result;
}

bool WCli::isValidCommandName(const char* name) {
    if (!name || strlen(name) == 0) return false;
    for (size_t i = 0; i < strlen(name); i++) {
        char c = name[i];
        if (!isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    return true;
}

uint32_t WCli::getFreeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (uint32_t)&v - (__brkval == 0 ? (uint32_t)&__heap_start : (uint32_t)__brkval);
}

//==============================================================================
// Internal Methods
//==============================================================================
WCliCommand* WCli::_findCommand(const char* name) const {
    if (!name) return nullptr;
    for (uint8_t i = 0; i < commandCount; i++) {
        if (commands[i] && commands[i]->equals(name, caseSensitive)) {
            return commands[i];
        }
    }
    return nullptr;
}

bool WCli::_addToCommandQueue(WCliCommand* cmd) {
    if (!cmd) return false;
    if (commandQueueCount >= WCLI_COMMAND_QUEUE_SIZE) {
        _handleError(WCliErrorType::COMMAND_QUEUE_FULL);
        return false;
    }
    
    commandQueue[commandQueueTail] = cmd;
    commandQueueTail = (commandQueueTail + 1) % WCLI_COMMAND_QUEUE_SIZE;
    commandQueueCount++;
    return true;
}

bool WCli::_addToErrorQueue(WCliError* error) {
    if (!error) return false;
    if (errorQueueCount >= WCLI_ERROR_QUEUE_SIZE) {
        WCliError* oldError = _popErrorQueue();
        delete oldError;
    }
    
    errorQueue[errorQueueTail] = error;
    errorQueueTail = (errorQueueTail + 1) % WCLI_ERROR_QUEUE_SIZE;
    errorQueueCount++;
    return true;
}

WCliCommand* WCli::_popCommandQueue() {
    if (commandQueueCount == 0) return nullptr;
    
    WCliCommand* cmd = commandQueue[commandQueueHead];
    commandQueue[commandQueueHead] = nullptr;
    commandQueueHead = (commandQueueHead + 1) % WCLI_COMMAND_QUEUE_SIZE;
    commandQueueCount--;
    return cmd;
}

WCliError* WCli::_popErrorQueue() {
    if (errorQueueCount == 0) return nullptr;
    
    WCliError* error = errorQueue[errorQueueHead];
    errorQueue[errorQueueHead] = nullptr;
    errorQueueHead = (errorQueueHead + 1) % WCLI_ERROR_QUEUE_SIZE;
    errorQueueCount--;
    return error;
}

void WCli::_parseCommand(const char* input) {
    // Simple parsing for now - will be enhanced
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    char* args[20];
    uint8_t argCount = _splitArguments(buffer, args, 20);
    
    if (argCount == 0) {
        _handleError(WCliErrorType::EMPTY_LINE);
        return;
    }
    
    WCliCommand* cmd = _findCommand(args[0]);
    if (!cmd) {
        _handleError(WCliErrorType::COMMAND_NOT_FOUND, args[0]);
        return;
    }
    
    if (argCount > 1) {
        _parseArguments(cmd, buffer, argCount);
    }
    
    _addToCommandQueue(cmd);
}

bool WCli::_parseArguments(WCliCommand* cmd, char* args, uint8_t argCount) {
    if (!cmd) return false;
    
    // Skip command name
    uint8_t startIndex = 1;
    
    // For single argument commands, treat everything as one argument
    if (cmd->getType() == WCliCommandType::SINGLE_ARG && argCount > startIndex) {
        String combined = "";
        for (uint8_t i = startIndex; i < argCount; i++) {
            if (i > startIndex) combined += " ";
            combined += args[i];
        }
        WCliArgument* arg = cmd->getArgument(0);
        if (arg) {
            arg->setValue(combined.c_str());
            arg->setSet(true);
        }
        return true;
    }
    
    // For boundless commands, each argument is separate
    if (cmd->getType() == WCliCommandType::BOUNDLESS) {
        for (uint8_t i = startIndex; i < argCount; i++) {
            WCliArgument* arg = cmd->addArgument(("arg" + String(i - startIndex)).c_str());
            if (arg) {
                arg->setValue(args[i]);
                arg->setSet(true);
            }
        }
        return true;
    }
    
    // Normal command parsing
    uint8_t positionalIndex = 0;
    for (uint8_t i = startIndex; i < argCount; i++) {
        char* argStr = args[i];
        
        // Check if it's a named argument
        if (argStr[0] == '-') {
            char* name = argStr + 1;
            char* value = nullptr;
            
            // Check if next argument is value
            if (i + 1 < argCount && args[i + 1][0] != '-') {
                value = args[i + 1];
                i++; // Skip value
            }
            
            WCliArgument* arg = cmd->getArgument(name);
            if (!arg) {
                _handleError(WCliErrorType::UNKNOWN_ARGUMENT, cmd->getName(), name);
                return false;
            }
            
            if (arg->getType() == WCliArgumentType::FLAG) {
                arg->setValue("true");
                arg->setSet(true);
            } else if (value) {
                arg->setValue(value);
                arg->setSet(true);
            } else {
                _handleError(WCliErrorType::MISSING_ARGUMENT_VALUE, cmd->getName(), name);
                return false;
            }
        } else {
            // Positional argument
            WCliArgument* arg = cmd->getArgument(positionalIndex);
            if (!arg || arg->getType() != WCliArgumentType::POSITIONAL) {
                _handleError(WCliErrorType::UNKNOWN_ARGUMENT, cmd->getName(), String(positionalIndex).c_str());
                return false;
            }
            arg->setValue(argStr);
            arg->setSet(true);
            positionalIndex++;
        }
    }
    
    // Check for missing required arguments
    for (uint8_t i = 0; i < cmd->getArgumentCount(); i++) {
        WCliArgument* arg = cmd->getArgument(i);
        if (arg && arg->isRequired() && !arg->isSet() && !arg->hasDefaultValue()) {
            _handleError(WCliErrorType::MISSING_ARGUMENT, cmd->getName(), arg->getName());
            return false;
        }
    }
    
    return true;
}

uint8_t WCli::_splitArguments(char* input, char** args, uint8_t maxArgs) {
    uint8_t count = 0;
    bool inQuotes = false;
    bool escapeNext = false;
    
    while (*input && count < maxArgs) {
        // Skip leading delimiters
        while (*input && (*input == delimiter || *input == '\t') && !inQuotes) {
            input++;
        }
        
        if (!*input) break;
        
        args[count] = input;
        count++;
        
        // Parse until delimiter or end
        while (*input) {
            if (escapeNext) {
                escapeNext = false;
                input++;
                continue;
            }
            
            if (*input == '\\') {
                escapeNext = true;
                input++;
                continue;
            }
            
            if (*input == quoteChar) {
                inQuotes = !inQuotes;
                // Remove quotes by shifting characters
                char* src = input;
                char* dst = input;
                while (*src) {
                    *dst = *src;
                    if (*src == quoteChar) {
                        src++;
                        continue;
                    }
                    dst++;
                    src++;
                }
                *dst = '\0';
                continue;
            }
            
            if (!inQuotes && (*input == delimiter || *input == '\t')) {
                *input = '\0';
                input++;
                break;
            }
            
            input++;
        }
    }
    
    return count;
}

void WCli::_handleError(WCliErrorType type, const char* commandName, 
                        const char* argumentName, const char* data) {
    WCliError* error = new WCliError(type);
    if (!error) return;
    
    if (commandName) error->setCommandName(commandName);
    if (argumentName) error->setArgumentName(argumentName);
    if (data) error->setData(data);
    
    // Set appropriate message
    String msg;
    switch (type) {
        case WCliErrorType::EMPTY_LINE:
            msg = "Empty command line";
            break;
        case WCliErrorType::COMMAND_NOT_FOUND:
            msg = "Command not found: ";
            msg += commandName ? commandName : "unknown";
            break;
        case WCliErrorType::UNKNOWN_ARGUMENT:
            msg = "Unknown argument: ";
            msg += argumentName ? argumentName : "unknown";
            break;
        case WCliErrorType::MISSING_ARGUMENT:
            msg = "Missing required argument: ";
            msg += argumentName ? argumentName : "unknown";
            break;
        case WCliErrorType::MISSING_ARGUMENT_VALUE:
            msg = "Missing value for argument: ";
            msg += argumentName ? argumentName : "unknown";
            break;
        case WCliErrorType::UNCLOSED_QUOTE:
            msg = "Unclosed quote in input";
            break;
        case WCliErrorType::COMMAND_QUEUE_FULL:
            msg = "Command queue is full";
            break;
        case WCliErrorType::ERROR_QUEUE_FULL:
            msg = "Error queue is full";
            break;
        default:
            msg = "Unknown error";
            break;
    }
    error->setMessage(msg.c_str());
    
    _addToErrorQueue(error);
}
