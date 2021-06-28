#include "../common.h"

namespace capsuleGene
{
    class AlgebraUtils
    {
    public:
        static std::vector<double> generate_unit_vec(const int num, const int slot_size, const double value);

        /**
         * @brief sum for ctxt. please set num as negative if you want to sum to 0idx.
         * 
         * @param enc_vec 
         * @param num 
         * @return Ciphertext 
         */
        static Ciphertext rotate_and_sum_in_col(const Ciphertext &x, const int num, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<GaloisKeys> gal_keys, const std::shared_ptr<CKKSEncoder> encoder, const int slot_size, const double scale);

        /**
         * @brief multiply two ctxt
         * 
         * @param x 
         * @param y 
         * @param evaluator evaluator
         * @param rel_keys relin key
         * @return Ciphertext 
         */
        static Ciphertext multiply(const Ciphertext &x, const Ciphertext &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<RelinKeys> rel_keys);

        /**
         * @brief multiply ctxt and vector
         * 
         * @param x 
         * @param y 
         * @param evaluator 
         * @param rel_keys 
         * @param encoder 
         * @param scale 
         * @return Ciphertext 
         */
        static Ciphertext multiply(const Ciphertext &x, const std::vector<double> &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<RelinKeys> rel_keys, const std::shared_ptr<CKKSEncoder> encoder, double scale);

        /**
         * @brief add ctxt and vector
         * 
         * @param x 
         * @param y 
         * @param evaluator 
         * @param rel_keys 
         * @param encoder 
         * @param scale 
         * @return Ciphertext 
         */
        static Ciphertext add(Ciphertext &x, const std::vector<double> &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder, const double scale);
    };
}