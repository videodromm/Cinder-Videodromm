#include "VDUtils.h"

using namespace VideoDromm;

VDUtils::VDUtils(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;
	CI_LOG_V("VDUtils constructor");
	x1 = 0;
	y1 = 0;
	x2 = mVDSettings->mFboWidth;
	y2 = mVDSettings->mFboHeight;

}
float VDUtils::formatFloat(float f)
{
	int i;
	f *= 100;
	i = ((int)f) / 100;
	return (float)i;
}

void VDUtils::setup()
{
	createWarpFbos();
}

void VDUtils::createWarpFbos()
{
	// vector + dynamic resize
	/*for (int a = 0; a < 12; a++)
	{
		WarpFbo newWarpFbo;
		if (a == 0)
		{
			newWarpFbo.textureIndex = 0; // spout
			newWarpFbo.textureMode = mVDSettings->TEXTUREMODEINPUT;
			newWarpFbo.active = true;
			newWarpFbo.fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		}
		else
		{
			newWarpFbo.textureIndex = 0; // index of MixFbo for shadamixa
			newWarpFbo.textureMode = mVDSettings->TEXTUREMODESHADER;
			newWarpFbo.active = false;
			newWarpFbo.fbo = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		}
		mVDSettings->mWarpFbos.push_back(newWarpFbo);
	}*/
}

int VDUtils::getWindowsResolution()
{
	mVDSettings->mDisplayCount = 0;
	int w = Display::getMainDisplay()->getWidth();
	int h = Display::getMainDisplay()->getHeight();

	// Display sizes
	if (mVDSettings->mAutoLayout)
	{
		mVDSettings->mMainWindowWidth = w;
		mVDSettings->mMainWindowHeight = h;
		mVDSettings->mRenderX = mVDSettings->mMainWindowWidth;
		// for MODE_MIX and triplehead(or doublehead), we only want 1/3 of the screen centered	
		for (auto display : Display::getDisplays())
		{
			CI_LOG_V("Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;
			mVDSettings->mRenderWidth = display->getWidth();
			mVDSettings->mRenderHeight = display->getHeight();
		}
	}
	else
	{
		for (auto display : Display::getDisplays())
		{
			CI_LOG_V("Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;

		}
	}
	mVDSettings->mRenderY = 0;

	CI_LOG_V(" mMainDisplayWidth:" + toString(mVDSettings->mMainWindowWidth) + " mMainDisplayHeight:" + toString(mVDSettings->mMainWindowHeight));
	CI_LOG_V(" mRenderX:" + toString(mVDSettings->mRenderX) + " mRenderY:" + toString(mVDSettings->mRenderY));
	mVDSettings->mRenderResoXY = vec2(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
	// in case only one screen , render from x = 0
	if (mVDSettings->mDisplayCount == 1) mVDSettings->mRenderX = 0;
	splitWarp(mVDSettings->mFboWidth,mVDSettings->mFboHeight);
	return w;
}
void VDUtils::splitWarp(int fboWidth, int fboHeight) {

	x1 = 0;
	y1 = 0;
	x2 = fboWidth;
	y2 = fboHeight;

	if (mVDSettings->mSplitWarpH) {
		x2 = (fboWidth / 2) - 1;
		mSrcAreaLeftOrTop = Area(x1, y1, x2, y2);
		x1 = fboWidth / 2;
		x2 = fboWidth;
		mSrcAreaRightOrBottom = Area(x1, y1, x2, y2);
	}
	else if (mVDSettings->mSplitWarpV) {
		int y2 = (fboHeight / 2) - 1;
		mSrcAreaLeftOrTop = Area(x1, y1, x2, y2);
		y1 = fboHeight / 2;
		y2 = fboHeight;
		mSrcAreaRightOrBottom = Area(x1, y1, x2, y2);
	} 
	else
	{
		mSrcAreaLeftOrTop = Area(x1, y1, x2, y2);
		mSrcAreaRightOrBottom = Area(x1, y1, x2, y2);
	}

}
void VDUtils::moveX1SrcAreaLeftOrTop(int x1LeftOrTop) {
	x1 = x1LeftOrTop;
	mSrcAreaLeftOrTop = Area(x1, y1, x2, y2);
}

Area VDUtils::getSrcAreaLeftOrTop() {
	return mSrcAreaLeftOrTop;
}
Area VDUtils::getSrcAreaRightOrBottom() {
	return mSrcAreaRightOrBottom;
}

fs::path VDUtils::getPath(string path)
{
	fs::path p = app::getAssetPath("");
	if (path.length() > 0) { p += fs::path("/" + path); }
	return p;
}
