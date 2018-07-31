#include "basictypes.h"
#include "filter.h"
#include "grid.h"

class DataGenerator {
	// These are the adjustable functions/parameters

	double mError;
	double mEnergyBandCoefficient; // This was alpha on the whiteboard
	double mSelfEnergyCoefficient; // This was beta on the whiteboard
	double mMinEnergy;
	double mMaxEnergy;
	double mMinMomentum;
	double mMaxMomentum;
	double mLambda;
	double mSigmaX;
	double mSigmaY;
	uint32 mHeight;
	uint32 mWidth;

	// This was g(x) on the whiteboard notes
	double generate(const double momentum) {
		return momentum;
	}

	// This was f1(y)
	double getEnergyBand(const double energy) {
		return mEnergyBandCoefficient * energy;
	}

	// This was f2(y)
	double getSelfEnergy(const double energy) {
		return mSelfEnergyCoefficient * energy * energy;
	}

	// This is the one before we noise it a lot
	double getIdealIntensity(const int x, const int y) {
		const double energy = mMinEnergy + (mMaxEnergy - mMinEnergy) * double(y)/double(mHeight);
		const double momentum =
			mMinMomentum + (mMaxMomentum - mMinMomentum) * double(x)/double(mWidth);
		const double generator = generate(momentum);
		const double energyBand = getEnergyBand(energy);
		const double selfEnergy = getSelfEnergy(energy);
		const double dTerm1 = (double(y) - energyBand - generator);
		const double denominator = dTerm1 * dTerm1 + selfEnergy * selfEnergy;

		if (denominator * 100000 < selfEnergy) {
			cout << "Houston, we have a problem" << endl;
			cout << "Our current energy band includes 0 energy" << endl;
			cout << "Try making mMinEnergy more" << endl;
			cout << "Or, try making mError more (so you don't have to look so far outside)" << endl;
			assert(0);
		}

		return selfEnergy / denominator;
	}

	// Determine the size of the gaussian kernel used to add noise
	uint32 getGaussianGridSize() const {
		const double zChangePerX = (mMaxMomentum - mMinMomentum) / mWidth / mSigmaX;
		const double zChangePerY = (mMaxEnergy - mMinEnergy) / mHeight / mSigmaY;
		const double zChange = min(zChangePerX, zChangePerY);
		uint32 stepsNeeded = 0;
		while (mError < Math::normalPDF(double(stepsNeeded) * zChange)) {
			stepsNeeded++;
		}
		return 1 + stepsNeeded * 2;
	}

	Grid createGaussianNoise(const int gaussianGridSize) {
		Grid gaussianNoise(gaussianGridSize, gaussianGridSize);
		const int median = gaussianGridSize / 2;
		for (int x = 0; x < gaussianGridSize; x++) {
			for (int y = 0; y < gaussianGridSize; y++) {
				const double zFromX = (mMaxMomentum - mMinMomentum)
					* double(x - median) / double(mWidth) / mSigmaX;
				const double zFromY = (mMaxEnergy - mMinEnergy)
					* double(y - median) / double(mHeight) / mSigmaY;
				const double z = sqrt(zFromX * zFromX + zFromY * zFromY);
				gaussianNoise.set(Location(x, y), Math::normalPDF(z));
			}
		}
		return gaussianNoise;
	}

	Grid createIdealImage(const int padding) {
		Grid paddedIdealImage(mWidth + 2 * padding, mHeight + 2 * padding);
		for (int i = 0; i < paddedIdealImage.getWidth(); i++) {
			int x = i - padding;
			for (int j = 0; j < paddedIdealImage.getHeight(); j++) {
				int y = j - padding;
				paddedIdealImage.set(Location(i, j), getIdealIntensity(x, y));
			}
		}
		return paddedIdealImage;
	}

	void normalizeGrid(const double lower, const double upper, Grid* grid) {
		for (int i = 0; i < grid->getWidth(); i++) {
			for (int j = 0; j < grid->getHeight(); j++) {
				Location loc(i, j);
				const float unnormalizedValue = grid->get(loc);
				const float normalizedValue = (unnormalizedValue - lower) / (upper - lower);
				grid->set(loc, normalizedValue);

			}
		}
	}

public:
	// This is the main function for generating training data
	// The idealOutput will be the same every run (unless you change parameters)
	// The input will be blurred and 12 more pixels in each direction
	pair<Grid, Grid> createTrainingExample() {
		// Returns <input, idealOutput>

		// Figure out how large to make the kernal for the gaussian noise
		// In theory, this should be infinity by infinity
		// However, as an optimization, just use the smallest square with nothing above mError
		const int gaussianGridSize = getGaussianGridSize();

		// Make a larger grid than needed because the gaussian noise is going to make it smaller
		Grid enlargedGrid = createIdealImage(gaussianGridSize / 2);

		// Because intensities can be basically anything, normalize them to the 0-to-1 range
		const double lower = enlargedGrid.getMin();
		const double upper = enlargedGrid.getMax();
		normalizeGrid(lower, upper, &enlargedGrid);

		// Add poisson noise, generating a new noise number for each pixel
		Grid noisyEnlargedGrid(enlargedGrid);
		for (int i = 0; i < noisyEnlargedGrid.getWidth(); i++) {
			for (int j = 0; j < noisyEnlargedGrid.getHeight(); j++) {
				const Location loc(i, j);
				noisyEnlargedGrid.set(loc, noisyEnlargedGrid.get(loc) + Math::poisson(mLambda));
			}
		}

		// Spread it out with a gaussian
		const Filter gaussianFilter(createGaussianNoise(gaussianGridSize));
		Grid noisyImage(mWidth, mHeight);
		gaussianFilter.apply(noisyEnlargedGrid, &noisyImage);

		// Make a reduced version of the ideal image, since the noisy one is also reduces
		// TODO: -6 is kind of a magic number (lose 4 for the 9x9, lose 2 for the 5x5)
		//	It might be good to define some constants for this
		Grid idealImage = createIdealImage(-6);
		normalizeGrid(lower, upper, &idealImage);

		return make_pair(noisyImage, idealImage);
	}

	double getError() const {
		return mError;
	}

	void setError(const double error) {
		mError = error;
	}

	double getEnergyBandCoefficient() const {
		return mEnergyBandCoefficient;
	}

	void setEnergyBandCoefficient(const double energyBandCoefficient) {
		mEnergyBandCoefficient = energyBandCoefficient;
	}

	double getSelfEnergyCoefficient() const {
		return mSelfEnergyCoefficient;
	}

	void setSelfEnergyCoefficient(const double selfEnergyCoefficient) {
		mSelfEnergyCoefficient = selfEnergyCoefficient;
	}

	double getMinEnergy() const {
		return mMinEnergy;
	}

	void setMinEnergy(const double minEnergy) {
		mMinEnergy = minEnergy;
	}

	double getMaxEnergy() const {
		return mMaxEnergy;
	}

	void setMaxEnergy(const double maxEnergy) {
		mMaxEnergy = maxEnergy;
	}

	double getMinMomentum() const {
		return mMinMomentum;
	}

	void setMinMomentum(const double minMomentum) {
		mMinMomentum = minMomentum;
	}

	double getMaxMomentum() const {
		return mMaxMomentum;
	}

	void setMaxMomentum(const double maxMomentum) {
		mMaxMomentum = maxMomentum;
	}

	double getLambda() const {
		return mLambda;
	}

	void setLambda(const double lambda) {
		mLambda = lambda;
	}

	double getSigmaX() const {
		return mSigmaX;
	}

	void setSigmaX(const double sigmaX) {
		mSigmaX = sigmaX;
	}

	double getSigmaY() const {
		return mSigmaY;
	}

	void setSigmaY(const double sigmaY) {
		mSigmaY = sigmaY;
	}

	uint32 getHeight() const {
		return mHeight;
	}

	void setHeight(const double height) {
		mHeight = height;
	}

	uint32 getWidth() const {
		return mWidth;
	}

	void setWidth(const double width) {
		mWidth = width;
	}

};