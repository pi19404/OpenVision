#include "Convolution.hpp"


template <typename T> Convolution<T> ::Convolution()
{
    _separable=true;
    _boundaryMode=Convolution::BORDER_CONSTANT;
}
