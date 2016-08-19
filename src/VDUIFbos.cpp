#include "VDUIFbos.h"

using namespace VideoDromm;

VDUIFbos::VDUIFbos(VDSettingsRef aVDSettings, VDMixRef aVDMix) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
}
VDUIFbos::~VDUIFbos() {
	
}

void VDUIFbos::Run(const char* title) {

	for (unsigned int f = 0; f < mVDMix->getFboCount(); f++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((f * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		// TODO ui::Begin(mVDMix->getFboLabel(f).c_str(), NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		sprintf(buf, "%s##fbolbl%d", mVDMix->getFboLabel(f).c_str(),f);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushID(f);
			ui::Image((void*)mVDMix->getFboTexture(f)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			for (unsigned int t = 0; t < mVDMix->getInputTexturesCount(); t++) {
				if (t > 0) ui::SameLine();
				if (mVDMix->getFboInputTextureIndex(f) == t) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(t / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(t / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##fboit%d%d", t, f, t);
				if (ui::Button(buf)) mVDMix->setFboInputTexture(f, t);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input texture");
				ui::PopStyleColor(3);
			}
			// left
			if (mVDMix->getLeftFboIndex() == f) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.0f, 0.8f, 0.8f));
			sprintf(buf, "L##s%d", f);
			if (ui::Button(buf)){
				mVDMix->setLeftFboIndex(f);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Set fbo to left");
			ui::PopStyleColor(3);
			//ui::NextColumn();
			ui::SameLine();
			// right
			if (mVDMix->getRightFboIndex() == f) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.3f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.3f, 0.8f, 0.8f));
			sprintf(buf, "R##s%d", f);
			if (ui::Button(buf)){
				mVDMix->setRightFboIndex(f);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Set fbo to right");
			ui::PopStyleColor(3);
			ui::Text("wh %dx%d", mVDMix->getFboTexture(f)->getWidth(), mVDMix->getFboTexture(f)->getHeight());


			ui::PopID();
		}
		ui::End();
	}
}
