#include <chrono>

class Timer
{
    private:
        std::chrono::system_clock::time_point start;

    public:
        Timer(){
            start = std::chrono::system_clock::now();
        };

        Timer(std::chrono::system_clock::time_point start){
            this->start = start;
        };

        double end()
        {
            std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
            double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            return elapsed;
        };

        void reset() 
        {
            start = std::chrono::system_clock::now();
        }

        std::chrono::system_clock::time_point get_start_time() {
            return this->start;
        }
        
};