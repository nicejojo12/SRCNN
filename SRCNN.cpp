#include <iostream>
#include "datagenerator.h"

using namespace std;

int main(int argc, char** argv) {
	for (int examples = 0; examples < 10; examples++) {
		DataGenerator dataGenerator;
		dataGenerator.setError(0.0001);
		dataGenerator.setEnergyBandCoefficient(.2);
		dataGenerator.setSelfEnergyCoefficient(.07);
		dataGenerator.setMinEnergy(10);
		dataGenerator.setMaxEnergy(11);
		dataGenerator.setMinMomentum(10);
		dataGenerator.setMaxMomentum(11);
		dataGenerator.setLambda(.1);
		dataGenerator.setSigmaX(.02);
		dataGenerator.setSigmaY(.03);
		dataGenerator.setWidth(25);
		dataGenerator.setHeight(25);
		pair<Grid, Grid> trainingExample = dataGenerator.createTrainingExample();

		if (0 == examples) {
			cout << "Ideal image:" << endl;
			trainingExample.second.printHeatmap();
		}

		cout << "Blurred image:" << endl;
		trainingExample.first.printHeatmap();

	}
	return 0;
}
