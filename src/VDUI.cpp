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
void VDUI::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
float VDUI::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUI::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUI::resize() {
	mIsResizing = true;
	// disconnect ui window and io events callbacks
	ImGui::disconnectWindow(getWindow());
}
void VDUI::Run(const char* title, unsigned int fps) {
	static int currentWindowRow1 = 2;
	//static int currentWindowRow2 = 4;

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

	ImGui::SetNextWindowSize(ImVec2(1000, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	sprintf(buf, "Fps %c %d (%.2f)###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], fps, mVDSession->getTargetFps());
	ImGui::Begin(buf);
	{
		// line 1
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
		// audio
		ImGui::SameLine();
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

		ImGui::PlotHistogram("H", mVDSession->getFreqs(), mVDSession->getWindowSize(), 0, NULL, 0.0f, 255.0f, ImVec2(0, 30));
		ImGui::SameLine();
		if (mVDSession->getMaxVolume() > 240.0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PlotLines("V", &timeValues.front(), (int)timeValues.size(), timeValues_offset, toString(int(mVDSession->getMaxVolume())).c_str(), 0.0f, 255.0f, ImVec2(0, 30));
		if (mVDSession->getMaxVolume() > 240.0) ImGui::PopStyleColor();

		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		ImGui::SameLine();
		/*
		ImGui::Image((void*)mVDSession->getFboRenderedTexture(mVDSession->getFboFragmentShaderIndex(0))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getMixTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getFboRenderedTexture(mVDSession->getFboFragmentShaderIndex(1))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		*/
		int currentNode = 0;

		// A (left)
		int fboIndex = mVDSession->getWarpAFboIndex(currentNode);
		int inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpAFboIndex(currentNode));
		ImGui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		// mix
		ImGui::Image((void*)mVDSession->getMixTexture(currentNode)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		// B (right)
		fboIndex = mVDSession->getWarpBFboIndex(currentNode);
		inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpBFboIndex(currentNode));
		ImGui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));


		multx = mVDSession->getFloatUniformValueByIndex(mVDSettings->IAUDIOX); // 13
		if (ImGui::SliderFloat("mult x", &multx, 0.01f, 12.0f)) {
			mVDSession->setFloatUniformValueByIndex(13, multx);
		}
		ImGui::SameLine();
		(mVDSession->isAudioBuffered()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3.0f, 0.8f, 0.8f));
		if (ImGui::Button("Wave")) {
			mVDSession->toggleAudioBuffered();
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		(mVDSession->getUseLineIn()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.0f, 0.8f, 0.8f));
		if (ImGui::Button("LineIn")) {
			mVDSession->toggleUseLineIn();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
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

		int w = 0;
		// loop on the fbos
		for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
			if (a > 0) ImGui::SameLine();
			if (mVDSession->getWarpAFboIndex(w) == a) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

			sprintf(buf, "%d##wia%d%d", a, w, a);
			if (ImGui::Button(buf)) mVDSession->setWarpAFboIndex(w, a);
			sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::PopStyleColor(3);
		}// crossfade
		ImGui::SameLine();
		float xFade = mVDSession->getCrossfade();
		sprintf(buf, "xfade##xfd");
		if (ImGui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
		{
			mVDSession->setCrossfade(xFade);
		}
		// loop on the fbos
		for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
			if (b > 0) ImGui::SameLine();
			if (mVDSession->getWarpBFboIndex(w) == b) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

			sprintf(buf, "%d##wib%d%d", b, w, b);
			if (ImGui::Button(buf)) mVDSession->setWarpBFboIndex(w, b);
			sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::PopStyleColor(3);
		}


		ImGui::RadioButton("Audio", &currentWindowRow1, 0); ImGui::SameLine();
		ImGui::RadioButton("Midi", &currentWindowRow1, 1); ImGui::SameLine();
		ImGui::RadioButton("Anim", &currentWindowRow1, 2); ImGui::SameLine();
		ImGui::RadioButton("Color", &currentWindowRow1, 3); ImGui::SameLine();
		ImGui::RadioButton("Tempo", &currentWindowRow1, 4); ImGui::SameLine();
		ImGui::RadioButton("Mouse", &currentWindowRow1, 5); ImGui::SameLine();
		ImGui::RadioButton("Osc", &currentWindowRow1, 6);  ImGui::SameLine();
		ImGui::RadioButton("WS", &currentWindowRow1, 7);  ImGui::SameLine();
		ImGui::RadioButton("Render", &currentWindowRow1, 8);  ImGui::SameLine();
		ImGui::RadioButton("Blend", &currentWindowRow1, 9); ImGui::SameLine();
		ImGui::RadioButton("Warps", &currentWindowRow1, 10);

#pragma region Info
		ctrl = mVDSettings->IWEIGHT0;
		iWeight0 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W0", &iWeight0, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight0);
		}
		ImGui::SameLine();
		ctrl = mVDSettings->IWEIGHT1;
		iWeight1 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W1", &iWeight1, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight1);
		}
		ImGui::SameLine();
		ctrl = mVDSettings->IWEIGHT2;
		iWeight2 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W2", &iWeight2, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight2);
		}
		ImGui::SameLine();

		ctrl = mVDSettings->IWEIGHT3;
		iWeight3 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W3", &iWeight3, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight3);
		}
		ImGui::SameLine();

		ctrl = mVDSettings->IWEIGHT4;
		iWeight4 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W4", &iWeight4, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight4);
		}
		ImGui::SameLine();

		ctrl = mVDSettings->IWEIGHT5;
		iWeight5 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W5", &iWeight5, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight5);
		}
		ImGui::SameLine();

		ctrl = mVDSettings->IWEIGHT6;
		iWeight6 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W6", &iWeight6, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight6);
		}
		ImGui::SameLine();

		ctrl = mVDSettings->IWEIGHT7;
		iWeight7 = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("W7", &iWeight7, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, iWeight7);
		}
		ImGui::TextWrapped("Msg: %s", mVDSettings->mMsg.c_str());
		ImGui::TextWrapped("Midi: %s", mVDSettings->mMidiMsg.c_str());
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
	case 9:
		// Blend
		mUIBlend->Run("Blend");
		break;
	case 10:
		// Warps
		mUIWarps->Run("Warps");
		break;	
	}
	mUITextures->Run("Textures");
	//mUIFbos->Run("Fbos");
	mUIShaders->Run("Shaders");


	mVDSession->blendRenderEnable(currentWindowRow1 == 9);

}
