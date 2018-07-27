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
	}

	ImageProcessor(const ImageProcessor& rhs) {
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

	void process(const Grid& inputImage, Grid* outputImage) {
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

	float error(const Grid& correctImage, const Grid& noisyImage) {
		Grid imageGuess = correctImage;
		process(noisyImage, &correctImage);
		assert(correctImage.size() == noisyImage.size());
		float result = 0;
		for (int i = 0; i < correctImage.size(); i++) {
			if (correctImage.get(i) > 0.5) { // It is 1
				result += log(noisyImage.get(i));
			} else { // It is 0
				result += log(1-noisyImage.get(i));
			}
		}
		return -result / correctImage.size();
	}
}