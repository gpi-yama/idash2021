#include "algebra.h"

namespace capsuleGene
{
    Ciphertext AlgebraUtils::multiply(Ciphertext &x, Ciphertext &y, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<RelinKeys> rel_keys)
    {
        Ciphertext mult;
        evaluator->multiply(x, y, mult);
        evaluator->relinearize_inplace(mult, *rel_keys.get());
        evaluator->rescale_to_next_inplace(mult);
        return mult;
    }

    Ciphertext AlgebraUtils::multiply(Ciphertext &x, std::vector<double> &y, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<RelinKeys> rel_keys, std::shared_ptr<CKKSEncoder> encoder, double scale)
    {
        Ciphertext mult;
        Plaintext plain;
        encoder->encode(y, scale, plain);
        evaluator->multiply_plain(x, plain, mult);
        evaluator->relinearize_inplace(mult, *rel_keys.get());
        evaluator->rescale_to_next_inplace(mult);
        return mult;
    }

    Ciphertext AlgebraUtils::rotate_and_sum_in_col(Ciphertext &x, int num, std::shared_ptr<Evaluator> evaluator, std::shared_ptr<GaloisKeys> gal_keys, std::shared_ptr<CKKSEncoder> encoder, int slot_size, double scale)
    {
        Ciphertext ctxt;
        Plaintext plain;
        int nIter = int(log2(num) + 2);
        ctxt = x;
        for (int i = 0; i < nIter; i++)
        {
            evaluator->rotate_vector(x, -pow(2, i), *gal_keys.get(), ctxt);
            evaluator->add_inplace(ctxt, ctxt);
        };
        return ctxt;
    }

    std::vector<double> AlgebraUtils::generate_unit_vec(int num, int slot_size, double scale)
    {
        std::vector<double> ones(slot_size, 0.0);
#pragma omp parallel for
        for (int i = 0; i < num; i++)
        {
            ones[i] = 1.0;
        };
        return ones;
    }
}