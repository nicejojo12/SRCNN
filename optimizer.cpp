#include <random>

class Optimizer {
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
		climber->verifyItsNotTotallyBroken();
		assert(climber != NULL);
		float error = getError(data, *climber);
		for (int failures = 0; failures < 500; failures++) {
			cout << "Failures = " << failures << endl;
			cout << "Error = " << error << endl;
			climber->verifyItsNotTotallyBroken();
			ImageProcessor newClimber(*climber);
			for (int i = 0; i <= failures%20; i++) {
				newClimber.mutate();
			}
			float newError = getError(data, newClimber);
			if (0 == failures) {
				cout << "Correct Image:" << endl;
				data[0].first.printHeatmap();
				cout << "Attempt at Correct Image:" << endl;
				Grid attemptAtCorrectImage(data[0].first);
				newClimber.process(data[0].second, &attemptAtCorrectImage);
				attemptAtCorrectImage.printHeatmap();
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
};