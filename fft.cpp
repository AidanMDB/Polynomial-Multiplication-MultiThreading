#include<vector>
#include<cstddef>   // required for size_t
#include<math.h>
#include<complex>

using power = size_t;
using namespace std::complex_literals;

    // x        is the input vector 
    // N        is the size of the vector (in factors of 2)
    // stride   is the 
std::vector<std::complex<double>> fft(std::vector<std::complex<double>> x, double N, int stride) {
    if (N == 1) {
        return x;
    }
    else {
        // calcaulates the even and odd partitions
        std::vector<std::complex<double>> x_odd; 
        std::vector<std::complex<double>> x_even; 
        for (int index = 0; index < x.size(); index++) {
            if (index % 2 == 0) {
                x_even.push_back(x[index]);
            }
            else {
                x_odd.push_back(x[index]);
            }
        }


        x_even =  fft(x_even, N / 2, 2 * stride);
        x_odd = fft(x_odd, N / 2, 2 * stride);
        for (double k = 0; k < ((N/2) - 1); k++) {
            std::complex<double> p = x[k];
            std::complex<double> q = std::exp((-2.0i * k * M_PI) / N);
            x[k] = p + q;
            x[k + (N / 2)] = p - q;
        }
        return x;
    }
}

    // poly1 and poly2 need to have already been converted to the form described below
    // For example, if your polynomial is P(x) = 3x^2 + 0x + 5 you would represent it as:  x=[3,0,5].
std::vector<std::complex<double>> multiply_polynomials(std::vector<std::complex<double>> poly1, std::vector<std::complex<double>> pol2) {
    
    // Pad the vectors so their sizes are factors of 2, & the vectors need to be the same size

    // compute fft's
    fft_result1 = fft(pol1)
    fft_result2 = fft(pol2)

    // element wise multiplication in frequency domain
    product
    for (int i = 0; i < fft_result.size(); i++) {
        product [i] = fft_result1[i] * fft_result2[i];
    }

    // compute inverse fft of product (conjugate of the twiddle function)
    return inverse_fft(product);

}