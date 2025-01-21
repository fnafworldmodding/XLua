#include "helper.h"

// takes mv, surface, width, height, hotspotX, hotspotY, actionPointX, actionPointY
WORD CreateImageFromSurface(LPMV pMV, LPSURFACE pSf, int dwWidth, int dwHeight, int hotSpotX, int hotSpotY, int actionPointX, int actionPointY) {
	// Create image
	WORD    wMode = (WORD)(pMV->mvAppMode & SM_MASK);
	if (pSf->HasAlpha() != NULL) {
		wMode |= (IF_ALPHA << 8);
	}
	WORD newImg = (WORD)AddImage(pMV->mvIdAppli, (WORD)dwWidth, (WORD)dwHeight, hotSpotX, hotSpotY,
		actionPointX, actionPointY, pSf->GetTransparentColor(), wMode,
		NULL, NULL);
	if (newImg != 0) {
		// Lock new image surface
		cSurface sfNewImg;
		if (LockImageSurface(pMV->mvIdAppli, newImg, sfNewImg)) {
			// Blit surface into image surface
			pSf->Blit(sfNewImg);
			// Unlock image surface
			UnlockImageSurface(sfNewImg);
		}
		else
			DelImage(pMV->mvIdAppli, newImg);
	}
	return newImg;
}

// takes mv, filePath, hotSpotX, hotSpotY, actionPointX, actionPointY, loadflags
WORD CreateImageFromFile(LPMV pMV, const TCHAR* filePath, int hotSpotX, int hotSpotY, int actionPointX, int actionPointY, LIFlags loadflags) {
	// load the image from file
	cSurface sourceSurface;
	if (sourceSurface.LoadImage(filePath, loadflags)) {
		return 0; // failure :p
	}

	int width = sourceSurface.GetWidth();
	int height = sourceSurface.GetHeight();

	return CreateImageFromSurface(pMV, &sourceSurface, width, height, hotSpotX, hotSpotY, actionPointX, actionPointY);
}
