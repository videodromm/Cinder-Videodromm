#include "VDUIAudio.h"

using namespace VideoDromm;

VDUIAudio::VDUIAudio(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIAudio::~VDUIAudio() {

}

void VDUIAudio::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

	ui::Begin("Audio");
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		static ImVector<float> timeValues; if (timeValues.empty()) { timeValues.resize(40); memset(&timeValues.front(), 0, timeValues.size()*sizeof(float)); }
		static int timeValues_offset = 0;
		// audio maxVolume
		static float tRefresh_time = -1.0f;
		if (ui::GetTime() > tRefresh_time + 1.0f / 20.0f)
		{
			tRefresh_time = ui::GetTime();
			timeValues[timeValues_offset] = mVDAnimation->maxVolume;
			timeValues_offset = (timeValues_offset + 1) % timeValues.size();
		}

		ui::SliderFloat("mult x", &mVDAnimation->controlValues[13], 0.01f, 10.0f);

		ui::PlotHistogram("Histogram", mVDAnimation->iFreqs, 7, 0, NULL, 0.0f, 255.0f, ImVec2(0, 30));

		if (mVDAnimation->maxVolume > 240.0) ui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ui::PlotLines("Volume", &timeValues.front(), (int)timeValues.size(), timeValues_offset, toString(int(mVDAnimation->maxVolume)).c_str(), 0.0f, 255.0f, ImVec2(0, 30));
		if (mVDAnimation->maxVolume > 240.0) ui::PopStyleColor();
		ui::PopItemWidth();
	}
	ui::End();

}
