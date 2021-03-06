#include "VDUITempo.h"

using namespace videodromm;

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
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

	ImGui::Begin("Tempo");
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth * 2);
		if (ImGui::Button("x##startx")) { mVDSettings->iStart = 0.0f; }
		ImGui::SameLine();
		ImGui::SliderFloat("start", &mVDSettings->iStart, 0.01f, 0.05f, "%.4f");

		if (ImGui::Button("x##spdx")) { mVDSettings->iSpeedMultiplier = 1.0; }
		ImGui::SameLine();
		ImGui::SliderFloat("speed x", &mVDSettings->iSpeedMultiplier, 0.01f, 1.0f, "%.4f");//, 2.01f

		static int tf = 5;
		if (ImGui::Button("x##tfx")) { tf = 5; mVDSession->setTimeFactor(5); }
		ImGui::SameLine();
		if (ImGui::SliderInt("time x", &tf, 0, 8)) mVDSession->setTimeFactor(tf);

		ImGui::Text("beat %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBEAT));
		ImGui::SameLine();
		ImGui::Text("bar %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBAR));
		ImGui::SameLine();
		ImGui::Text("bb %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBARBEAT));

		ImGui::Text("Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITIME));
		ImGui::SameLine();
		ImGui::Text("Tempo Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITEMPOTIME));

		ImGui::Text("Delta Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->IDELTATIME));
		// LiveOSC Obsolete ImGui::Text("Trk %s %.2f", mVDSettings->mTrackName.c_str(), mVDSettings->liveMeter);
		ImGui::SameLine();
		//			ImGui::Checkbox("Playing", &mVDSettings->mIsPlaying);
		ImGui::Text("Tempo %.2f ", mVDSession->getBpm());
		ImGui::Text("Elapsed %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->IELAPSED));
		ImGui::SameLine();
		ImGui::Text("phase %.2f ", mVDSession->getFloatUniformValueByIndex(mVDSettings->IPHASE));

		// BUG taptempo
		if (ImGui::Button("Tap toggle")) { toggleSpinalTap(); }
		if (spinalTap) {
			static float tempo = mVDSession->getBpm();
			//ImGui::Text("Tempo %.2f ", tempo);
			if (ImGui::DragFloat("Tempo", &tempo, 0.01f, 0.01f, 200.0f, "%.2f"))
			{
				mVDSession->setBpm(tempo);
			};
			if (ImGui::SliderFloat("TempoS", &tempo, 0.01f, 200.0f, "%.01f")) {
				mVDSession->setBpm(tempo);
			}
		}
		else {
			if (ImGui::Button("Tap tempo")) { mVDSession->tapTempo(); }
		}


		if (mVDSession->getUseTimeWithTempo()) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.5f));
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		if (ImGui::Button("Time tempo")) { mVDSession->toggleUseTimeWithTempo(); }
		ImGui::PopStyleColor(3);

		ImGui::PopItemWidth();
	}
	ImGui::End();

#pragma endregion Tempo	

}
