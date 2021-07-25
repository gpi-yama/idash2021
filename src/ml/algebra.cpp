#include "algebra.h"

namespace capsuleGene
{
    Ciphertext AlgebraUtils::multiply(const Ciphertext &x, const Ciphertext &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<RelinKeys> rel_keys)
    {
        Ciphertext mult;
        evaluator->multiply(x, y, mult);
        evaluator->relinearize_inplace(mult, *rel_keys.get());
        evaluator->rescale_to_next_inplace(mult);
        return mult;
    }

    Ciphertext AlgebraUtils::multiply(const Ciphertext &x, const std::vector<double> &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<RelinKeys> rel_keys, const std::shared_ptr<CKKSEncoder> encoder, double scale)
    {
        Ciphertext mult;
        Plaintext plain;
        encoder->encode(y, scale, plain);
        evaluator->multiply_plain(x, plain, mult);
        evaluator->relinearize_inplace(mult, *rel_keys.get());
        evaluator->rescale_to_next_inplace(mult);
        return mult;
    }

    Ciphertext AlgebraUtils::rotate_and_sum_in_col(const Ciphertext &x, const int num, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<GaloisKeys> gal_keys)
    {
        Ciphertext rotated, ctxt = x;
        Plaintext plain;
        int nIter = int(log2(num) + 1);
        for (int i = 0; i < nIter; i++)
        {
            evaluator->rotate_vector(ctxt, pow(2, i), *gal_keys.get(), rotated);
            evaluator->add(ctxt, rotated, ctxt);
        };
        return ctxt;
    }

    std::vector<double> AlgebraUtils::generate_unit_vec(const int num, const int slot_size, const double value)
    {
        std::vector<double> ones(slot_size, 0.0);
#pragma omp parallel for
        for (int i = 0; i < num; i++)
        {
            ones[i] = value;
        };
        return ones;
    }

    Ciphertext AlgebraUtils::add(Ciphertext &x, const std::vector<double> &y, const std::shared_ptr<Evaluator> evaluator, const std::shared_ptr<CKKSEncoder> encoder)
    {
        Ciphertext added;
        Plaintext plain;
        int scale_n = int(round(log2(x.scale())));
        x.scale() = pow(2.0, scale_n);
        encoder->encode(y, pow(2.0, scale_n), plain);
        evaluator->mod_switch_to_inplace(plain, x.parms_id());
        evaluator->add_plain(x, plain, added);
        return added;
    }
}