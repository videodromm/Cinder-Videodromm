#include "VDUI.h"

using namespace VideoDromm;

VDUI::VDUI(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	// Console
	mUIConsole = VDConsole::create(mVDSettings, mVDSession);
	// UITextures
	mUITextures = VDUITextures::create(mVDSettings, mVDSession);
	// UIFbos
	mUIFbos = VDUIFbos::create(mVDSettings, mVDSession);
	// UIAnimation
	mUIAnimation = VDUIAnimation::create(mVDSettings, mVDSession);
	// UIMidi
	mUIMidi = VDUIMidi::create(mVDSettings, mVDSession);
	// UIAudio
	mUIAudio = VDUIAudio::create(mVDSettings, mVDSession);
	// UIColor
	mUIColor = VDUIColor::create(mVDSettings, mVDSession);
	// UITempo
	mUITempo = VDUITempo::create(mVDSettings, mVDSession);
	// UIBlend
	mUIBlend = VDUIBlend::create(mVDSettings, mVDSession);
	// UIOsc
	mUIOsc = VDUIOsc::create(mVDSettings, mVDSession);
	// UIWebsockets
	mUIWebsockets = VDUIWebsockets::create(mVDSettings, mVDSession);
	// UIMouse
	mUIMouse = VDUIMouse::create(mVDSettings, mVDSession);
	// UIShaders
	mUIShaders = VDUIShaders::create(mVDSettings, mVDSession);
	// UIWarps
	mUIWarps = VDUIWarps::create(mVDSettings, mVDSession);
	// imgui
	margin = 3;
	inBetween = 3;
	// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15 mPreviewWidth = 160;mPreviewHeight = 120;
	w = mVDSettings->mPreviewFboWidth + margin;
	h = mVDSettings->mPreviewFboHeight * 2.3;
	largeW = (mVDSettings->mPreviewFboWidth + margin) * 4;
	largeH = (mVDSettings->mPreviewFboHeight + margin) * 5;
	largePreviewW = mVDSettings->mPreviewWidth + margin;
	largePreviewH = (mVDSettings->mPreviewHeight + margin) * 2.4;
	displayHeight = mVDSettings->mMainWindowHeight - 50;
	yPosRow1 = 100 + margin;
	yPosRow2 = yPosRow1 + largePreviewH + margin;
	yPosRow3 = yPosRow2 + h*1.4 + margin;

	mouseGlobal = false;
	
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

	ImGuiStyle& style = ui::GetStyle();

	if (mIsResizing) {
		mIsResizing = false;

		// set ui window and io events callbacks 
		ui::connectWindow(getWindow());
		ui::initialize();

#pragma region style
		// our theme variables
		style.WindowRounding = 4;
		style.WindowPadding = ImVec2(3, 3);
		style.FramePadding = ImVec2(2, 2);
		style.ItemSpacing = ImVec2(3, 3);
		style.ItemInnerSpacing = ImVec2(3, 3);
		style.WindowMinSize = ImVec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		style.Alpha = 0.85f;
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
#pragma region menu
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("Options"))
		{
			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

#pragma endregion menu
	mVDSettings->uiXPos = mVDSettings->uiMargin;
	//ui::SetNextWindowSize(ImVec2(mVDSettings->mRenderWidth - 20, mVDSettings->uiYPosRow2 - mVDSettings->uiYPosRow1 - mVDSettings->uiMargin), ImGuiSetCond_Once);
	ui::SetNextWindowSize(ImVec2(800, mVDSettings->uiYPosRow2 - mVDSettings->uiYPosRow1 - mVDSettings->uiMargin), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPos, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	sprintf(buf, "Videodromm Fps %c %d###fps", "|/-\\"[(int)(ui::GetTime() / 0.25f) & 3], fps);
	ui::Begin(buf);
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// fps
		static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size() * sizeof(float)); }
		static int values_offset = 0;
		static float refresh_time = -1.0f;
		if (ui::GetTime() > refresh_time + 1.0f / 6.0f)
		{
			refresh_time = ui::GetTime();
			values[values_offset] = mVDSettings->iFps;
			values_offset = (values_offset + 1) % values.size();
		}
		if (mVDSettings->iFps < 12.0) ui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ui::PlotLines("FPS ", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, mVDSession->getTargetFps(), ImVec2(0, 30));
		if (mVDSettings->iFps < 12.0) ui::PopStyleColor();
		ui::SameLine();
		ui::Text("(Target FPS %.2f) ", mVDSession->getTargetFps());
		ui::SameLine();
		ui::Text("fp %dx%d f %dx%d r %dx%d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);

		ui::RadioButton("Audio", &currentWindowRow1, 0); ui::SameLine();
		ui::RadioButton("Midi", &currentWindowRow1, 1); ui::SameLine();
		ui::RadioButton("Anim", &currentWindowRow1, 2); ui::SameLine();
		ui::RadioButton("Color", &currentWindowRow1, 3); ui::SameLine();
		ui::RadioButton("Tempo", &currentWindowRow1, 4); ui::SameLine();
		ui::RadioButton("Mouse", &currentWindowRow1, 5); ui::SameLine();
		ui::RadioButton("Osc", &currentWindowRow1, 6);  ui::SameLine();
		ui::RadioButton("WS", &currentWindowRow1, 7);  ui::SameLine();
		// flip vertically
		int hue = 0;
		mVDSession->isFlipV() ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
		if (ui::Button("FlipV")) {
			mVDSession->flipV();
		}
		ui::PopStyleColor(3);
		hue++;
		ui::SameLine();
		// flip horizontally
		mVDSession->isFlipH() ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
		if (ui::Button("FlipH")) {
			mVDSession->flipH();
		}
		ui::PopStyleColor(3);
		hue++;

		ui::RadioButton("Textures", &currentWindowRow2, 0); ui::SameLine();
		ui::RadioButton("Fbos", &currentWindowRow2, 1); ui::SameLine();
		ui::RadioButton("Shaders", &currentWindowRow2, 2); ui::SameLine();
		ui::RadioButton("Blend", &currentWindowRow2, 3); ui::SameLine();
		ui::RadioButton("Console", &currentWindowRow2, 4); ui::SameLine();
		ui::RadioButton("Warps", &currentWindowRow2, 5);

#pragma region Info
		ui::TextWrapped("Msg: %s", mVDSettings->mMsg.c_str());
		ui::TextWrapped("WS Msg: %s", mVDSettings->mWebSocketsMsg.c_str());
		ui::TextWrapped("OSC Msg: %s", mVDSettings->mOSCMsg.c_str());

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
		mUIConsole->Run("Console");
		break;
	case 5:
		// Warps
		mUIWarps->Run("Warps");
		break;
	}
	
	mVDSession->blendRenderEnable(currentWindowRow2 == 3);

}
