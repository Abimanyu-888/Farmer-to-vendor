#include "helper.hpp" // Include the header to ensure consistency

#include <random>
#include <array>
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>


/**
 * @brief Generates a random, high-performance Version 4 UUID.
 *
 * This function is optimized for speed and an extremely low probability of
 * collision, making it ideal for generating unique product IDs in a
 * high-throughput system.
 *
 * It uses a thread_local Mersenne Twister engine (`std::mt19937`), which means
 * each thread gets its own independent random number generator. The generator
 * is seeded only once per thread using std::random_device to ensure a
 * high-quality, non-deterministic starting point. Subsequent calls within the
 * same thread are extremely fast as they use the already-seeded PRNG.
 *
 * @return A string formatted as a standard v4 UUID, e.g.,
 * "123e4567-e89b-42d3-a456-556642440000".
 */
std::string generate_product_id() {
    // `thread_local` ensures that each thread has its own separate instance
    // of the generator. It's initialized only once per thread, making it
    // both thread-safe and highly performant.
    thread_local std::mt19937 generator = []{
        // Use std::random_device to get a seed with high entropy.
        std::random_device rd;

        // The Mersenne Twister engine has a large internal state. To seed it
        // thoroughly, we fill an array with random numbers from the device
        // and use a seed_seq to initialize the engine.
        std::array<int, std::mt19937::state_size> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());

        // Return the fully-seeded Mersenne Twister engine.
        return std::mt19937(seq);
    }();

    // The character set includes uppercase, lowercase, and numbers.
    const std::string charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::uniform_int_distribution<int> dist(0, charset.length() - 1);

    std::string id;
    id.reserve(8);
    for (int i = 0; i < 8; ++i) {
        id += charset[dist(generator)];
    }

    return id;
}