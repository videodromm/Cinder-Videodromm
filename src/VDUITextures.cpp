#include "VDUITextures.h"

using namespace VideoDromm;

VDUITextures::VDUITextures(VDSettingsRef aVDSettings, VDMixRef aVDMix) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;

}
VDUITextures::~VDUITextures() {

}

void VDUITextures::Run(const char* title) {

	//int t = 0;

	static int XLeft[64];
	static int YTop[64];
	static int XRight[64];
	static int YBottom[64];
	static bool rnd[64];
	static bool anim[64];

	for (int t = 0; t < mVDMix->getInputTexturesCount(); t++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##s%d", mVDMix->getInputTextureName(t).c_str(), t);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(t);
			ui::Image((void*)mVDMix->getInputTexture(t)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth/2);

			for (unsigned int f = 0; f < mVDMix->getFboCount(); f++) {
				if (f > 0) ui::SameLine();
				//int ti = mVDMix->getFboInputTextureIndex(f);
				//CI_LOG_V("fbo" + toString(f) + " t" + toString(t) + " ti" + toString(ti));
				if (mVDMix->getFboInputTextureIndex(f) == t) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(t / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(t / 7.0f, 0.8f, 0.8f));
				sprintf(buf, "%d##fboinputtex%d%d", f, t, f);
				if (ui::Button(buf)) mVDMix->setFboInputTexture(f, t);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input texture for fbo");
				ui::PopStyleColor(3);
			}
			XLeft[t] = mVDMix->getInputTextureXLeft(t);
			if (anim[t]) {
				if (rnd[t]) {
					XLeft[t] += xStep * Rand::randBool();
				}
				else {
					XLeft[t] += xStep;
				}
				// check bounds 
				if (XLeft[t] < 1) {
					xStep = -xStep;
				}
				if (XLeft[t] > mVDMix->getInputTextureOriginalWidth(t) - mVDSettings->mFboWidth - 1) {
					xStep = -xStep;
				}
			}
			sprintf(buf, "XL##xl%d", t);
			ui::SliderInt(buf, &XLeft[t], 0, mVDMix->getInputTextureOriginalWidth(t) - mVDSettings->mFboWidth);			
			mVDMix->setInputTextureXLeft(t, XLeft[t]);

			YTop[t] = mVDMix->getInputTextureYTop(t);
			if (anim[t]) {
				if (rnd[t]) {
					YTop[t] += yStep * Rand::randBool();
				}
				else {
					YTop[t] += yStep;
				}
				// check bounds
				if (YTop[t] < 1) {
					yStep = -yStep;
				}
				if (YTop[t] > mVDMix->getInputTextureOriginalHeight(t) - mVDSettings->mFboHeight - 1) {
					yStep = -yStep;
				}
			}
			sprintf(buf, "YT##yt%d", t);
			ui::SliderInt(buf, &YTop[t], 0, mVDMix->getInputTextureOriginalHeight(t) - mVDSettings->mFboHeight);
			mVDMix->setInputTextureYTop(t, YTop[t]);
			

			ui::SameLine();
			(mVDMix->getInputTextureLockBounds(t)) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "8##lk%d", t);
			if (ui::Button(buf)) {
				mVDMix->toggleInputTextureLockBounds(t);
			}
			ui::PopStyleColor(3);
			hue++;

			XRight[t] = mVDMix->getInputTextureXRight(t);
			sprintf(buf, "XR##xr%d", t);
			if (ui::SliderInt(buf, &XRight[t], 0, mVDMix->getInputTextureOriginalWidth(t))) {
				mVDMix->setInputTextureXRight(t, XRight[t]);
			}
			ui::SameLine();
			(anim[t]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "anim##r%d", t);
			anim[t] ^= ui::Button(buf);
			ui::PopStyleColor(3);
			hue++;

			YBottom[t] = mVDMix->getInputTextureYBottom(t);
			sprintf(buf, "YB##yb%d", t);
			if (ui::SliderInt(buf, &YBottom[t], 0, mVDMix->getInputTextureOriginalHeight(t))) {
				mVDMix->setInputTextureYBottom(t, YBottom[t]);
			}

			ui::SameLine();
			(rnd[t]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "rnd##r%d", t);
			rnd[t] ^= ui::Button(buf);
			ui::PopStyleColor(3);
			hue++;

			(mVDMix->getInputTextureTopDown(t)) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "TopDown##td%d", t);
			if (ui::Button(buf)) {
				mVDMix->toggleInputTextureTopDown(t);
			}
			ui::PopStyleColor(3);
			hue++;
			




#pragma region todo
			//BEGIN
			/*sprintf(buf, "WS##s%d", i);
			if (ui::Button(buf))
			{
			sprintf_s(buf, "IMG=%d.jpg", i);
			//mBatchass->wsWrite(buf);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Send texture file name via WebSockets");
			ui::SameLine();
			sprintf(buf, "FV##s%d", i);
			if (ui::Button(buf))
			{
			mVDTextures->flipTexture(i);
			}*/

			//  
			//if (ui::Button("Stop Load")) mVDImageSequences[0]->stopLoading();
			//ui::SameLine();

			/*if (mVDTextures->inputTextureIsSequence(i)) {
			if (!(mVDTextures->inputTextureIsLoadingFromDisk(i))) {
			ui::SameLine();
			sprintf_s(buf, "l##s%d", i);
			if (ui::Button(buf))
			{
			mVDTextures->inputTextureToggleLoadingFromDisk(i);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Pause loading from disk");
			}
			ui::SameLine();
			sprintf_s(buf, "p##s%d", i);
			if (ui::Button(buf))
			{
			mVDTextures->inputTexturePlayPauseSequence(i);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Play/Pause");
			ui::SameLine();
			sprintf_s(buf, "b##s%d", i);
			if (ui::Button(buf))
			{
			mVDTextures->inputTextureSyncToBeatSequence(i);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Sync to beat");
			ui::SameLine();
			sprintf_s(buf, "r##s%d", i);
			if (ui::Button(buf))
			{
			mVDTextures->inputTextureReverseSequence(i);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Reverse");
			playheadPositions[i] = mVDTextures->inputTextureGetPlayheadPosition(i);
			sprintf_s(buf, "p%d##s%d", playheadPositions[i], i);
			if (ui::Button(buf))
			{
			mVDTextures->inputTextureSetPlayheadPosition(i, playheadPositions[i]);
			}

			if (ui::SliderInt("scrub", &playheadPositions[i], 0, mVDTextures->inputTextureGetMaxFrame(i)))
			{
			mVDTextures->inputTextureSetPlayheadPosition(i, playheadPositions[i]);
			}
			speeds[i] = mVDTextures->inputTextureGetSpeed(i);
			if (ui::SliderInt("speed", &speeds[i], 0.0f, 6.0f))
			{
			mVDTextures->inputTextureSetSpeed(i, speeds[i]);
			}

			}*/
			//END
#pragma endregion todo
			ui::PopItemWidth();
			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}

}
