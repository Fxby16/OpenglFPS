#pragma once

#include <string>

extern void SetLogBufferSize(unsigned int size);
extern unsigned int GetLogBufferSize();

extern void LogMessage(const char* message, ...);
extern void LogError(const char* message, ...);
extern void LogWarning(const char* message, ...);

/**
 * \brief Get the log message at the specified index (last message is at index 0)
 * \param index The index of the message
 * \return A pointer to the message
 */
extern const std::string* GetLogMessage(unsigned int index);

/**
 * \brief Get the log error at the specified index (last error is at index 0)
 * \param index The index of the error
 * \return A pointer to the error message
 */
extern const std::string* GetLogError(unsigned int index);

/**
 * \brief Get the log warning at the specified index (last warning is at index 0)
 * \param index The index of the warning
 * \return A pointer to the warning message
 */
extern const std::string* GetLogWarning(unsigned int index);

extern unsigned int GetLogMessagesCount();
extern unsigned int GetLogErrorsCount();
extern unsigned int GetLogWarningsCount();

extern void ClearLogMessages();
extern void ClearLogErrors();
extern void ClearLogWarnings();

extern void LogMessagesToFile(const char* filename);
extern void LogErrorsToFile(const char* filename);
extern void LogWarningsToFile(const char* filename);

extern void LogMessagesToConsole();
extern void LogErrorsToConsole();
extern void LogWarningsToConsole();