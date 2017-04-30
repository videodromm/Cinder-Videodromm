#include "VDUtils.h"

using namespace VideoDromm;

VDUtils::VDUtils(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;
	//CI_LOG_V("VDUtils constructor");
	x1LeftOrTop = 0;
	y1LeftOrTop = 0;
	x2LeftOrTop = mVDSettings->mFboWidth;
	y2LeftOrTop = mVDSettings->mFboHeight;
	x1RightOrBottom = 0;
	y1RightOrBottom = 0;
	x2RightOrBottom = mVDSettings->mFboWidth;
	y2RightOrBottom = mVDSettings->mFboHeight;

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
			//CI_LOG_V("VDUtils Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;
			mVDSettings->mRenderWidth = display->getWidth();
			mVDSettings->mRenderHeight = display->getHeight();
		}
	}
	else
	{
		for (auto display : Display::getDisplays())
		{
			//CI_LOG_V("VDUtils Window #" + toString(mVDSettings->mDisplayCount) + ": " + toString(display->getWidth()) + "x" + toString(display->getHeight()));
			mVDSettings->mDisplayCount++;
		}
	}
	mVDSettings->mRenderY = 0;

	//CI_LOG_V("VDUtils mMainDisplayWidth:" + toString(mVDSettings->mMainWindowWidth) + " mMainDisplayHeight:" + toString(mVDSettings->mMainWindowHeight));
	//CI_LOG_V("VDUtils mRenderWidth:" + toString(mVDSettings->mRenderWidth) + " mRenderHeight:" + toString(mVDSettings->mRenderHeight));
	//CI_LOG_V("VDUtils mRenderX:" + toString(mVDSettings->mRenderX) + " mRenderY:" + toString(mVDSettings->mRenderY));
	//mVDSettings->mRenderResoXY = vec2(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
	// in case only one screen , render from x = 0
	if (mVDSettings->mDisplayCount == 1) mVDSettings->mRenderX = 0;
	splitWarp(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
	
	return w;
}
void VDUtils::splitWarp(int fboWidth, int fboHeight) {

	x1LeftOrTop = x1RightOrBottom = 0;
	y1LeftOrTop = y1RightOrBottom = 0;
	x2LeftOrTop = x2RightOrBottom = mVDSettings->mFboWidth;
	y2LeftOrTop = y2RightOrBottom = mVDSettings->mFboHeight;

	if (mVDSettings->mSplitWarpH) {
		x2LeftOrTop = (fboWidth / 2) - 1;

		x1RightOrBottom = fboWidth / 2;
		x2RightOrBottom = fboWidth;
	}
	else if (mVDSettings->mSplitWarpV) {
		y2LeftOrTop = (fboHeight / 2) - 1;
		y1RightOrBottom = fboHeight / 2;
		y2RightOrBottom = fboHeight;
	}
	else
	{
		// no change
	}
	mSrcAreaLeftOrTop = Area(x1LeftOrTop, y1LeftOrTop, x2LeftOrTop, y2LeftOrTop);
	mSrcAreaRightOrBottom = Area(x1RightOrBottom, y1RightOrBottom, x2RightOrBottom, y2RightOrBottom);

}
void VDUtils::moveX1LeftOrTop(int x1) {
	x1LeftOrTop = x1;
	mSrcAreaLeftOrTop = Area(x1LeftOrTop, y1LeftOrTop, x2LeftOrTop, y2LeftOrTop);
}
void VDUtils::moveY1LeftOrTop(int y1) {
	y1LeftOrTop = y1;
	mSrcAreaLeftOrTop = Area(x1LeftOrTop, y1LeftOrTop, x2LeftOrTop, y2LeftOrTop);
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
string VDUtils::getFileNameFromFullPath(string path)
{
	fs::path fullPath = path;
	return fullPath.filename().string();
}
