#include <Log.hpp>

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <queue>
#include <cassert>

static unsigned int g_LogBufferSize = 10;

static std::vector<const char*> g_LogMessages;
static std::vector<const char*> g_LogErrors;
static std::vector<const char*> g_LogWarnings;

static bool g_LogMessagesToFile = false;
static bool g_LogErrorsToFile = false;
static bool g_LogWarningsToFile = false;

static char g_LogMessagesFilename[256];
static char g_LogErrorsFilename[256];
static char g_LogWarningsFilename[256];

static int ctr = 0;

[[nodiscard]] char* FormatMessage(const char* type, const char* message, va_list args)
{
    char buffer[256];
    vsnprintf(buffer + snprintf(buffer, sizeof(buffer), "[%s] ", type), sizeof(buffer), message, args);
    ctr++;
    return strdup(buffer);
}

void InsertMessage(std::vector<const char*>& buffer, const char* message)
{
    if(buffer.size() >= g_LogBufferSize){
        free((void*)buffer[0]);
        buffer.erase(buffer.begin());
        ctr--;
    }

    buffer.push_back(message);
} 

void LogMessage(const char* message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_MESSAGES_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogMessages, FormatMessage("MESSAGE", message, args));
    if(g_LogMessagesToFile){
        FILE* file = fopen(g_LogMessagesFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogMessages.back());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogMessages.back());
    }
    va_end(args);

    #endif
}

void LogError(const char* message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_ERRORS_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogErrors, FormatMessage("ERROR", message, args));
    if(g_LogErrorsToFile){
        FILE* file = fopen(g_LogErrorsFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogErrors.back());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogErrors.back());
    }
    va_end(args);

    #endif
}

void LogWarning(const char* message, ...)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_WARNINGS_ENABLED)

    va_list args;
    va_start(args, message);
    InsertMessage(g_LogWarnings, FormatMessage("WARNING", message, args));
    if(g_LogWarningsToFile){
        FILE* file = fopen(g_LogWarningsFilename, "a");
        if(file){
            fprintf(file, "%s\n", g_LogWarnings.back());
            fclose(file);
        }
    }else{
        printf("%s\n", g_LogWarnings.back());
    }
    va_end(args);

    #endif
}

const char* GetLogMessage(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_MESSAGES_ENABLED)

    if(index < g_LogMessages.size()){
        return g_LogMessages[g_LogMessages.size() - index - 1];
    }else{
        return nullptr;
    }

    #else
    return nullptr;
    #endif
}

const char* GetLogError(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_ERRORS_ENABLED)

    if(index < g_LogErrors.size()){
        return g_LogErrors[g_LogErrors.size() - index - 1];
    }else{
        return nullptr;
    }

    #else
    return nullptr;
    #endif
}

const char* GetLogWarning(unsigned int index)
{
    #if defined(DEBUG) || defined(LOGS_ENABLED) || defined(LOG_WARNINGS_ENABLED)

    if(index < g_LogWarnings.size()){
        return g_LogWarnings[g_LogWarnings.size() - index - 1];
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
    for(int i = 0; i < g_LogMessages.size(); i++){
        free((void*)g_LogMessages[i]);
        ctr--;
    }

    g_LogMessages.clear();
}
void ClearLogErrors()
{
    for(int i = 0; i < g_LogErrors.size(); i++){
        free((void*)g_LogErrors[i]);
        ctr--;
    }
    
    g_LogErrors.clear();
}
void ClearLogWarnings()
{
    for(int i = 0; i < g_LogWarnings.size(); i++){
        free((void*)g_LogWarnings[i]);
        ctr--;
    }

    g_LogWarnings.clear();
}

void ClearLogs()
{
    ClearLogMessages();
    ClearLogErrors();
    ClearLogWarnings();

    assert(ctr == 0); //check if all logs were cleared to avoid memory leaks
}

void LogMessagesToFile(const char* filename)
{
    g_LogMessagesToFile = true;
    strncpy(g_LogMessagesFilename, filename, sizeof(g_LogMessagesFilename) - 1);
}

void LogErrorsToFile(const char* filename)
{
    g_LogErrorsToFile = true;
    strncpy(g_LogErrorsFilename, filename, sizeof(g_LogErrorsFilename) - 1);
}

void LogWarningsToFile(const char* filename)
{
    g_LogWarningsToFile = true;
    strncpy(g_LogWarningsFilename, filename, sizeof(g_LogWarningsFilename) - 1);
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