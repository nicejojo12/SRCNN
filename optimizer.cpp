#include <random>

class Optimizer {
	static const kWidth = 200;
	static const kHeight = 100;

public:
	static float error(const vector<pair<Grid, Grid> >& data, const ImageProcessor& climber) {
		float result = 0;
		for (int i = 0; i < data.size(); i++) {
			result += climber.error(data[i].first, data[i].second);
		}
		return result;
	}

	// data is a vector of <correct, noisy> pairs in that order
	static climb(const vector<pair<Grid, Grid> >& data, ImageProcessor* climber) {
		float error = error(data, *climber);
		for (int failures = 0; failures < 500; failures++) {
			ImageProcessor newClimber(*climber);
			newClimber.mutate();
			newClimber.mutate();
			newClimber.mutate();
			newClimber.mutate();
			newClimber.mutate();
			newClimber.mutate();
			newClimber.mutate();
			float newError = error(data, newClimber);
			if (error > newError) {
				error = newError;
				failures = 0;
				*climber = newClimber;
			}
		}
	}
};