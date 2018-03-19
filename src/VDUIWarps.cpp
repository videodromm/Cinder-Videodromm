#include "VDUIWarps.h"

using namespace VideoDromm;

VDUIWarps::VDUIWarps(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}

void VDUIWarps::Run(const char* title) {
	static int currentNode = 0;
	xPos = mVDSettings->uiMargin;
	yPos = mVDSettings->uiYPosRow3;// - mVDSettings->uiLargePreviewH
	for (int w = 0; w < mVDSession->getWarpCount(); w++) {
		ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		//int hue = 0;
		sprintf(buf, "%s##sh%d", mVDSession->getWarpName(w).c_str(), w);
		ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ImGui::PushID(w);
			ImGui::Image((void*)mVDSession->getMixTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(mVDSession->getWarpName(w).c_str());
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a % 6 != 0)) ImGui::SameLine();
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
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ImGui::SameLine();
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

			// crossfade
			float xFade = mVDSession->getWarpCrossfade(w);
			sprintf(buf, "xfade##xf%d", w);
			if (ImGui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDSession->setWarpCrossfade(w, xFade);
			}
			// nodes
			if (currentNode == w) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "N##n%d", w);
			if (ImGui::Button(buf)) {
				if (w == currentNode) {
					// if the same button pressed we hide the nodes
					currentNode = -1;
				}
				else {
					currentNode = w;
					mVDSession->setCurrentEditIndex(w);
				}
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Show nodes");
			ImGui::SameLine();
			// spout output
			if (mVDSession->getSharedMixIndex() == w && mVDSession->isSharedOutputActive()) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "O##sp%d", w);
			if (ImGui::Button(buf)) {
				mVDSession->toggleSharedOutput(w);
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Spout output");
			ImGui::SameLine();
			// active
			if (mVDSession->isWarpActive(w)) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "AC##a%d", w);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "A##a%d", w);
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ImGui::Button(buf)) {
				mVDSession->toggleWarpActive(w);
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Warp Active");
			ImGui::SameLine();
			// solo
			if (mVDSession->isWarpSolo(w)) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "SOLO##s%d", w);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "S##s%d", w);
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ImGui::Button(buf)) {
				mVDSession->toggleWarpSolo(w);
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Warp Solo");
			ImGui::SameLine();
			// delete
			if (mVDSession->isWarpDeleted(w)) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "X##x%d", w);
			if (ImGui::Button(buf)) {
				mVDSession->toggleDeleteWarp(w);
			}
			ImGui::PopStyleColor(3);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Delete Warp");

			ImGui::PopID();
			ImGui::PopItemWidth();
		}

#pragma region Nodes
		if (currentNode == w) {
			// A (left)
			int t = 0;
			int fboIndex = mVDSession->getWarpAFboIndex(currentNode);
			int inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpAFboIndex(currentNode));

			ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##txa", mVDSession->getInputTextureName(inputTexture).c_str());
			ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::PopItemWidth();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::Text("WoH %dx%d", mVDSession->getInputTextureOriginalWidth(inputTexture), mVDSession->getInputTextureOriginalHeight(inputTexture));
			ImGui::Text("WxH %dx%d", mVDSession->getInputTexture(inputTexture)->getWidth(), mVDSession->getInputTexture(inputTexture)->getHeight());
			ImGui::Text("tx A %s", mVDSession->getInputTextureName(inputTexture).c_str());
			ImGui::End();

			t++;
			ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##fboa", mVDSession->getFboName(fboIndex).c_str());
			ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::PopItemWidth();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::Text("WxH %dx%d", mVDSession->getFboRenderedTexture(fboIndex)->getWidth(), mVDSession->getFboRenderedTexture(fboIndex)->getHeight());
			ImGui::End();

			// mix
			t++;
			ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			ImGui::Begin(mVDSession->getWarpName(currentNode).c_str(), NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::Image((void*)mVDSession->getMixTexture(currentNode)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::PopItemWidth();
			}
			ImGui::Text("WxH %dx%d", mVDSession->getMixTexture(currentNode)->getWidth(), mVDSession->getMixTexture(currentNode)->getHeight());
			ImGui::End();

			// B (right)
			t++;
			fboIndex = mVDSession->getWarpBFboIndex(currentNode);
			inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpBFboIndex(currentNode));
			ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##fbob", mVDSession->getFboName(fboIndex).c_str());
			ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::PopItemWidth();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::Text("WxH %dx%d", mVDSession->getFboRenderedTexture(fboIndex)->getWidth(), mVDSession->getFboRenderedTexture(fboIndex)->getHeight());
			ImGui::End();

			t++;

			ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##txb", mVDSession->getInputTextureName(inputTexture).c_str());
			ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::PopItemWidth();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
			ImGui::Text("WoH %dx%d", mVDSession->getInputTextureOriginalWidth(inputTexture), mVDSession->getInputTextureOriginalHeight(inputTexture));
			ImGui::Text("WxH %dx%d", mVDSession->getInputTexture(inputTexture)->getWidth(), mVDSession->getInputTexture(inputTexture)->getHeight());
			ImGui::Text("tx B %s", mVDSession->getInputTextureName(inputTexture).c_str());
			ImGui::End();
		}
#pragma endregion Nodes

		ImGui::End();
		xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		if (xPos > mVDSettings->mRenderWidth)
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiLargePreviewH + mVDSettings->uiMargin;
		}
	}
}
