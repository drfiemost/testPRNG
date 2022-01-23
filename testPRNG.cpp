#include <cstdint>
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>
#include <string>

// generate numbers in the range [0, MAX_VALUE)
constexpr int MAX_VALUE = 100;

// https://www.learncpp.com/cpp-tutorial/timing-your-code/
class Timer
{
private:
    // Type aliases to make accessing nested type easier
    using clock_type = std::chrono::steady_clock;
    using second_type = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_type> m_beg { clock_type::now() };

public:
    void reset()
    {
        m_beg = clock_type::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_type>(clock_type::now() - m_beg).count();
    }
};

// base class
class randomGenerator {
public:
    virtual void seed(uint32_t s) = 0;
    virtual int random(int max) = 0;
};

// https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/
class lemire : public randomGenerator {
    
private:
    uint16_t wyhash16_x; 

    uint32_t hash16(uint32_t input, uint32_t key) {
        uint32_t hash = input * key;
        return ((hash >> 16) ^ hash) & 0xFFFF;
    }

    uint16_t wyhash16() {
        wyhash16_x += 0xfc15;
        return hash16(wyhash16_x, 0x2ab);
    }

    uint16_t rand_range16(const uint16_t s) {
        uint16_t x = wyhash16();
        uint32_t m = (uint32_t)x * (uint32_t)s;
        uint16_t l = (uint16_t)m;
        if (l < s) {
            // NOTE the original version is being optimized as 'xor eax, eax'
            //      by the compiler due to integer promotion so add a cast
            //uint16_t t = -s % s;
            uint16_t t = static_cast<uint16_t>(-s) % s;
            while (l < t) {
                x = wyhash16();
                m = (uint32_t)x * (uint32_t)s;
                l = (uint16_t)m;
            }
        }
        return m >> 16;
    }

public:
    void seed(uint32_t s) override { wyhash16_x = static_cast<uint16_t>(s); }

    int random(int max) override { return rand_range16(max); }
};

// 16 bit xorshift
class xorshift : public randomGenerator {
    
private:
    uint16_t x;

    uint16_t xorshift16()
    {
        x ^= x << 7;
        x ^= x >> 9;
        x ^= x << 8;
        return x;
    }

public:
    void seed(uint32_t s) override { x = static_cast<uint16_t>(s); }

    int random(int max) override { return (xorshift16() - 1) % max; }
};

// Linear congruential generator
class lcg : public randomGenerator {
    
private:
    uint32_t fast_rand_seed_val;

    // fast_rand routine returns an integer in range 0..32767
    int fast_rand() {
        fast_rand_seed_val = (214013 * fast_rand_seed_val + 2531011);
        return static_cast<int>((fast_rand_seed_val >> 16) & 0x7FFF);
    }

public:
    void seed(uint32_t s) override { fast_rand_seed_val = s; }

    int random(int max) override { return fast_rand() % max; }
};

// C random generator
class standard : public randomGenerator {
    
public:
    void seed(uint32_t s) override { srand(s); }

    int random(int max) override { return rand() % max; }
};

// https://stackoverflow.com/questions/11758809/what-is-the-optimal-algorithm-for-generating-an-unbiased-random-integer-within-a
void Distribution(randomGenerator* rand, std::string name) {

    std::array<int, MAX_VALUE> buckets;
    buckets.fill(0);

    rand->seed(0x12345678);

    Timer t;
    for (int i=0; i<10000000; i++) {
        int rnd = rand->random(MAX_VALUE);
        buckets[rnd]++;
    }
    std::cout << name << ": time elapsed: " << t.elapsed() << " seconds" << std::endl;

    std::ofstream outfile;
    outfile.open (name+"_distribution.csv");
    for (int i = 0; i < MAX_VALUE; i++)
        outfile << i << "," << buckets[i] << std::endl;
    outfile.close();
}

void Dump(randomGenerator* rand, std::string name) {

    rand->seed(0x87654321);

    std::ofstream outfile;
    outfile.open (name+"_dump.csv");
    for (int i=0; i<1000; i++) {
        int rnd = rand->random(MAX_VALUE);
        outfile << rnd << std::endl;
    }
    outfile.close();
}

int main( int argc, char* argv[]) {

    lemire r1;
    xorshift r2;
    lcg r3;
    standard r4;

    Distribution(&r1, "Lemire");
    Distribution(&r2, "xorshift16");
    Distribution(&r3, "LCG");
    Distribution(&r4, "standard");

    Dump(&r1, "Lemire");
    Dump(&r2, "xorshift16");
    Dump(&r3, "LCG");
    Dump(&r4, "standard");
} 
