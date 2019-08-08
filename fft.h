#ifndef FFT_H
#define FFT_H

#include <complex>
#include <valarray>

namespace fft {

typedef std::complex<float> Complex;
typedef std::valarray<Complex> ComplexArray;

void fft(ComplexArray& x) {
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    ComplexArray even = x[std::slice(0, N/2, 2)];
    ComplexArray  odd = x[std::slice(1, N/2, 2)];

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar<float>(1.0, -2 * M_PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

void ifft(ComplexArray& x) {
    x = x.apply(std::conj);
    fft( x );
    x = x.apply(std::conj);
    x /= x.size();
}

}

#endif

