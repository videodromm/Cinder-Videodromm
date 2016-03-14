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

	settings.addChild(ci::JsonTree("bpm", ci::toString(mBpm)));

	doc.pushBack(settings);
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
		mTargetFps = mBpm / 60.0f * mFpb;
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
	resetSomeParams();


}
