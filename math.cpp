#include <math.h>
#include <time.h>
#include <random>
#include <stdlib.h>

#include "basictypes.h"

using namespace std;

class Math {
	static default_random_engine* sGenerator;
public:
	static default_random_engine* getGenerator() {
		if (NULL == sGenerator) {
			sGenerator = new default_random_engine();
		}
		return sGenerator;
	}

	static double normalDistribution() {
		normal_distribution<double> distribution(0.0,1.0);
		return distribution(*(getGenerator()));
	}

	static uint64 poisson(const double mean) {
		// default_random_engine generator;
  		poisson_distribution<uint64> distribution(mean);
		const uint64 result = distribution(*(getGenerator()));
		return result;
	}

	static double normalPDF(const double z) {
		return exp(-z * z / 2) / sqrt(2 * M_PI);
	}
};

default_random_engine* Math::sGenerator = NULL;