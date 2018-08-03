#include <fstream>
#include <math.h>

const string imageProcessorFilename = "savedProcessor";

class ImageProcessor {
	Filter* mFilter;

public:
	ImageProcessor() {
		mFilter = new Filter(1, 13, 1);
	}

	Filter* getFilter() {
		return mFilter;
	}

	void save() {
		ofstream file;
		file.open(imageProcessorFilename);
		char line[sizeof(float)];
		for (int i = 0; i < mFilter->size(); i++) {
			const float coeff = mFilter->getC(i);
			memcpy(line, &coeff, sizeof(float));
			file.write(line, sizeof(float));

			const float bias = mFilter->getB(i);
			memcpy(line, &bias, sizeof(float));
			file.write(line, sizeof(float));
		}
		file.close();
	}

	void load() {
		ifstream file;
		file.open(imageProcessorFilename);
		char line[sizeof(float)];
		for (int i = 0; i < mFilter->size(); i++) {
			file.read(line, sizeof(float));
			float coeff = 0;
			memcpy(&coeff, line, sizeof(float));
			mFilter->setC(i, coeff);

			file.read(line, sizeof(float));
			float bias = 0;
			memcpy(&bias, line, sizeof(float));
			mFilter->setB(i, bias);
		}
		file.close();
	}

	void verifyItsNotTotallyBroken() {
		assert(mFilter->size());
	}

	ImageProcessor(const ImageProcessor& rhs) {
		assert(rhs.mFilter != NULL);
		assert(rhs.mFilter->size());
		mFilter = new Filter(*(rhs.mFilter));
	}

	// TODO: don't use copyFrom, this should work with a copy constructor and assignment operator
	void copyFrom(const ImageProcessor& rhs) {
		if (mFilter != NULL) delete mFilter;
		assert(rhs.mFilter != NULL);
		assert(rhs.mFilter->size());
		mFilter = new Filter(*(rhs.mFilter));
	}

	~ImageProcessor() {
		delete mFilter;
	}

	void mutate() {
		mFilter->mutate();
	}

	void process(const Grid& inputImage, Grid* outputImage) const {
		assert(1 == inputImage.getLayers());
		assert(1 == outputImage->getLayers());
		assert(12 + outputImage->getWidth() == inputImage.getWidth());
		assert(12 + outputImage->getHeight() == inputImage.getHeight());
		mFilter->apply(inputImage, outputImage);
		for (int i = 0; i < outputImage->size(); i++) {
			const float currentValue = outputImage->get(i);
			if (currentValue >= 0.999999) {
				outputImage->set(i, 0.999999);
			}
			if (currentValue <= 0.000001) {
				outputImage->set(i, 0.000001);
			}

		}
	}

	float error(const Grid& correctImage, const Grid& noisyImage) const {
		Grid imageGuess = correctImage;
		process(noisyImage, &imageGuess);
		assert(correctImage.size() == imageGuess.size());
		float result = 0;
		for (int i = 0; i < correctImage.size(); i++) {
			const float diff = correctImage.get(i) - imageGuess.get(i);
			result += diff * diff;
		}
		return result;
	}
};