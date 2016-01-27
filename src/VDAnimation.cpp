#include "VDAnimation.h"

using namespace VideoDromm;
	
VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	JsonBag::add(&mBackgroundColor, "background_color");
	JsonBag::add(&mBpm, "bpm");
	JsonBag::add(&mFpb, "fpb");

}
void VDAnimation::load() {
	bag()->load();
}
void VDAnimation::save() {
	bag()->save();
}