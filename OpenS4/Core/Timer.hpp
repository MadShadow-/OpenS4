#pragma once
#include <chrono>

namespace OpenS4::Core
{
    /// @brief A timer to time things. Currently only used for benchmarks but
    /// could be extended.
    class Timer
    {
       private:
        using clock_t = std::chrono::high_resolution_clock;
        using second_t = std::chrono::duration<double, std::ratio<1>>;

        /// @brief holding the value of object creation or last reset
        std::chrono::time_point<clock_t> m_timestamp{clock_t::now()};

       public:
        Timer() = default;

        /// @brief resets timestamp to now time
        void reset() { m_timestamp = clock_t::now(); }

        /// @return number of seconds since the timer was started
        double seconds()
        {
            return std::chrono::duration_cast<second_t>(clock_t::now() -
                                                        m_timestamp)
                .count();
        }
    };

}  // namespace OpenS4::Core
