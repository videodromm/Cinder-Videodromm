#include "VDUI.h"

using namespace videodromm;

VDUI::VDUI(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
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
	// UIRender
	mUIRender = VDUIRender::create(mVDSettings, mVDSession);
	// imgui
	mouseGlobal = false;
	mIsResizing = true;
}

void VDUI::resize() {
	mIsResizing = true;
	// disconnect ui window and io events callbacks
	ImGui::disconnectWindow(getWindow());
}
void VDUI::Run(const char* title, unsigned int fps) {
	static int currentWindowRow1 = 2;
	static int currentWindowRow2 = 4;

	ImGuiStyle& style = ImGui::GetStyle();

	if (mIsResizing) {
		mIsResizing = false;

		// set ui window and io events callbacks 
		ImGui::connectWindow(getWindow());
		ImGui::initialize();

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
		// style.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
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
	
	//ImGui::SetNextWindowSize(ImVec2(mVDSettings->mRenderWidth - 20, mVDSettings->uiYPosRow2 - mVDSettings->uiYPosRow1 - mVDSettings->uiMargin), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(800, mVDSettings->uiYPosRow2 - mVDSettings->uiYPosRow1 - mVDSettings->uiMargin), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	sprintf(buf, "videodromm Fps %c %d###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], fps);
	ImGui::Begin(buf);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// fps
		static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size() * sizeof(float)); }
		static int values_offset = 0;
		static float refresh_time = -1.0f;
		if (ImGui::GetTime() > refresh_time + 1.0f / 6.0f)
		{
			refresh_time = ImGui::GetTime();
			values[values_offset] = mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS);
			values_offset = (values_offset + 1) % values.size();
		}
		if (mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS) < 12.0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PlotLines("FPS ", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, mVDSession->getTargetFps(), ImVec2(0, 30));
		if (mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS) < 12.0) ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("(Target FPS %.2f) ", mVDSession->getTargetFps());

		ImGui::RadioButton("Audio", &currentWindowRow1, 0); ImGui::SameLine();
		ImGui::RadioButton("Midi", &currentWindowRow1, 1); ImGui::SameLine();
		ImGui::RadioButton("Anim", &currentWindowRow1, 2); ImGui::SameLine();
		ImGui::RadioButton("Color", &currentWindowRow1, 3); ImGui::SameLine();
		ImGui::RadioButton("Tempo", &currentWindowRow1, 4); ImGui::SameLine();
		ImGui::RadioButton("Mouse", &currentWindowRow1, 5); ImGui::SameLine();
		ImGui::RadioButton("Osc", &currentWindowRow1, 6);  ImGui::SameLine();
		ImGui::RadioButton("WS", &currentWindowRow1, 7);  ImGui::SameLine();
		ImGui::RadioButton("Render", &currentWindowRow1, 8);  ImGui::SameLine();
		// flip vertically
		int hue = 0;
		mVDSession->isFlipV() ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("FlipV")) {
			mVDSession->flipV();
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();
		// flip horizontally
		mVDSession->isFlipH() ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("FlipH")) {
			mVDSession->flipH();
		}
		ImGui::PopStyleColor(3);
		hue++;

		ImGui::RadioButton("Textures", &currentWindowRow2, 0); ImGui::SameLine();
		ImGui::RadioButton("Fbos", &currentWindowRow2, 1); ImGui::SameLine();
		ImGui::RadioButton("Shaders", &currentWindowRow2, 2); ImGui::SameLine();
		ImGui::RadioButton("Blend", &currentWindowRow2, 3); ImGui::SameLine();
		ImGui::RadioButton("Warps", &currentWindowRow2, 4); 

#pragma region Info
		ImGui::TextWrapped("Msg: %s", mVDSettings->mMsg.c_str());
		ImGui::TextWrapped("WS Msg: %s", mVDSettings->mWebSocketsMsg.c_str());
		ImGui::TextWrapped("OSC Msg: %s", mVDSettings->mOSCMsg.c_str());

#pragma endregion Info	
		ImGui::PopItemWidth();
	}
	ImGui::End();


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
	case 8:
		// Render
		mUIRender->Run("Render");
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
		// Warps
		mUIWarps->Run("Warps");
		break;
	
	}
	
	mVDSession->blendRenderEnable(currentWindowRow2 == 3);

}
