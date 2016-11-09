#include "VDUIWarps.h"

using namespace VideoDromm;

VDUIWarps::VDUIWarps(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
	
}
VDUIWarps::~VDUIWarps() {

}

void VDUIWarps::Run(const char* title) {
	static int showNodes = -1;

	for (int w = 0; w < mVDMix->getWarpCount(); w++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##sh%d", mVDMix->getWarpName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDMix->getWarpTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			// loop on the fbos
			for (unsigned int a = 0; a < mVDMix->getMixFboCount(); a++) {
				if (a > 0) ui::SameLine();
				if (mVDMix->getWarpAFboIndex(w) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wia%d%d", a, w, a);
				if (ui::Button(buf)) mVDMix->setWarpAFboIndex(w, a);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input fbo A");
				ui::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDMix->getMixFboCount(); b++) {
				if (b > 0) ui::SameLine();
				if (mVDMix->getWarpBFboIndex(w) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wib%d%d", b, w, b);
				if (ui::Button(buf)) mVDMix->setWarpBFboIndex(w, b);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input fbo B");
				ui::PopStyleColor(3);
			}
			// edit
			if (showNodes == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "S##se%d", w);
			if (ui::Button(buf)){
				if (w == showNodes) {
					// if the same button pressed we hide the nodes
					showNodes = -1;
				}
				else{
					showNodes = w;
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Show nodes");
			ui::SameLine();
			// crossfade
			float xFade = mVDMix->getWarpCrossfade(w);
			sprintf(buf, "xfade##xf%d", w);
			if (ui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDMix->setWarpCrossfade(w, xFade);
			}
			ui::PopID();
			ui::PopItemWidth();
		}

#pragma region Nodes
		if (showNodes == w) {
			mVDSettings->mMsg = "Warp Node " + toString(showNodes) + " name " + mVDMix->getShaderName(showNodes);
			ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW * 3, mVDSettings->uiLargeH), ImGuiSetCond_FirstUseEver);
			// left
			int t = 0;
			int fboIndex = mVDMix->getLeftFboIndex();

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2));
			ui::Begin("it a", NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDMix->getInputTexture(mVDMix->getFboInputTextureIndex(fboIndex))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::End();
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2));
			/*ui::Begin(mMixes[0]->getFboLabel(fboIndex).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::Image((void*)mMixes[0]->getFboTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			ui::PopItemWidth();
			}
			ui::End();*/
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2));
			ui::Begin("f a", NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDMix->getTexture(1)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::End();


			//right
			t = 0;
			fboIndex = mVDMix->getRightFboIndex();

			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + mVDSettings->uiPreviewH + mVDSettings->uiMargin));
			ui::Begin("it b", NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDMix->getInputTexture(mVDMix->getFboInputTextureIndex(fboIndex))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::End();
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + mVDSettings->uiPreviewH + mVDSettings->uiMargin));
			/*ui::Begin(mMixes[0]->getFboLabel(fboIndex).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::Image((void*)mMixes[0]->getFboTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			ui::PopItemWidth();
			}
			ui::End();*/
			t++;
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH));
			ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2 + mVDSettings->uiPreviewH + mVDSettings->uiMargin));
			ui::Begin("f b", NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDMix->getTexture(2)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			ui::End();
		}
#pragma endregion Nodes

		ui::End();
	}
}
