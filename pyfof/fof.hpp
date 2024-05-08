#pragma once
#include <cstdlib>
#include <vector>

typedef std::size_t size_t;

std::vector< std::vector<size_t> >  friends_of_friends(double* data, size_t npts, size_t ndim, double linking_length);
