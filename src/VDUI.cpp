#include "VDUI.h"

using namespace VideoDromm;

VDUI::VDUI(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
	mVDAnimation = aVDAnimation;
	mVDSession = aVDSession;
	// Console
	mVDConsole = VDConsole::create(mVDSettings, mVDMix, mVDRouter);
	// UITextures
	mUITextures = VDUITextures::create(mVDSettings, mVDMix);
	// UIFbos
	mUIFbos = VDUIFbos::create(mVDSettings, mVDMix);
	// UIAnimation
	mUIAnimation = VDUIAnimation::create(mVDSettings, mVDMix, mVDRouter, mVDAnimation);
	// UIMidi
	mUIMidi = VDUIMidi::create(mVDSettings, mVDRouter);
	// UIAudio
	mUIAudio = VDUIAudio::create(mVDSettings, mVDAnimation);
	// UIColor
	mUIColor = VDUIColor::create(mVDSettings, mVDMix, mVDRouter, mVDAnimation);
	// UITempo
	mUITempo = VDUITempo::create(mVDSettings, mVDMix, mVDAnimation, mVDSession);
	// UIBlend
	mUIBlend = VDUIBlend::create(mVDSettings, mVDMix, mVDAnimation);
	// UIOsc
	mUIOsc = VDUIOsc::create(mVDSettings, mVDMix, mVDRouter);
	// UIWebsockets
	mUIWebsockets = VDUIWebsockets::create(mVDSettings, mVDMix, mVDRouter);
	// UIMouse
	mUIMouse = VDUIMouse::create(mVDSettings, mVDMix, mVDAnimation);		
	// UIShaders
	mUIShaders = VDUIShaders::create(mVDSettings, mVDMix, mVDRouter);
}
VDUI::~VDUI() {

}

void VDUI::Run(const char* title, unsigned int fps) {
	static int currentWindowRow1 = 0;
	static int currentWindowRow2 = 0;
	static int currentWindowRow3 = 0;
	mVDSettings->uiXPos = mVDSettings->uiMargin;
	ui::SetNextWindowSize(ImVec2(1000, 100), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPos, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	sprintf(buf, "Videodromm Fps %c %d###fps", "|/-\\"[(int)(ui::GetTime() / 0.25f) & 3], fps);
	ui::Begin(buf);
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);

		ui::RadioButton("Audio", &currentWindowRow1, 0); ui::SameLine();
		ui::RadioButton("Midi", &currentWindowRow1, 1); ui::SameLine();
		ui::RadioButton("Anim", &currentWindowRow1, 2); ui::SameLine();
		ui::RadioButton("Color", &currentWindowRow1, 3); ui::SameLine();
		ui::RadioButton("Tempo", &currentWindowRow1, 4); ui::SameLine();
		ui::RadioButton("Mouse", &currentWindowRow1, 5); ui::SameLine();
		ui::RadioButton("Osc", &currentWindowRow1, 6);  ui::SameLine();
		ui::RadioButton("WS", &currentWindowRow1, 7); 

		ui::RadioButton("Textures", &currentWindowRow2, 0); ui::SameLine();
		ui::RadioButton("Fbos", &currentWindowRow2, 1); ui::SameLine();
		ui::RadioButton("Shaders", &currentWindowRow2, 2); ui::SameLine();
		ui::RadioButton("Blend", &currentWindowRow2, 3); ui::SameLine();
		ui::RadioButton("Chn", &currentWindowRow2, 4);ui::SameLine();

#pragma region Info
		ui::Text("Msg: %s", mVDSettings->mMsg.c_str());
		ui::Text("Target FPS %.2f ", mVDSession->getTargetFps());
		ui::SameLine();

		// fps
		static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size()*sizeof(float)); }
		static int values_offset = 0;
		static float refresh_time = -1.0f;
		if (ui::GetTime() > refresh_time + 1.0f / 6.0f)
		{
			refresh_time = ui::GetTime();
			values[values_offset] = mVDSettings->iFps;
			values_offset = (values_offset + 1) % values.size();
		}
		if (mVDSettings->iFps < 12.0) ui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ui::PlotLines("FPS", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, mVDSession->getTargetFps(), ImVec2(0, 30));
		if (mVDSettings->iFps < 12.0) ui::PopStyleColor();
#pragma endregion Info	
		ui::PopItemWidth();
	}
	ui::End();


	switch (currentWindowRow1) {
	case 0:
		// Audio
		mUIAudio->Run("Audio");
		break;
	case 1:
		// Midi
		mUIMidi->Run("Midi");
		break;
	case 2:
		// Animation
		mUIAnimation->Run("Animation");
		break;
	case 3:
		// Color
		mUIColor->Run("Color");
		break;
	case 4:
		// Tempo
		mUITempo->Run("Tempo");
		break;
	case 5:
		// Mouse
		mUIMouse->Run("Mouse");
		break;
	case 6:
		// Osc
		mUIOsc->Run("Osc");
		break;
	case 7:
		// Websockets
		mUIWebsockets->Run("Websockets");
		break;
	}
	switch (currentWindowRow2) {
	case 0:
		// textures
		mUITextures->Run("Textures");
		break;
	case 1:
		// Fbos
		mUIFbos->Run("Fbos");
		break;
	case 2:
		// Console
		mVDConsole->Run("Console");
		break;
	case 3:
		// Blend
		mUIBlend->Run("Blend");
		break;
	}


}
