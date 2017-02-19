#include "VDUIFbos.h"

using namespace VideoDromm;

VDUIFbos::VDUIFbos(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIFbos::~VDUIFbos() {

}

void VDUIFbos::Run(const char* title) {
	// fbos
	xPos = mVDSettings->uiMargin + mVDSettings->uiXPosCol1;
	yPos = mVDSettings->uiYPosRow2;

	for (unsigned int f = 0; f < mVDSession->getFboListSize(); f++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		sprintf(buf, "%s##fbolbl%d", mVDSession->getFboName(f).c_str(), f);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushID(f);
			ui::Image((void*)mVDSession->getFboRenderedTexture(f)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) mVDSession->getFboTexture(f);
			for (unsigned int t = 0; t < mVDSession->getInputTexturesCount(); t++) {
				if (t > 0 && (t % 6 != 0)) ui::SameLine();
				if (mVDSession->getFboInputTextureIndex(f) == t) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				sprintf(buf, "%d##fboit%d%d", t, f, t);
				if (ui::Button(buf)) mVDSession->setFboInputTexture(f, t);

				sprintf(buf, "Set input texture to %s", mVDSession->getInputTextureName(t).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(1);
			}
			if (mVDSession->isFboFlipV(f)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(f / 7.0f, 1.0f, 1.0f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(f / 7.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(f / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(f / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "FlipV##fboflipv%d", f);
			if (ui::Button(buf)) mVDSession->fboFlipV(f);
			ui::PopStyleColor(3);
			ui::SameLine();
			sprintf(buf, "T##fboupd%d", f);
			if (ui::Button(buf)) mVDSession->updateShaderThumbFile(f);
			ui::Text("wh %dx%d", mVDSession->getFboRenderedTexture(f)->getWidth(), mVDSession->getFboRenderedTexture(f)->getHeight());
			static int fb = mVDSession->getFeedbackFrames(f);
			sprintf(buf, "feedback##fbofbk%d", f);
			if (ui::SliderInt(buf, &fb, 0, 9))
			{
				mVDSession->setFeedbackFrames(f, fb);
			}
			// next UI position		
			xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;

			if (xPos > mVDSettings->mRenderWidth - mVDSettings->uiXPosCol1)
			{
				xPos = mVDSettings->uiMargin + mVDSettings->uiXPosCol1;
				yPos += mVDSettings->uiLargePreviewH + mVDSettings->uiMargin;
			}
			ui::PopID();
		}
		ui::End();
	}
	// mix fbos
	for (unsigned int m = 0; m < mVDSession->getMixFbosCount(); m++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2((m * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		// TODO ui::Begin(mVDMix->getFboLabel(m).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		// heap problem: sprintf(buf, "%s##ml%d", mVDSession->getMixFboName(m).c_str(), m);
		sprintf(buf, "%d##ml%d", m, m);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushID(m+100);// unique id
			
			ui::Image((void*)mVDSession->getMixTexture(m)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a % 6 != 0)) ui::SameLine();
				if (mVDSession->getWarpAFboIndex(m) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				//ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				//ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wia%d%d", a, m, a);
				if (ui::Button(buf)) mVDSession->setWarpAFboIndex(m, a);
				sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(1);
				//::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ui::SameLine();
				if (mVDSession->getWarpBFboIndex(m) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				//ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				//ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##wib%d%d", b, m, b);
				if (ui::Button(buf)) mVDSession->setWarpBFboIndex(m, b);
				sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(1);
				//ui::PopStyleColor(3);
			}
			ui::Text("mixwh %dx%d", mVDSession->getMixTexture(m)->getWidth(), mVDSession->getMixTexture(m)->getHeight());
			
			ui::PopID();
		}
		ui::End();
	}
}
