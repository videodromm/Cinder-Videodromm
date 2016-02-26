#include "VDAudio.h"

using namespace VideoDromm;

VDAudio::VDAudio(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;

	mTexture = gl::Texture::create(dTexture, 0x1909, 512, 2); //#define GL_LUMINANCE 0x1909

	auto ctx = audio::Context::master();
	// linein
	if (mVDSettings->mUseLineIn) {
		try {
			mLineIn = ctx->createInputDeviceNode(); //crashes if linein is present but disabled, doesn't go to catch block

			//*mScopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(512).windowSize(256);
			auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mVDSettings->mFftSize).windowSize(mVDSettings->mWindowSize);
			mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
			mLineIn >> mMonitorLineInSpectralNode;

			mLineIn->enable();
		}
		catch (...) {
			// never called
			mVDSettings->mUseLineIn = false;
		}
	}

	ctx->enable();

}

ci::gl::TextureRef VDAudio::getTexture() {

	return mTexture;
}

void VDAudio::update()
{
	if (mVDSettings->mUseLineIn)
	{
		mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
	}
	
	if (mMagSpectrum.empty())
		return;

	unsigned char signal[kBands];
	mVDSettings->maxVolume = 0.0;
	size_t mDataSize = mMagSpectrum.size();
	if (mDataSize > 0 && mDataSize < 2048)
	{
		float mv;
		float db;
		float maxdb = 0.0f;
		for (size_t i = 0; i < mDataSize; i++) {
			float f = mMagSpectrum[i];
			db = audio::linearToDecibel(f);
			f = db * mVDSettings->controlValues[13];
			if (f > mVDSettings->maxVolume)
			{
				mVDSettings->maxVolume = f; mv = f;
			}
			mVDSettings->mData[i] = f;
			int ger = f;
			signal[i] = static_cast<unsigned char>(ger);

			if (db > maxdb) maxdb = db;

			switch (i)
			{
			case 11:
				mVDSettings->iFreqs[0] = f;
				arr[0] = f;
				break;
			case 13:
				mVDSettings->iFreqs[1] = f;
				arr[1] = f;
				break;
			case 15:
				mVDSettings->iFreqs[2] = f;
				arr[2] = f;
				break;
			case 18:
				mVDSettings->iFreqs[3] = f;
				arr[3] = f;
				break;
			case 25:
				arr[4] = f;
				break;
			case 30:
				arr[5] = f;
				break;
			case 35:
				arr[6] = f;
				break;

			default:
				break;
			}

		}
		// store it as a 512x2 texture in UPDATE only!!
		mTexture = gl::Texture::create(signal, 0x1909, 512, 2); //#define GL_LUMINANCE 0x1909
	}


}