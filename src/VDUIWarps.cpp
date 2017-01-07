#include "VDUIWarps.h"

using namespace VideoDromm;

VDUIWarps::VDUIWarps(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIWarps::~VDUIWarps() {

}

void VDUIWarps::Run(const char* title) {
	static int currentNode = 0;

	for (int w = 0; w < mVDSession->getWarpCount(); w++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		//int hue = 0;
		sprintf(buf, "%s##sh%d", mVDSession->getWarpName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDSession->getMixTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) ui::SetTooltip(mVDSession->getWarpName(w).c_str());
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a % 6 != 0)) ui::SameLine();
				if (mVDSession->getWarpAFboIndex(w) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wia%d%d", a, w, a);
				if (ui::Button(buf)) mVDSession->setWarpAFboIndex(w, a);
				sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ui::SameLine();
				if (mVDSession->getWarpBFboIndex(w) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wib%d%d", b, w, b);
				if (ui::Button(buf)) mVDSession->setWarpBFboIndex(w, b);
				sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}

			// crossfade
			float xFade = mVDSession->getWarpCrossfade(w);
			sprintf(buf, "xfade##xf%d", w);
			if (ui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDSession->setWarpCrossfade(w, xFade);
			}
			// nodes
			if (currentNode == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "N##n%d", w);
			if (ui::Button(buf)) {
				if (w == currentNode) {
					// if the same button pressed we hide the nodes
					currentNode = -1;
				}
				else {
					currentNode = w;
					mVDSession->setCurrentEditIndex(w);
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Show nodes");
			ui::SameLine();
			// spout output
			if (mVDSession->getSharedMixIndex() == w && mVDSession->isSharedOutputActive()) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "O##sp%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleSharedOutput(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Spout output");
			ui::SameLine();
			// active
			if (mVDSession->isWarpActive(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "AC##a%d", w);
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "A##a%d", w);
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ui::Button(buf)) {
				mVDSession->toggleWarpActive(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Warp Active");
			ui::SameLine();
			// solo
			if (mVDSession->isWarpSolo(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "SOLO##s%d", w);
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "S##s%d", w);
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ui::Button(buf)) {
				mVDSession->toggleWarpSolo(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Warp Solo");
			ui::SameLine();
			// delete
			if (mVDSession->isWarpDeleted(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "X##x%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleDeleteWarp(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Delete Warp");

			ui::PopID();
			ui::PopItemWidth();
		}

#pragma region Nodes
		if (currentNode == w) {
			// A (left)
			int t = 0;
			int fboIndex = mVDSession->getWarpAFboIndex(currentNode);
			int inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpAFboIndex(currentNode));

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##txa", mVDSession->getInputTextureName(inputTexture).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::Text("WoH %dx%d", mVDSession->getInputTextureOriginalWidth(inputTexture), mVDSession->getInputTextureOriginalHeight(inputTexture));
			ui::Text("WxH %dx%d", mVDSession->getInputTexture(inputTexture)->getWidth(), mVDSession->getInputTexture(inputTexture)->getHeight());
			ui::Text("tx A %s", mVDSession->getInputTextureName(inputTexture).c_str());
			ui::End();

			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##fboa", mVDSession->getFboName(fboIndex).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::Text("WxH %dx%d", mVDSession->getFboRenderedTexture(fboIndex)->getWidth(), mVDSession->getFboRenderedTexture(fboIndex)->getHeight());
			ui::End();

			// mix
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			ui::Begin(mVDSession->getWarpName(currentNode).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getMixTexture(currentNode)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::Text("WxH %dx%d", mVDSession->getMixTexture(currentNode)->getWidth(), mVDSession->getMixTexture(currentNode)->getHeight());
			ui::End();

			// B (right)
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##fbob", mVDSession->getFboName(fboIndex).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::Text("WxH %dx%d", mVDSession->getFboRenderedTexture(fboIndex)->getWidth(), mVDSession->getFboRenderedTexture(fboIndex)->getHeight());
			ui::End();

			t++;
			fboIndex = mVDSession->getWarpBFboIndex(currentNode);
			inputTexture = mVDSession->getFboInputTextureIndex(mVDSession->getWarpBFboIndex(currentNode));

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##txb", mVDSession->getInputTextureName(inputTexture).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::Text("WoH %dx%d", mVDSession->getInputTextureOriginalWidth(inputTexture), mVDSession->getInputTextureOriginalHeight(inputTexture));
			ui::Text("WxH %dx%d", mVDSession->getInputTexture(inputTexture)->getWidth(), mVDSession->getInputTexture(inputTexture)->getHeight());
			ui::Text("tx B %s", mVDSession->getInputTextureName(inputTexture).c_str());
			ui::End();
		}
#pragma endregion Nodes

		ui::End();
	}
}
