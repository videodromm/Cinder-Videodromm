#include "VDAnimation.h"

using namespace VideoDromm;
	
VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	JsonBag::add(&mBackgroundColor, "background_color");
	JsonBag::add(&mExposure, "exposure");

}
void VDAnimation::load() {
	bag()->load();
}
void VDAnimation::save() {
	bag()->save();
}