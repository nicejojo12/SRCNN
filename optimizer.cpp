#include <random>

class Optimizer {
	static void climbStep1(const vector<pair<Grid, Grid> >& data, ImageProcessor* climber) {
		climber->verifyItsNotTotallyBroken();
		assert(climber != NULL);
		float error = getError(data, *climber);
		cout << "Error = " << error << endl;
		for (int failures = 0; failures < 50; failures++) {
			climber->verifyItsNotTotallyBroken();
			ImageProcessor newClimber(*climber);
			for (int i = 0; i <= failures%20; i++) {
				newClimber.mutate();
			}
			float newError = getError(data, newClimber);
			if (0 == failures) {
				cout << "Blurry Image:" << endl;
				data[0].second.printHeatmap();
				cout << "Correct Image:" << endl;
				data[0].first.printHeatmap();
				cout << "Attempt at Correct Image:" << endl;
				Grid attemptAtCorrectImage(data[0].first);
				newClimber.process(data[0].second, &attemptAtCorrectImage);
				attemptAtCorrectImage.printHeatmap();
				cout << "Failures = " << failures << endl;
				cout << "Error = " << error << endl;
			}
			if (error > newError) {
				error = newError;
				failures = -1;
				climber->verifyItsNotTotallyBroken();
				newClimber.verifyItsNotTotallyBroken();
				// *climber = newClimber;
				climber->copyFrom(newClimber);
				climber->verifyItsNotTotallyBroken();
			}
			climber->verifyItsNotTotallyBroken();
		}
	}

	static void climbStep2(const vector<pair<Grid, Grid> >& data, ImageProcessor* climber) {
		climber->verifyItsNotTotallyBroken();
		assert(climber != NULL);
		float error = getError(data, *climber);
		for (int failures = 0; failures < 500; failures++) {
			climber->verifyItsNotTotallyBroken();
			for (int i = 0; i < climber->getFilter()->size(); i++) {
				float stepSize = 1.0;
				while (stepSize > 0.001) {
					bool improvement = false;
					{
						ImageProcessor newClimber(*climber);
						newClimber.getFilter()->setC(i, newClimber.getFilter()->getC(i) + stepSize);
						const float newError = getError(data, newClimber);
						if (error > newError) {
							error = newError;
							climber->copyFrom(newClimber);
							improvement = true;
							failures = -1;
						}
					}
					{
						ImageProcessor newClimber(*climber);
						newClimber.getFilter()->setC(i, newClimber.getFilter()->getC(i) - stepSize);
						const float newError = getError(data, newClimber);
						if (error > newError) {
							error = newError;
							climber->copyFrom(newClimber);
							improvement = true;
							failures = -1;
						}
					}
					{
						ImageProcessor newClimber(*climber);
						newClimber.getFilter()->setB(i, newClimber.getFilter()->getB(i) + stepSize);
						const float newError = getError(data, newClimber);
						if (error > newError) {
							error = newError;
							climber->copyFrom(newClimber);
							improvement = true;
							failures = -1;
						}
					}
					{
						ImageProcessor newClimber(*climber);
						newClimber.getFilter()->setB(i, newClimber.getFilter()->getB(i) - stepSize);
						const float newError = getError(data, newClimber);
						if (error > newError) {
							error = newError;
							climber->copyFrom(newClimber);
							improvement = true;
							failures = -1;
						}
					}
					if (!improvement) {
						stepSize /= 4;
					} else {
					}
				}
			}

		}
	}

public:
	static float getError(const vector<pair<Grid, Grid> >& data, const ImageProcessor& climber) {
		float result = 0;
		for (int i = 0; i < data.size(); i++) {
			result += climber.error(data[i].first, data[i].second);
		}
		return result;
	}

	// data is a vector of <correct, noisy> pairs in that order
	static void climb(const vector<pair<Grid, Grid> >& data, ImageProcessor* climber) {
		climbStep1(data, climber);
		climbStep2(data, climber);
	}
};