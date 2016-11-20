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

	for (unsigned int f = 0; f < mVDSession->getFboListSize(); f++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH*1.5));
		ui::SetNextWindowPos(ImVec2((f * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2));
		sprintf(buf, "%s##fbolbl%d", mVDSession->getFboLabel(f).c_str(), f);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushID(f);
			ui::Image((void*)mVDSession->getFboRenderedTexture(f)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) mVDSession->getFboTexture(f);
			for (unsigned int t = 0; t < mVDSession->getInputTexturesCount(); t++) {
				if (t > 0) ui::SameLine();
				if (mVDSession->getFboInputTextureIndex(f) == t) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(t / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(t / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##fboit%d%d", t, f, t);
				if (ui::Button(buf)) mVDSession->setFboInputTexture(f, t);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input texture");
				ui::PopStyleColor(3);
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
			sprintf(buf, "U##fboupd%d", f);
			if (ui::Button(buf)) mVDSession->getFboTexture(f);
			ui::Text("wh %dx%d", mVDSession->getFboRenderedTexture(f)->getWidth(), mVDSession->getFboRenderedTexture(f)->getHeight());

			ui::PopID();
		}
		ui::End();
	}
	// mix fbos
	for (unsigned int m = 0; m < mVDSession->getMixFbosCount(); m++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiPreviewH*1.2));
		//ui::SetNextWindowPos(ImVec2((f * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		ui::SetNextWindowPos(ImVec2((m * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));// + mVDSettings->uiPreviewH*1.3));
		// TODO ui::Begin(mVDMix->getFboLabel(m).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		sprintf(buf, "%s##mixfbolbl%d", mVDSession->getMixFboLabel(m).c_str(), m);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushID(m);
			ui::Image((void*)mVDSession->getMixTexture(m)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			for (unsigned int it = 0; it < mVDSession->getInputTexturesCount(); it++) {
				if (it > 0) ui::SameLine();
				if (mVDSession->getFboInputTextureIndex(m) == it) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(it / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(it / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(it / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(it / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##mixfboit%d%d", it, m, it);
				if (ui::Button(buf)) mVDSession->setFboInputTexture(m, it);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input texture");
				ui::PopStyleColor(3);
			}
			ui::Text("mixwh %dx%d", mVDSession->getMixTexture(m)->getWidth(), mVDSession->getMixTexture(m)->getHeight());

			ui::PopID();
		}
		ui::End();
	}
}
