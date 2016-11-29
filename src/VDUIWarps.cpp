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
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##sh%d", mVDSession->getWarpName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDSession->getMixTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) ui::SetTooltip(mVDSession->getWarpName(w).c_str());
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0) ui::SameLine();
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
				if (b > 0) ui::SameLine();
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
			sprintf(buf, "S##se%d", w);
			if (ui::Button(buf)){
				if (w == currentNode) {
					// if the same button pressed we hide the nodes
					currentNode = -1;
				}
				else{
					currentNode = w;
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Show nodes");
			ui::SameLine();
			// other button 
			ui::PopID();
			ui::PopItemWidth();
		}

#pragma region Nodes
		if (currentNode == w) {
			mVDSettings->mMsg = "Warp " + toString(currentNode) + " name " + mVDSession->getWarpName(currentNode);
			// A (left)
			int t = 0;
			int fboIndex = mVDSession->getWarpAFboIndex(currentNode);

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2));
			sprintf(buf, "txa %s", mVDSession->getInputTextureName(mVDSession->getFboInputTextureIndex(mVDSession->getWarpAFboIndex(currentNode))).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getInputTexture(mVDSession->getFboInputTextureIndex(mVDSession->getWarpAFboIndex(currentNode)))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::End();
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2));
			sprintf(buf, "%s", mVDSession->getFboName(fboIndex).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getFboTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);

			ui::End();

			// B (right)
			t = 0;
			fboIndex = mVDSession->getWarpBFboIndex(currentNode);

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + mVDSettings->uiPreviewH + mVDSettings->uiMargin));
			sprintf(buf, "txb %s", mVDSession->getInputTextureName(mVDSession->getFboInputTextureIndex(mVDSession->getWarpBFboIndex(currentNode))).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getInputTexture(mVDSession->getFboInputTextureIndex(mVDSession->getWarpBFboIndex(currentNode)))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);

			ui::End();
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + mVDSettings->uiPreviewH + mVDSettings->uiMargin));
			sprintf(buf, "%s", mVDSession->getFboName(fboIndex).c_str());
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getFboTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			ui::End();
			
			// mix
			t = 0;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + 2*(mVDSettings->uiPreviewH + mVDSettings->uiMargin)));
			ui::Begin(mVDSession->getWarpName(currentNode).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getMixTexture(currentNode)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				// ui::Image((void*)mVDSession->getRenderTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::End();
		
		}
#pragma endregion Nodes

		ui::End();
	}
}
