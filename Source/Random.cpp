#include <Random.hpp>
#include <random>

uint64_t RandUint64()
{
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    return gen();
}

uint64_t RandUint64(uint64_t min, uint64_t max)
{
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(min, max);
    return dist(gen);
}

uint32_t RandUint32()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen();
}

uint32_t RandUint32(uint32_t min, uint32_t max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(gen);
}

double RandDouble()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

double RandDouble(double min, double max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

float RandFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen);
}

float RandFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int RandInt()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    return dist(gen);
}

int RandInt(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}