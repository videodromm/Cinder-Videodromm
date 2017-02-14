#include "VDUITempo.h"

using namespace VideoDromm;

VDUITempo::VDUITempo(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUITempo::~VDUITempo() {
}

void VDUITempo::Run(const char* title) {

	// Tempo
#pragma region Tempo
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ui::Begin("Tempo");
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		if (ui::Button("x##spdx")) { mVDSettings->iSpeedMultiplier = 1.0; }
		ui::SameLine();
		ui::SliderFloat("speed x", &mVDSettings->iSpeedMultiplier, 0.01f, 5.0f, "%.1f");
		/* TODO
		ui::Text("Beat %d ", mVDSettings->iBeat);
		ui::SameLine();
		ui::Text("Beat Idx %d ", mVDAnimation->iBeatIndex);
		//ui::SameLine();
		//ui::Text("Bar %d ", mVDAnimation->iBar);
		if (ui::Button("x##bpbx")) { mVDSession->setControlValue("iBeat", 1); }
		ui::SameLine();
 */
		ui::Text("beat %d ", mVDSession->getIntUniformValueByName("iBeat"));
		ui::SameLine();
		ui::Text("beats/bar %d ", mVDSession->getIntUniformValueByName("iBeatsPerBar"));

		ui::Text("Time %.2f", mVDSession->getFloatUniformValueByName("iGlobalTime"));
		ui::SameLine();
		ui::Text("Tempo Time %.2f", mVDSession->getFloatUniformValueByName("iTempoTime"));

		ui::Text("Trk %s %.2f", mVDSettings->mTrackName.c_str(), mVDSettings->liveMeter);
		ui::SameLine();
		//			ui::Checkbox("Playing", &mVDSettings->mIsPlaying);
		ui::Text("Tempo %.2f ", mVDSession->getBpm());

		if (ui::Button("Tap tempo")) { mVDSession->tapTempo(); }
		// TODO if (ui::Button("Time tempo")) { mVDAnimation->mUseTimeWithTempo = !mVDAnimation->mUseTimeWithTempo; }

		// TODO ui::SliderFloat("time x", &mVDAnimation->iTimeFactor, 0.0001f, 1.0f, "%.01f");
		ui::SameLine();
		ui::PopItemWidth();
	}
	ui::End();

#pragma endregion Tempo	

}
