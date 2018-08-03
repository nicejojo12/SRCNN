#include <iostream>
#include "datagenerator.h"
#include "imageprocessor.h"
#include "optimizer.h"

using namespace std;

int main(int argc, char** argv) {
	const int numberOfTrainingExamples = 10;

	DataGenerator dataGenerator;
	dataGenerator.setError(0.000000001);			// Max weight Gaussian kernel ignores
	dataGenerator.setEnergyBandCoefficient(.02);		// Coefficient of y in f1
	dataGenerator.setSelfEnergyCoefficient(.7);	// Coefficient of y^2 in f2
	dataGenerator.setMinEnergy(3);					// Lower bound of y
	dataGenerator.setMaxEnergy(5);					// Upper bound of y
	dataGenerator.setMinMomentum(3);				// 
	dataGenerator.setMaxMomentum(5);
	
	// Because the ideal image is normalized to a 0-1 range, everything is on a 0-1 range
	// So, lambda is the noise to signal ratio
	// For example, lambda of .1 is a 1:10 noise to signal ratio
	dataGenerator.setLambda(.02);
	dataGenerator.setSigmaX(.1);
	dataGenerator.setSigmaY(.1);
	dataGenerator.setWidth(50);
	dataGenerator.setHeight(50);

	vector <pair<Grid, Grid> > trainingExamples;
	for (int examples = 0; examples < numberOfTrainingExamples; examples++) {

		pair<Grid, Grid> trainingExample = dataGenerator.createTrainingExample();
		trainingExamples.push_back(trainingExample);

		// if (0 == examples) {
		// 	cout << "Ideal image:" << endl;
		//  trainingExample.first.printHeatmap();
		// }

		// cout << "Blurred image:" << endl;
		// trainingExample.second.printHeatmap();

	}

	ImageProcessor processor;
	if (argc > 1) {
		processor.load();
	}
	processor.verifyItsNotTotallyBroken();

	Optimizer::climb(trainingExamples, &processor);


	return 0;
}
