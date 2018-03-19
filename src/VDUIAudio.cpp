#include "VDUIAudio.h"

using namespace VideoDromm;

VDUIAudio::VDUIAudio(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIAudio::~VDUIAudio() {

}

void VDUIAudio::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	sprintf(buf, "%s##inpt", mVDSession->getInputTextureName(0).c_str());
	ImGui::Begin(buf);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth*2);
		static ImVector<float> timeValues; if (timeValues.empty()) { timeValues.resize(40); memset(&timeValues.front(), 0, timeValues.size() * sizeof(float)); }
		static int timeValues_offset = 0;
		// audio maxVolume
		static float tRefresh_time = -1.0f;
		if (ImGui::GetTime() > tRefresh_time + 1.0f / 20.0f)
		{
			tRefresh_time = ImGui::GetTime();
			timeValues[timeValues_offset] = mVDSession->getMaxVolume();
			timeValues_offset = (timeValues_offset + 1) % timeValues.size();
		}
		multx = mVDSession->getFloatUniformValueByIndex(13);
		if (ImGui::SliderFloat("mult x", &multx, 0.01f, 12.0f)) {
			mVDSession->setFloatUniformValueByIndex(13, multx);
		}

		ImGui::PlotHistogram("Histogram", mVDSession->getFreqs(), mVDSession->getWindowSize(), 0, NULL, 0.0f, 255.0f, ImVec2(0, 30));

		if (mVDSession->getMaxVolume() > 240.0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PlotLines("Volume", &timeValues.front(), (int)timeValues.size(), timeValues_offset, toString(int(mVDSession->getMaxVolume())).c_str(), 0.0f, 255.0f, ImVec2(0, 30));
		if (mVDSession->getMaxVolume() > 240.0) ImGui::PopStyleColor();

		(mVDSession->isAudioBuffered()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3.0f, 0.8f, 0.8f));
		if (ImGui::Button("Audio Buffered")) {
			mVDSession->toggleAudioBuffered();
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		(mVDSession->getUseLineIn()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.0f, 0.8f, 0.8f));
		if (ImGui::Button("Use Line In")) {
			mVDSession->toggleUseLineIn();
		}
		ImGui::PopStyleColor(3);

		ImGui::Text("Position %d", mVDSession->getPosition(0));

		static int iFreq0 = mVDSession->getFreqIndex(0);
		sprintf(buf, "f0 %4.2f##f0", mVDSession->getFreq(0));
		if (ImGui::SliderInt(buf, &iFreq0, 0, mVDSession->getWindowSize()))
		{
			mVDSession->setFreqIndex(0, iFreq0);
		}
		static int iFreq1 = mVDSession->getFreqIndex(1);
		sprintf(buf, "f1 %4.2f##f1", mVDSession->getFreq(1));
		if (ImGui::SliderInt(buf, &iFreq1, 0, mVDSession->getWindowSize()))
		{
			mVDSession->setFreqIndex(1, iFreq1);
		} 

		static int iFreq2 = mVDSession->getFreqIndex(2);
		sprintf(buf, "f2 %4.2f##f2", mVDSession->getFreq(2));
		if (ImGui::SliderInt(buf, &iFreq2, 0, mVDSession->getWindowSize()))
		{
			mVDSession->setFreqIndex(2, iFreq2);
		}

		static int iFreq3 = mVDSession->getFreqIndex(3);
		sprintf(buf, "f3 %4.2f##f3", mVDSession->getFreq(3));
		if (ImGui::SliderInt(buf, &iFreq3, 0, mVDSession->getWindowSize()))
		{
			mVDSession->setFreqIndex(3, iFreq3);
		}

		(mVDSession->getFreqWSSend()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.0f, 0.8f, 0.8f));
		if (ImGui::Button("Send WS Freqs")) {
			mVDSession->toggleFreqWSSend();
		}
		ImGui::PopStyleColor(3);
		
		ImGui::PopItemWidth();
	}
	ImGui::End();

}
