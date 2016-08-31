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
	mIsResizing = true;
}
VDUI::~VDUI() {

}
void VDUI::resize() {
	mIsResizing = true;
	// disconnect ui window and io events callbacks
	ui::disconnectWindow(getWindow());
}
void VDUI::Run(const char* title, unsigned int fps) {
	static int currentWindowRow1 = 0;
	static int currentWindowRow2 = 0;
	static int currentWindowRow3 = 0;

	if (mIsResizing) {
		mIsResizing = false;

		// set ui window and io events callbacks 
		ui::connectWindow(getWindow());
		ui::initialize();

#pragma region style
		// our theme variables
		ImGuiStyle& style = ui::GetStyle();
		style.WindowRounding = 4;
		style.WindowPadding = ImVec2(3, 3);
		style.FramePadding = ImVec2(2, 2);
		style.ItemSpacing = ImVec2(3, 3);
		style.ItemInnerSpacing = ImVec2(3, 3);
		style.WindowMinSize = ImVec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		style.Alpha = 0.6f;
		style.Colors[ImGuiCol_Text] = ImVec4(0.89f, 0.92f, 0.94f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.38f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.4f, 0.0f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.97f, 0.0f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.99f, 0.22f, 0.22f, 0.50f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.04f, 0.04f, 0.04f, 0.22f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9f, 0.45f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.65f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.8f, 0.35f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
#pragma endregion style
	}

	mVDSettings->uiXPos = mVDSettings->uiMargin;
	ui::SetNextWindowSize(ImVec2(mVDSettings->mRenderWidth / 2, mVDSettings->uiYPosRow2 - mVDSettings->uiYPosRow1 - mVDSettings->uiMargin), ImGuiSetCond_Once);
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
		ui::RadioButton("WS", &currentWindowRow1, 7);  ui::SameLine();
		// crossfade
		if (ui::DragFloat("Xfade", &mVDAnimation->controlValues[18], 0.01f, 0.001f, 1.0f))
		{
		}

		ui::RadioButton("Textures", &currentWindowRow2, 0); ui::SameLine();
		ui::RadioButton("Fbos", &currentWindowRow2, 1); ui::SameLine();
		ui::RadioButton("Shaders", &currentWindowRow2, 2); ui::SameLine();
		ui::RadioButton("Blend", &currentWindowRow2, 3); ui::SameLine();
		ui::RadioButton("Console", &currentWindowRow2, 4); ui::SameLine();
		ui::RadioButton("Chn", &currentWindowRow2, 5); ui::SameLine();


#pragma region Info
		ui::Text("fp %dx%d f %dx%d r %dx%d t %d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, mVDAnimation->iTempoTime);
		ui::Text("Msg: %s", mVDSettings->mMsg.c_str());
		ui::Text("WS Msg: %s", mVDSettings->mWebSocketsMsg.c_str());
		ui::Text("OSC Msg: %s", mVDSettings->mOSCMsg.c_str());
		
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
		// Shaders
		mUIShaders->Run("Shaders");
		break;
	case 3:
		// Blend
		mUIBlend->Run("Blend");
		break;
	case 4:
		// Console
		mVDConsole->Run("Console");
		break;
	}
	
	mVDMix->blendRenderEnable(currentWindowRow2 == 3);

}
