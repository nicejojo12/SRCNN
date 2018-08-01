#include <math.h>

class ImageProcessor {
	Filter* mInput;
	Filter* mFirst;
	Filter* mSecond;

public:
	ImageProcessor() {
		mInput = new Filter(64, 9, 1);
		mFirst = new Filter(32, 1, 64);
		mSecond = new Filter(1, 5, 32);
		assert(mInput->size());
		assert(mFirst->size());
		assert(mSecond->size());
	}

	void verifyItsNotTotallyBroken() {
		assert(mInput->size());
		assert(mFirst->size());
		assert(mSecond->size());
	}

	ImageProcessor(const ImageProcessor& rhs) {
		assert(rhs.mInput != NULL);
		assert(rhs.mFirst != NULL);
		assert(rhs.mSecond != NULL);
		assert(rhs.mInput->size());
		assert(rhs.mFirst->size());
		assert(rhs.mSecond->size());
		mInput = new Filter(*(rhs.mInput));
		mFirst = new Filter(*(rhs.mFirst));
		mSecond = new Filter(*(rhs.mSecond));
	}

	// TODO: don't use copyFrom, this should work with a copy constructor and assignment operator
	void copyFrom(const ImageProcessor& rhs) {
		if (mInput == NULL) delete mInput;
		if (mFirst == NULL) delete mFirst;
		if (mSecond == NULL) delete mSecond;
		assert(rhs.mInput != NULL);
		assert(rhs.mFirst != NULL);
		assert(rhs.mSecond != NULL);
		assert(rhs.mInput->size());
		assert(rhs.mFirst->size());
		assert(rhs.mSecond->size());
		mInput = new Filter(*(rhs.mInput));
		mFirst = new Filter(*(rhs.mFirst));
		mSecond = new Filter(*(rhs.mSecond));
	}

	~ImageProcessor() {
		delete mInput;
		delete mFirst;
		delete mSecond;
	}

	void mutate() {
		switch (rand() % 3) {
			case 0: mInput->mutate(); break;
			case 1: mFirst->mutate(); break;
			case 2: mSecond->mutate(); break;
		}
	}

	void process(const Grid& inputImage, Grid* outputImage) const {
		assert(1 == inputImage.getLayers());
		assert(1 == outputImage->getLayers());
		assert(12 + outputImage->getWidth() == inputImage.getWidth());
		assert(12 + outputImage->getHeight() == inputImage.getHeight());
		Grid firstGrid(inputImage.getWidth() - 8, inputImage.getHeight() - 8, 64);
		mInput->apply(inputImage, &firstGrid);
		firstGrid.floor();
		Grid secondGrid(inputImage.getWidth() - 8, inputImage.getHeight() - 8, 32);
		mFirst->apply(firstGrid, &secondGrid);
		secondGrid.floor();
		mSecond->apply(secondGrid, outputImage);
		for (int i = 0; i < outputImage->size(); i++) {
			outputImage->set(i, .0000005 + .999999/(1+exp(-outputImage->get(i))));
		}
	}

	float error(const Grid& correctImage, const Grid& noisyImage) const {
		Grid imageGuess = correctImage;
		process(noisyImage, &imageGuess);
		assert(correctImage.size() == imageGuess.size());
		float result = 0;
		for (int i = 0; i < correctImage.size(); i++) {
			result +=      correctImage.get(i)  * log(    imageGuess.get(i));
			result += (1 - correctImage.get(i)) * log(1 - imageGuess.get(i));
		}
		return -result / correctImage.size();
	}
};