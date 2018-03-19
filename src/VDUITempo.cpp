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
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ImGui::Begin("Tempo");
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		if (ImGui::Button("x##spdx")) { mVDSettings->iSpeedMultiplier = 1.0; }
		ImGui::SameLine();
		ImGui::SliderFloat("speed x", &mVDSettings->iSpeedMultiplier, 0.01f, 5.0f, "%.1f");
		/* TODO
		ImGui::Text("Beat %d ", mVDSettings->iBeat);
		ImGui::SameLine();
		ImGui::Text("Beat Idx %d ", mVDAnimation->iBeatIndex);
		//ImGui::SameLine();
		//ImGui::Text("Bar %d ", mVDAnimation->iBar);
		if (ImGui::Button("x##bpbx")) { mVDSession->setControlValue("iBeat", 1); }
		ImGui::SameLine();
 */
		ImGui::Text("beat %d ", mVDSession->getIntUniformValueByName("iBeat"));
		ImGui::SameLine();
		ImGui::Text("beats/bar %d ", mVDSession->getIntUniformValueByName("iBeatsPerBar"));

		ImGui::Text("Time %.2f", mVDSession->getFloatUniformValueByName("iGlobalTime"));
		ImGui::SameLine();
		ImGui::Text("Tempo Time %.2f", mVDSession->getFloatUniformValueByName("iTempoTime"));

		ImGui::Text("Trk %s %.2f", mVDSettings->mTrackName.c_str(), mVDSettings->liveMeter);
		ImGui::SameLine();
		//			ImGui::Checkbox("Playing", &mVDSettings->mIsPlaying);
		ImGui::Text("Tempo %.2f ", mVDSession->getBpm());

		if (ImGui::Button("Tap tempo")) { mVDSession->tapTempo(); }
		// TODO if (ImGui::Button("Time tempo")) { mVDAnimation->mUseTimeWithTempo = !mVDAnimation->mUseTimeWithTempo; }

		// TODO ImGui::SliderFloat("time x", &mVDAnimation->iTimeFactor, 0.0001f, 1.0f, "%.01f");
		ImGui::SameLine();
		ImGui::PopItemWidth();
	}
	ImGui::End();

#pragma endregion Tempo	

}
