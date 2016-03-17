//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace VideoDromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;

	// reset no matter what, so we don't miss anything
	reset();

	// check to see if VDSession.xml file exists and restore if it does
	sessionPath = getAssetPath("") / mVDSettings->mAssetsPath / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		// Create json file if it doesn't already exist.
		std::ofstream oStream(sessionPath.string());
		oStream.close();
		save();
	}
}

VDSessionRef VDSession::create(VDSettingsRef aVDSettings)
{
	return shared_ptr<VDSession>(new VDSession(aVDSettings));
}

bool VDSession::save()
{
	// save in sessionPath
	JsonTree doc;

	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("bpm", mBpm));
	settings.addChild(ci::JsonTree("fadeindelay", mFadeInDelay));
	settings.addChild(ci::JsonTree("fadeoutdelay", mFadeOutDelay));
	settings.addChild(ci::JsonTree("endframe", mEndFrame));
	doc.pushBack(settings);

	JsonTree assets = JsonTree::makeArray("assets");
	assets.addChild(ci::JsonTree("wavefile", mWaveFileName));
	assets.addChild(ci::JsonTree("waveplaybackdelay", mWavePlaybackDelay));
	assets.addChild(ci::JsonTree("moviefile", mMovieFileName));
	assets.addChild(ci::JsonTree("movieplaybackdelay", mMoviePlaybackDelay));
	doc.pushBack(assets);

	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());

	return true;
}

void VDSession::restore()
{
	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));

		JsonTree settings(doc.getChild("settings"));
		mBpm = settings.getValueForKey<float>("bpm");
		mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
		mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
		mEndFrame = settings.getValueForKey<int>("endframe");
		mTargetFps = mBpm / 60.0f * mFpb;

		JsonTree assets(doc.getChild("assets"));
		mWaveFileName = assets.getValueForKey<string>("wavefile");
		mWavePlaybackDelay = assets.getValueForKey<int>("waveplaybackdelay");
		mMovieFileName = assets.getValueForKey<string>("moviefile");
		mMoviePlaybackDelay = assets.getValueForKey<int>("movieplaybackdelay");
	}
	catch (const JsonTree::ExcJsonParserError&)  {
		//CI_LOG_E("Failed to parse json file.");
	}
	//"sequence_folder" : "mandalas"

}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
	mFpb = 16;
	mTargetFps = mBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mFlipV = false;
	mFlipH = false;
	mBpm = 166;
	mWaveFileName = "none.wav";
	mWavePlaybackDelay = 10;
	mMovieFileName = "none.mov";
	mMoviePlaybackDelay = 10;
	mFadeInDelay= 1;
	mFadeOutDelay = 1;
	mEndFrame = 20000;
	resetSomeParams();


}
