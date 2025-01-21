#include "common.h"

WORD CreateImageFromSurface(LPMV pMV, LPSURFACE pSf, int dwWidth, int dwHeight, int hotSpotX, int hotSpotY, int actionPointX, int actionPointY);
WORD CreateImageFromFile(LPMV pMV, const TCHAR* filePath, int hotSpotX, int hotSpotY, int actionPointX, int actionPointY, LIFlags loadflags = LI_NONE);
//bool CreateAnimationFromImageArray(rAni* OrAni, const WORD* imageIDs, size_t count, int animID = 12);
//bool CreateAnimationFromImageArray(rAni* OrAni, const WORD* imageIDs, size_t count, int animID, BYTE speed, BYTE minSpeed, BYTE maxSpeed, short repeat, short repeatFrame);

/*
// TODO: simplify the functions by creating Mate structures for the parameters
// example
struct ImageMate {
	int width;
	int height;
	int hotSpotX;
	int hotSpotY;
	int actionPointX;
	int actionPointY;
};

struct AnimationMate {
	int animID;
	int numberOfFrame;
	byte dirIndex;
	byte speed; // also speedForced (that if set, it forces that speed)
	byte maxSpeed;
	byte minSpeed;
	short repeat;
	short repeatFrame;
	string files[1]; // list of image mates
};

WORD CreateImageFromSurface(LPMV pMV, LPSURFACE pSf, ImageMate mate);
WORD CreateImageFromFile(LPMV pMV, const TCHAR* filePath, ImageMate mate, LIFlags loadflags = LI_NONE);

*/
