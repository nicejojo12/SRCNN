#include <cassert>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "basictypes.h"
#include "math.h"



using namespace std;

struct Location {
	const uint32 col, row, layer;

	Location(const uint32 yCol, const uint32 yRow, const uint32 yLayer = 0)
		: col(yCol), row(yRow), layer(yLayer) {}
};

class Grid {

	// Define variables
	const uint32 kWidth, kHeight, kLayers;
	float* mData; // TODO: maybe use a vector here

	uint32 getIndex(const Location& loc) const {
		if (loc.col < kWidth && loc.row < kHeight && loc.layer < kLayers) {
			const uint32 index = loc.layer * kHeight * kWidth + loc.row * kWidth + loc.col;
			assert(loc.col == getCol(index));
			assert(loc.row == getRow(index));
			assert(loc.layer == getLayer(index));
			return index;
		}
		return -1;
	}

	uint32 getCol(const uint32 index) const {
		return index % kWidth;
	}

	uint32 getRow(const uint32 index) const {
		return (index / kWidth) % kHeight;
	}

	uint32 getLayer(const uint32 index) const {
		return index / kWidth / kHeight;
	}

public:
	Grid()
		: kWidth(0), kHeight(0), kLayers(0) {
		assert(0);
	}

	Grid(const uint32 width, const uint32 height, const uint32 layers = 1)
		: kWidth(width), kHeight(height), kLayers(layers) {
		mData = new float[size()];
	}

	Grid(const Grid& grid)
		: kWidth(grid.kWidth), kHeight(grid.kHeight), kLayers(grid.kLayers) {
		mData = new float[grid.size()];
		*this = grid;
	}

	~Grid() {
		delete mData;
	}

	Grid& operator= (const Grid& rhs) {
		assert(kWidth == rhs.kWidth);
		assert(kHeight == rhs.kHeight);
		assert(kLayers == rhs.kLayers);
		for (uint32 i = 0; i < size(); i++) {
			set(i, rhs.get(i));
		}
		return *this;
	}

	void floor() {
		for (uint32 i = 0; i < size(); i++) {
			if (get(i) < 0) {
				set(i, 0);
			}
		}
	}

	void mutate() {
		mData[rand()%size()] += Math::normalDistribution();
	}

	void set(const Location& loc, const float value) {
		set(getIndex(loc), value);
		assert(value == get(loc));
	}

	void set(const uint32 index, const float value) {
		if (index < size()) {
			mData[index] = value;
		}
		if (!(value == get(index))) {
			cout << "value = " << value << endl;
			cout << "saved: " << get(index) << endl;
			assert(value == get(index));
		}
	}

	float get(const Location& loc) const {
		assert(loc.col < kWidth);
		assert(loc.row < kHeight);
		assert(loc.layer < kLayers);
		return get(getIndex(loc));
	}

	float get(const uint32 index) const {
		assert(index < size());
		return mData[index];
	}

	uint32 size() const {
		return kWidth * kHeight * kLayers;
	}

	uint32 area() const {
		return kWidth * kHeight;
	}

	uint32 getWidth() const {
		return kWidth;
	}

	uint32 getHeight() const {
		return kHeight;
	}

	uint32 getLayers() const {
		return kLayers;
	}

	float getMax() const {
		float result = get(0);
		for (uint32 i = 0; i < size(); i++) {
			result = max(result, get(i));
		}
		return result;
	}

	float getMin() const {
		float result = get(0);
		for (uint32 i = 0; i < size(); i++) {
			result = min(result, get(i));
		}
		return result;
	}

	void print() const {
		for (uint32 layer = 0; layer < kLayers; layer++) {
			cout << "Layer " << layer << ":" << endl;
			for (uint32 row = 0; row < kHeight; row++) {
				for (uint32 col = 0; col < kWidth; col++) {
					cout << get(Location(col, row, layer)) << " ";
				}
				cout << endl;
			}
			cout << endl;
		}
	}

	// In the case of a 1-layer grid, print it as a 6-color heatmap
	// Each color has a domain of 1/6, and only values from 0 to 1 make sense here
	// Any value greater than 1 is automatically 1
	// Any value less than 0 is automatically 0
	void printHeatmap() const {
		assert(1 == kLayers);
		cout << "Key: low intensity ";
		int key[6] = {47, 46, 42, 43, 41, 40};
		for (int i = 0; i < 6; i++) {
			cout << "\033[" << key[i] << "m \033[0m";
		}
		cout << " high intensity" << endl;
		for (int i = 0; i < kHeight; i++) {
			for (int j = 0; j < kWidth; j++) {
				const float value = get(Location(i, j));
				const int index = max(0, min(5, int(value*6)));
				cout << "\033[" << key[index] << "m  \033[0m";
			}
			cout << "\n";
		}
		cout << endl;
	}

	void writeToFile(const string& filename) const {
		FILE *imageFile;
		imageFile=fopen((filename + ".ppm").c_str(),"wb");
		if(imageFile==NULL){
		perror("ERROR: Cannot open output file");
		exit(EXIT_FAILURE);
		}

		fprintf(imageFile,"P6\n");	// P6 filetype
		fprintf(imageFile,"%d %d\n",kWidth,kHeight);	// dimensions
		fprintf(imageFile,"255\n");

		for (int i = 0; i < size(); i++) {
			const float value = get(i);
			const float trimmedValue = max(0.0f, min(1.0f, value));
			const uint8 pixelValue = (int)(255 * trimmedValue);
			uint8 pix[] = {pixelValue, pixelValue, pixelValue};
			fwrite(pix,1,3,imageFile);
		}
		fclose(imageFile);

		// // From http://www.cplusplus.com/forum/general/6194/
		// //   RGB_t data[ height ][ width ]

		// ofstream tgafile( filename.c_str(), ios::binary );
		// assert(tgafile);

		// int8 header[ 18 ] = { 0 };
		// header[  2 ] = 1;  // truecolor
		// header[ 12 ] =  kWidth        & 0xFF;
		// header[ 13 ] = (kWidth  >> 8) & 0xFF;
		// header[ 14 ] =  kHeight       & 0xFF;
		// header[ 15 ] = (kHeight >> 8) & 0xFF;
		// header[ 16 ] = 24;  // bits per pixel

		// tgafile.write( (const char*)header, 18 );

		// // The image data is stored bottom-to-top, left-to-right
		// for (int i = 0; i < size(); i++) {
		// 	const float value = get(i);
		// 	const float trimmedValue = max(0.0f, min(1.0f, value));
		// 	const uint8 pixelValue = (int)(255 * trimmedValue);
		// 	tgafile.put((char)pixelValue);
		// 	tgafile.put((char)pixelValue);
		// 	tgafile.put((char)pixelValue);
		// 	// tgafile.write(&pixelValue, 1);
		// 	// tgafile.write(&pixelValue, 1);
		// 	// tgafile.write(&pixelValue, 1);
		// }

		// // The file footer. This part is totally optional.
		// static const char footer[ 26 ] =
		// 	"\0\0\0\0"  // no extension area
		// 	"\0\0\0\0"  // no developer directory
		// 	"TRUEVISION-XFILE"  // yep, this is a TGA file
		// 	".";
		// tgafile.write( footer, 26 );

		// tgafile.close();
	}
};
