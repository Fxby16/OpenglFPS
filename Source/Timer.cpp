#include <Timer.hpp>

#include <cstdio>
#include <glad/glad.h>

#include <unordered_set>
#include <unordered_map>

#if defined(DEBUG) || defined(PROFILE)

static std::unordered_set<int> TimersStarted;
static std::unordered_map<int, std::pair<unsigned int, bool>> GPUQueries; 
static std::unordered_map<int, std::pair<double, double>> lastTimes;

static bool ShouldDisplay = false;

#endif

Timer::Timer(const char* name)
{
    #if defined(DEBUG) || defined(PROFILE)

    m_ID = Hash(name);
    m_Name = name;
    
    m_StartTime = std::chrono::high_resolution_clock::now();
    if(TimersStarted.find(m_ID) == TimersStarted.end()){
        glGenQueries(1, &GPUQueries[m_ID].first);
        glBeginQuery(GL_TIME_ELAPSED, GPUQueries[m_ID].first);
        TimersStarted.insert(m_ID);
        GPUQueries[m_ID].second = false;
    }

    #endif
}

Timer::~Timer()
{
}

double Timer::GetMillisecondsElapsed()
{
    #if defined(DEBUG) || defined(PROFILE)

    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(endTime - m_StartTime).count();

    #else

    return 0.0;

    #endif
}

void Timer::PrintTime()
{
    #if defined(DEBUG) || defined(PROFILE)

    if(!GPUQueries[m_ID].second){
        glEndQuery(GL_TIME_ELAPSED);
        GPUQueries[m_ID].second = true;
    }

    GLint gpuTimeAvailable = 0;
    glGetQueryObjectiv(GPUQueries[m_ID].first, GL_QUERY_RESULT_AVAILABLE, &gpuTimeAvailable);

    GLuint64 gpuTime;
    double cpuTime;

    if(gpuTimeAvailable){
        glGetQueryObjectui64v(GPUQueries[m_ID].first, GL_QUERY_RESULT, &gpuTime);
        cpuTime = GetMillisecondsElapsed();
        glDeleteQueries(1, &GPUQueries[m_ID].first);
        TimersStarted.erase(m_ID);

        lastTimes[m_ID] = {cpuTime, gpuTime / 1000000.0};
    }

    if(ShouldDisplay){
        if(gpuTimeAvailable){
            printf("CPU: %s took %f ms ID: %d\n", m_Name, cpuTime, m_ID);
            printf("GPU: %s took %f ms ID: %d\n", m_Name, gpuTime / 1000000.0, m_ID);
        }else{
            printf("CPU: %s took %f ms ID: %d\n", m_Name, lastTimes[m_ID].first, m_ID);
            printf("GPU: %s took %f ms ID: %d\n", m_Name, lastTimes[m_ID].second, m_ID);
        }
    }

    #endif
}

void ShouldDisplayTimers(bool shouldDisplay)
{
    #if defined(DEBUG) || defined(PROFILE)

    ShouldDisplay = shouldDisplay;

    #endif
}

int Timer::Hash(const char* str)
{
    int h = 0;

    for(int i = 0; str[i] != '\0'; i++){
        h = 31 * h + str[i];
    }

    return h;
}

void FreeRemainingTimers()
{
    #if defined(DEBUG) || defined(PROFILE)

    for(int id : TimersStarted){
        glDeleteQueries(1, &GPUQueries[id].first);
    }

    #endif
}