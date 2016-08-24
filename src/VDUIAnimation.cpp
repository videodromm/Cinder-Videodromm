#include "VDUIAnimation.h"

using namespace VideoDromm;

VDUIAnimation::VDUIAnimation(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
	mVDAnimation = aVDAnimation;
}
VDUIAnimation::~VDUIAnimation() {

}

void VDUIAnimation::Run(const char* title) {
	int ctrl;
	stringstream aParams;
	aParams << "{\"params\" :[{\"name\" : 0,\"value\" : " << getElapsedFrames() << "}"; // TimeStamp

	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));
	ui::Begin("Animation", NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		if (ui::CollapsingHeader("Effects", NULL, true, true))
		{
			int hue = 0;

			(mVDSettings->iRepeat) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			mVDSettings->iRepeat ^= ui::Button("repeat");
			ui::PopStyleColor(3);
			hue++;
			ui::SameLine();

			ctrl = 45;
			(mVDAnimation->controlValues[ctrl]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ui::Button("glitch")) {
				mVDAnimation->controlValues[ctrl] = !mVDAnimation->controlValues[ctrl];
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			ui::PopStyleColor(3);
			hue++;
			ui::SameLine();

			ctrl = 46;
			(mVDAnimation->controlValues[ctrl]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ui::Button("toggle")) {
				mVDAnimation->controlValues[ctrl] = !mVDAnimation->controlValues[ctrl];
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			ui::PopStyleColor(3);
			hue++;

			ctrl = 47;
			(mVDAnimation->controlValues[47]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ui::Button("vignette")) {
				mVDAnimation->controlValues[ctrl] = !mVDAnimation->controlValues[ctrl];
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			ui::PopStyleColor(3);
			hue++;
			ui::SameLine();

			ctrl = 48;
			(mVDAnimation->controlValues[48]) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ui::Button("invert")) {
				mVDAnimation->controlValues[ctrl] = !mVDAnimation->controlValues[ctrl];
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			ui::PopStyleColor(3);
			hue++;
			ui::SameLine();

			(mVDSettings->iGreyScale) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			mVDSettings->iGreyScale ^= ui::Button("greyscale");
			ui::PopStyleColor(3);
			hue++;

			if (ui::Button("blackout"))
			{
				mVDAnimation->controlValues[1] = mVDAnimation->controlValues[2] = mVDAnimation->controlValues[3] = mVDAnimation->controlValues[4] = 0.0;
				mVDAnimation->controlValues[5] = mVDAnimation->controlValues[6] = mVDAnimation->controlValues[7] = mVDAnimation->controlValues[8] = 0.0;
			}
			(mVDMix->isFboUsed()) ? ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.1f, 0.1f));
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ui::Button("use fbo")) {
				mVDMix->toggleFboUsed();
			}
			ui::PopStyleColor(3);
			hue++;
		}
		if (ui::CollapsingHeader("Animation", NULL, true, true))
		{
			//ui::SliderInt("UI Zoom", &mVDSettings->mUIZoom, 1, 8);

			// iChromatic
			ctrl = 10;
			if (ui::Button("a##chromatic")) {
				mVDAnimation->lockChromatic();
			}
			ui::SameLine();
			if (ui::Button("t##chromatic")) { mVDAnimation->tempoChromatic(); }
			ui::SameLine();
			if (ui::Button("x##chromatic")) {
				mVDAnimation->resetChromatic();
			}
			ui::SameLine();
			if (ui::SliderFloat("chromatic/min/max", &mVDAnimation->controlValues[ctrl], mVDAnimation->minChromatic, mVDAnimation->maxChromatic))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}

			// ratio
			ctrl = 11;
			if (ui::Button("a##ratio")) { mVDAnimation->lockRatio(); }
			ui::SameLine();
			if (ui::Button("t##ratio")) { mVDAnimation->tempoRatio(); }
			ui::SameLine();
			if (ui::Button("x##ratio")) { mVDAnimation->resetRatio(); }
			ui::SameLine();
			if (ui::SliderFloat("ratio/min/max", &mVDAnimation->controlValues[ctrl], mVDAnimation->minRatio, mVDAnimation->maxRatio))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// exposure
			ctrl = 14;
			if (ui::Button("a##exposure")) { mVDAnimation->lockExposure(); }
			ui::SameLine();
			if (ui::Button("t##exposure")) { mVDAnimation->tempoExposure(); }
			ui::SameLine();
			if (ui::Button("x##exposure")) { mVDAnimation->resetExposure(); }
			ui::SameLine();
			if (ui::DragFloat("exposure", &mVDAnimation->controlValues[ctrl], 0.1f, mVDAnimation->minExposure, mVDAnimation->maxExposure))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
				mVDAnimation->setExposure(mVDAnimation->controlValues[ctrl]);
			}
			// zoom
			ctrl = 22;
			if (ui::Button("a##zoom"))
			{
				mVDAnimation->lockZoom();
			}
			ui::SameLine();
			if (ui::Button("t##zoom")) { mVDAnimation->tempoZoom(); }
			ui::SameLine();
			if (ui::Button("x##zoom")) { mVDAnimation->resetZoom(); }
			ui::SameLine();
			if (ui::DragFloat("zoom", &mVDAnimation->controlValues[ctrl], 0.1f, mVDAnimation->minZoom, mVDAnimation->maxZoom))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// z position
			ctrl = 9;
			if (ui::Button("a##zpos")) { mVDAnimation->lockZPos(); }
			ui::SameLine();
			if (ui::Button("t##zpos")) { mVDAnimation->tempoZPos(); }
			ui::SameLine();
			if (ui::Button("x##zpos")) { mVDAnimation->resetZPos(); }
			ui::SameLine();
			if (ui::SliderFloat("zPosition", &mVDAnimation->controlValues[ctrl], mVDAnimation->minZPos, mVDAnimation->maxZPos))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}

			// rotation speed 
			ctrl = 19;
			if (ui::Button("a##rotationspeed")) { mVDAnimation->lockRotationSpeed(); }
			ui::SameLine();
			if (ui::Button("t##rotationspeed")) { mVDAnimation->tempoRotationSpeed(); }
			ui::SameLine();
			if (ui::Button("x##rotationspeed")) { mVDAnimation->resetRotationSpeed(); }
			ui::SameLine();
			if (ui::DragFloat("rotationSpeed", &mVDAnimation->controlValues[ctrl], 0.01f, mVDAnimation->minRotationSpeed, mVDAnimation->maxRotationSpeed))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// badTv
			/*if (ui::Button("x##badtv")) { mVDSettings->iBadTv = 0.0f; }
			ui::SameLine();
			if (ui::SliderFloat("badTv/min/max", &mVDSettings->iBadTv, 0.0f, 5.0f))
			{
			}*/
			// param1
			if (ui::Button("x##param1")) { mVDSettings->iParam1 = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("param1/min/max", &mVDSettings->iParam1, 0.01f, 100.0f))
			{
			}
			// param2
			if (ui::Button("x##param2")) { mVDSettings->iParam2 = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("param2/min/max", &mVDSettings->iParam2, 0.01f, 100.0f))
			{
			}
			sprintf(buf, "XorY");
			mVDSettings->iXorY ^= ui::Button(buf);

			// steps
			ctrl = 20;
			if (ui::Button("x##steps")) { mVDAnimation->controlValues[ctrl] = 16.0f; }
			ui::SameLine();
			if (ui::SliderFloat("steps", &mVDAnimation->controlValues[ctrl], 1.0f, 128.0f))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// pixelate
			ctrl = 15;
			if (ui::Button("x##pixelate")) { mVDAnimation->controlValues[ctrl] = 1.0f; }
			ui::SameLine();
			if (ui::SliderFloat("pixelate", &mVDAnimation->controlValues[ctrl], 0.01f, 1.0f))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// trixels
			ctrl = 16;
			if (ui::Button("x##trixels")) { mVDAnimation->controlValues[ctrl] = 0.0f; }
			ui::SameLine();
			if (ui::SliderFloat("trixels", &mVDAnimation->controlValues[ctrl], 0.00f, 1.0f))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}
			// grid
			ctrl = 17;
			if (ui::Button("x##grid")) { mVDAnimation->controlValues[ctrl] = 0.0f; }
			ui::SameLine();
			if (ui::SliderFloat("grid", &mVDAnimation->controlValues[ctrl], 0.00f, 60.0f))
			{
				aParams << ",{\"name\" : " << ctrl << ",\"value\" : " << mVDAnimation->controlValues[ctrl] << "}";
			}


		}


		if (ui::CollapsingHeader("Camera", NULL, true, true))
		{
			ui::SliderFloat("Pos.x", &mVDSettings->mRenderPosXY.x, 0.0f, mVDSettings->mRenderWidth);
			ui::SliderFloat("Pos.y", &mVDSettings->mRenderPosXY.y, 0.0f, mVDSettings->mRenderHeight);
			float eyeZ = mVDSettings->mCamera.getEyePoint().z;
			if (ui::SliderFloat("Eye.z", &eyeZ, -500.0f, 1.0f))
			{
				vec3 eye = mVDSettings->mCamera.getEyePoint();
				eye.z = eyeZ;
				mVDSettings->mCamera.setEyePoint(eye);
			}
			ui::SliderFloat("ABP Bend", &mVDSettings->mBend, -20.0f, 20.0f);

		}
		ui::PopItemWidth();
		// finish message and send if a value changed
		aParams << "]}";
		string strAParams = aParams.str();
		if (strAParams.length() > 60)
		{
			mVDRouter->sendJSON(strAParams);
		}
	}
	ui::End();
}
