#include <cassert>
#include "basictypes.h"

class Grid {
	const uint32 mWidth, mHeight;
	float* mData;

public:
	Grid()
		: mWidth(0), mHeight(0) {
		assert(0);
	}

	Grid(const uint32 width, const uint32 height)
		: mWidth(width), mHeight(height) {
		mData = new float[mWidth*mHeight];
	}
};