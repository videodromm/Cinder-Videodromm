#include "VDUITextures.h"

using namespace VideoDromm;

VDUITextures::VDUITextures(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUITextures::~VDUITextures() {

}

void VDUITextures::Run(const char* title) {

	static int XLeft[64];
	static int YTop[64];
	static int XRight[64];
	static int YBottom[64];
	static bool rnd[64];
	static bool anim[64];

	for (int t = 0; t < mVDSession->getInputTexturesCount(); t++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		int hue = 0;
		sprintf(buf, "%s##s%d", mVDSession->getInputTextureName(t).c_str(), t);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(t);
			ui::Image((void*)mVDSession->getInputTexture(t)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth * 0.7);
			// flip vertically
			mVDSession->isFlipVInputTexture(t) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "FlipV##vd%d", t);
			if (ui::Button(buf)) {
				mVDSession->inputTextureFlipV(t);
			}
			ui::PopStyleColor(3);
			hue++;
			ui::SameLine();
			// flip horizontally
			mVDSession->isFlipHInputTexture(t) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			sprintf(buf, "FlipH##hd%d", t);
			if (ui::Button(buf)) {
				mVDSession->inputTextureFlipH(t);
			}
			ui::PopStyleColor(3);
			hue++;

			/*for (unsigned int f = 0; f < mVDMix->getWarpCount(); f++) {
				if (f > 0) ui::SameLine();
				//int ti = mVDMix->getFboInputTextureIndex(f);
				//CI_LOG_V("fbo" + toString(f) + " t" + toString(t) + " ti" + toString(ti));
				if (mVDMix->getWarpATextureIndex(f) == t) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(t / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(t / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(t / 7.0f, 0.8f, 0.8f));
				sprintf(buf, "%d##fboinputtex%d%d", f, t, f);
				if (ui::Button(buf)) mVDMix->setFboInputTexture(f, t);
				if (ui::IsItemHovered()) ui::SetTooltip("Set input texture for warp");
				ui::PopStyleColor(3);
			}

			sprintf(buf, "WS##s%d", i);
			if (ui::Button(buf))
			{
			sprintf(buf, "IMG=%d.jpg", i);
			//mVDRouter->wsWrite(buf);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Send texture file name via WebSockets");
			*/

			if (mVDSession->isSequence(t) || mVDSession->isMovie(t)) {
				sprintf(buf, "p##s%d", t);
				if (ui::Button(buf))
				{
					mVDSession->togglePlayPause(t);
				}
				if (ui::IsItemHovered()) ui::SetTooltip("Play/Pause");
			}
			if (mVDSession->isSequence(t)) {
				ui::SameLine();
				sprintf(buf, "b##sqs%d", t);
				if (ui::Button(buf))
				{
					mVDSession->syncToBeat(t);
				}
				if (ui::IsItemHovered()) ui::SetTooltip("Sync to beat");

				ui::SameLine();
				sprintf(buf, "r##rs%d", t);
				if (ui::Button(buf))
				{
					mVDSession->reverse(t);
				}
				if (ui::IsItemHovered()) ui::SetTooltip("Reverse");

				if (mVDSession->isLoadingFromDisk(t)) {
					ui::SameLine();
					sprintf(buf, "l##ts%d", t);
					if (ui::Button(buf))
					{
						mVDSession->toggleLoadingFromDisk(t);
					}
					if (ui::IsItemHovered()) ui::SetTooltip("Pause loading from disk");
				}
				speeds[t] = mVDSession->getSpeed(t);
				sprintf(buf, "speed##spd%d", t);
				if (ui::SliderFloat(buf, &speeds[t], 0.0f, 1.0f))
				{
					mVDSession->setSpeed(t, speeds[t]);
				}

				playheadPositions[t] = mVDSession->getPosition(t);
				sprintf(buf, "scrub##srb%d", t);
				if (ui::SliderInt(buf, &playheadPositions[t], 0, mVDSession->getMaxFrame(t)))
				{
					mVDSession->setPlayheadPosition(t, playheadPositions[t]);
				}


			}
			else {
				if (!mVDSession->isMovie(t)) {
					// not a sequence nor video, animate x y...
					XLeft[t] = mVDSession->getInputTextureXLeft(t);
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
						if (XLeft[t] > mVDSession->getInputTextureOriginalWidth(t) - mVDSettings->mFboWidth - 1) {
							xStep = -xStep;
						}
					}
					sprintf(buf, "XL##xl%d", t);
					ui::SliderInt(buf, &XLeft[t], 0, mVDSession->getInputTextureOriginalWidth(t));// CHECK - mVDSettings->mFboWidth
					mVDSession->setInputTextureXLeft(t, XLeft[t]);

					YTop[t] = mVDSession->getInputTextureYTop(t);
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
						if (YTop[t] > mVDSession->getInputTextureOriginalHeight(t) - mVDSettings->mFboHeight - 1) {
							yStep = -yStep;
						}
					}
					sprintf(buf, "YT##yt%d", t);
					ui::SliderInt(buf, &YTop[t], 0, mVDSession->getInputTextureOriginalHeight(t));// - mVDSettings->mFboHeight
					mVDSession->setInputTextureYTop(t, YTop[t]);


					ui::SameLine();
					(mVDSession->getInputTextureLockBounds(t)) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
					ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
					ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
					sprintf(buf, "8##lk%d", t);
					if (ui::Button(buf)) {
						mVDSession->toggleInputTextureLockBounds(t);
					}
					ui::PopStyleColor(3);
					hue++;

					XRight[t] = mVDSession->getInputTextureXRight(t);
					sprintf(buf, "XR##xr%d", t);
					if (ui::SliderInt(buf, &XRight[t], 0, mVDSession->getInputTextureOriginalWidth(t))) {
						mVDSession->setInputTextureXRight(t, XRight[t]);
					}
					ui::SameLine();
					(anim[t]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
					ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
					ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
					sprintf(buf, "ani##r%d", t);
					anim[t] ^= ui::Button(buf);
					ui::PopStyleColor(3);
					hue++;

					YBottom[t] = mVDSession->getInputTextureYBottom(t);
					sprintf(buf, "YB##yb%d", t);
					if (ui::SliderInt(buf, &YBottom[t], 0, mVDSession->getInputTextureOriginalHeight(t))) {
						mVDSession->setInputTextureYBottom(t, YBottom[t]);
					}

					ui::SameLine();
					(rnd[t]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
					ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
					ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
					sprintf(buf, "rnd##r%d", t);
					rnd[t] ^= ui::Button(buf);
					ui::PopStyleColor(3);
					hue++;
				}
			}
			ui::PopItemWidth();
			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
	}

}
