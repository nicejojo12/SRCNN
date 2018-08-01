#include <iostream>
#include "datagenerator.h"
#include "imageprocessor.h"
#include "optimizer.h"

// The really stupid one: Error = 28.871 after maybe 20 minutes???

using namespace std;

int main(int argc, char** argv) {

	vector <pair<Grid, Grid> > trainingExamples;
	for (int examples = 0; examples < 10; examples++) {
		DataGenerator dataGenerator;
		dataGenerator.setError(0.000000001);			// Max weight Gaussian kernel ignores
		dataGenerator.setEnergyBandCoefficient(.2);		// Coefficient of y in f1
		dataGenerator.setSelfEnergyCoefficient(.07);	// Coefficient of y^2 in f2
		dataGenerator.setMinEnergy(10);					// Lower bound of y
		dataGenerator.setMaxEnergy(11);					// Upper bound of y
		dataGenerator.setMinMomentum(10);				// 
		dataGenerator.setMaxMomentum(11);
		
		// Because the ideal image is normalized to a 0-1 range, everything is on a 0-1 range
		// So, lambda is the noise to signal ratio
		// For example, lambda of .1 is a 10:1 signal to noise ratio
		dataGenerator.setLambda(.1);
		dataGenerator.setSigmaX(.02);
		dataGenerator.setSigmaY(.03);
		dataGenerator.setWidth(25);
		dataGenerator.setHeight(25);
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
	processor.verifyItsNotTotallyBroken();

	Optimizer::climb(trainingExamples, &processor);


	return 0;
}
