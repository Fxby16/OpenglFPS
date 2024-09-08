#include <Log.hpp>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <queue>

static unsigned int g_LogBufferSize = 10;

static std::vector<std::string> g_LogMessages;
static std::vector<std::string> g_LogErrors;
static std::vector<std::string> g_LogWarnings;

static bool g_LogMessagesToFile = false;
static bool g_LogErrorsToFile = false;
static bool g_LogWarningsToFile = false;

static char g_LogMessagesFilename[256];
static char g_LogErrorsFilename[256];
static char g_LogWarningsFilename[256];

char* FormatMessage(const std::string& type, const std::string& message, va_list args)
{
    char buffer[256];
    vsnprintf(buffer + snprintf(buffer, sizeof(buffer), "[%s] ", type.c_str()), sizeof(buffer), message.c_str(), args);
    return strdup(buffer);
}

void InsertMessage(std::vector<std::string>& buffer, const std::string& message)
{
    if(buffer.size() >= g_LogBufferSize){
        buffer.erase(buffer.begin());
    }

    buffer.push_back(message);
} 

void LogMessage(const std::string& message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_MESSAGES_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogMessages, FormatMessage("MESSAGE", message, args));
    if(g_LogMessagesToFile){
        FILE* file = fopen(g_LogMessagesFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogMessages.back().c_str());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogMessages.back().c_str());
    }
    va_end(args);

    #endif
}

void LogError(const std::string& message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_ERRORS_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogErrors, FormatMessage("ERROR", message, args));
    if(g_LogErrorsToFile){
        FILE* file = fopen(g_LogErrorsFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogErrors.back().c_str());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogErrors.back().c_str());
    }
    va_end(args);

    #endif
}

void LogWarning(const std::string& message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_WARNINGS_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogWarnings, FormatMessage("WARNING", message, args));
    if(g_LogWarningsToFile){
        FILE* file = fopen(g_LogWarningsFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogWarnings.back().c_str());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogWarnings.back().c_str());
    }
    va_end(args);

    #endif
}

const std::string* GetLogMessage(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_MESSAGES_ENABLED)

    if(index < g_LogMessages.size()){
        return &g_LogMessages[g_LogMessages.size() - index - 1];
    }else{
        return nullptr;
    }

    #else
    return nullptr;
    #endif
}

const std::string* GetLogError(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_ERRORS_ENABLED)

    if(index < g_LogErrors.size()){
        return &g_LogErrors[g_LogErrors.size() - index - 1];
    }else{
        return nullptr;
    }

    #else
    return nullptr;
    #endif
}

const std::string* GetLogWarning(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_WARNINGS_ENABLED)

    if(index < g_LogWarnings.size()){
        return &g_LogWarnings[g_LogWarnings.size() - index - 1];
    }else{
        return nullptr;
    }

    #else
    return nullptr;
    #endif
}

unsigned int GetLogMessagesCount()
{
    return g_LogMessages.size();
}
unsigned int GetLogErrorsCount()
{
    return g_LogErrors.size();
}
unsigned int GetLogWarningsCount()
{
    return g_LogWarnings.size();
}

void ClearLogMessages()
{
    g_LogMessages.clear();
}
void ClearLogErrors()
{
    g_LogErrors.clear();
}
void ClearLogWarnings()
{
    g_LogWarnings.clear();
}

void LogMessagesToFile(const std::string& filename)
{
    g_LogMessagesToFile = true;
    strncpy(g_LogMessagesFilename, filename.c_str(), sizeof(g_LogMessagesFilename) - 1);
}

void LogErrorsToFile(const std::string& filename)
{
    g_LogErrorsToFile = true;
    strncpy(g_LogErrorsFilename, filename.c_str(), sizeof(g_LogErrorsFilename) - 1);
}

void LogWarningsToFile(const std::string& filename)
{
    g_LogWarningsToFile = true;
    strncpy(g_LogWarningsFilename, filename.c_str(), sizeof(g_LogWarningsFilename) - 1);
}

void LogMessagesToConsole()
{
    g_LogMessagesToFile = false;
}

void LogErrorsToConsole()
{
    g_LogErrorsToFile = false;
}

void LogWarningsToConsole()
{
    g_LogWarningsToFile = false;
}