#include <stdlib.h>

#include "basictypes.h"
#include "grid.h"

// Class that takes layers of Grids as inputs, then generates the next layer
// If the inputLayers is 1, then it takes the image as input
// If outputLayers is 1, then it is generating the final image
class Filter {
	const uint32 kOutputLayers, kSize, kInputLayers;
	float* mCoef;
	float* mBias;

	uint32 getIndex(
		const uint32 outputLayer,
		const uint32 col,
		const uint32 row,
		const uint32 inputLayer) const {
		assert(outputLayer < kOutputLayers);
		assert(col < kSize);
		assert(row < kSize);
		assert(inputLayer < kInputLayers);
		// It's kind of like a 4-digit number with a different base on every digit
		uint32 result = 0;
		result += kInputLayers * kSize * kSize * outputLayer;
		result += kInputLayers * kSize * row;
		result += kInputLayers * col;
		result += inputLayer;
		return result;
	} 

public:
	Filter()
		: kOutputLayers(0), kSize(0), kInputLayers(0) {
		assert(0);
	}

	Filter(const uint32 outputLayers, const uint32 windowSize, const uint32 inputLayers)
		: kOutputLayers(outputLayers), kSize(windowSize), kInputLayers(inputLayers) {
		mCoef = new float[size()];
		mBias = new float[size()];
		for (int i = 0; i < size(); i++) {
			mCoef[i] = Math::normalDistribution();
			mBias[i] = Math::normalDistribution()/100;
		}
	}

	Filter(const Filter& rhs)
		: kOutputLayers(rhs.kOutputLayers), kSize(rhs.kSize), kInputLayers(rhs.kInputLayers) {
		mCoef = new float[size()];
		mBias = new float[size()];
		*this = rhs;
	}

	Filter(const Grid& grid)
		: kOutputLayers(1), kSize(grid.getWidth()), kInputLayers(1) {
		const uint32 N = grid.getWidth();
		assert(N == grid.getHeight());
		mCoef = new float[size()];
		mBias = new float[size()];
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				mCoef[i*N+j] = grid.get(Location(i, j));
				mBias[i*N+j] = 0;
			}
		}
	}

	~Filter() {
		delete mCoef;
	}

	Filter& operator= (const Filter& rhs) {
		assert(kOutputLayers == rhs.kOutputLayers);
		assert(kSize == rhs.kSize);
		assert(kInputLayers == rhs.kInputLayers);
		for (uint32 i = 0; i < size(); i++) {
			setC(i, rhs.getC(i));
			setB(i, rhs.getB(i));
		}
		return *this;
	}

	uint32 size() const {
		return kOutputLayers * kSize * kSize * kInputLayers;
	}

	float getC(const uint32 i) const {
		assert(i < size());
		return mCoef[i];
	}

	float getB(const uint32 i) const {
		assert(i < size());
		return mBias[i];
	}

	void setC(const uint32 i, const float value) {
		assert(i < size());
		mCoef[i] = value;
	}

	void setB(const uint32 i, const float value) {
		assert(i < size());
		mBias[i] = value;
	}

	void mutate() {
		if (0 == rand() % 2) {
			mCoef[rand()%size()] += Math::normalDistribution();
		} else {
			mBias[rand()%size()] += Math::normalDistribution()/100;
		}
	}

	// For a specific pixel of the output, do the convolution of all the things that go
	// into that pixel using the input
	void setPixel(
		const Grid& input,
		const uint32 layer,
		const uint32 col,
		const uint32 row,
		Grid* output) const {

		float result = 0;
		for (uint32 srcCol = col; srcCol < col + kSize; srcCol++) {
			for (uint32 srcRow = row; srcRow < row + kSize; srcRow++) {
				for (uint32 srcLayer = 0; srcLayer < input.getLayers(); srcLayer++) {
					const float srcCoef = input.get(Location(srcCol, srcRow, srcLayer));
					const uint32 index = getIndex(
						layer, srcCol - col, srcRow - row, srcLayer);
					const float kernelValue = getC(index);
					const float bias = getB(index);
					result += srcCoef * kernelValue + bias;
				}
			}
		}
		output->set(Location(col, row, layer), result);
	}

	// Loop over every pixel of the output and assign the correct propagated value
	void apply(const Grid& input, Grid* output) const {
		assert(kInputLayers == input.getLayers());
		assert(kOutputLayers == output->getLayers());
		assert(input.getWidth() - (kSize-1) == output->getWidth());
		assert(input.getHeight() - (kSize-1) == output->getHeight());
		for (uint32 outputLayer = 0; outputLayer < kOutputLayers; outputLayer++) {
			for (uint32 outputCol = 0; outputCol < output->getHeight(); outputCol++) {
				for (uint32 outputRow = 0; outputRow < output->getWidth(); outputRow++) {
					setPixel(input, outputLayer, outputCol, outputRow, output);
				}
			}
		}
	}
};