#include "../common.h"

namespace capsuleGene
{
    class AlgebraUtils
    {
    private:
        static void test();
        static std::vector<double> generate_unit_vec(int num, int slot_num, double scale);

    public:
        /**
         * @brief sum for ctxt. please set num as negative if you want to sum to 0idx.
         * 
         * @param enc_vec 
         * @param num 
         * @return Ciphertext 
         */
        static Ciphertext rotate_and_sum_in_col(Ciphertext &x, int num, std::shared_ptr<Evaluator> eval, std::shared_ptr<GaloisKeys> gal_key, std::shared_ptr<CKKSEncoder> encoder, int slot_size, double scale);

        /**
         * @brief multiply two ctxt
         * 
         * @param x 
         * @param y 
         * @param eval evaluator
         * @param relKey relin key
         * @return Ciphertext 
         */
        static Ciphertext multiply(Ciphertext &x, Ciphertext &y, std::shared_ptr<Evaluator> eval, std::shared_ptr<RelinKeys> relKey);

        /**
         * @brief multiply ctxt and vector
         * 
         * @param x 
         * @param y 
         * @param evaluator 
         * @param relKey 
         * @param encoder 
         * @param scale 
         * @return Ciphertext 
         */
        static Ciphertext multiply(Ciphertext &x, std::vector<double> &y, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<RelinKeys> relKey, std::shared_ptr<CKKSEncoder> encoder, double scale);
    };
}