#include "VDUIAnimation.h"

using namespace VideoDromm;

VDUIAnimation::VDUIAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	// zoom
	minZoom = getMinUniformValueByIndex(12);
	maxZoom = getMaxUniformValueByIndex(12);
	for (int c = 0; c < 128; c++)
	{
		localValues[c] = mVDSession->getFloatUniformValueByIndex(c);
	}
}
VDUIAnimation::~VDUIAnimation() {

}
float VDUIAnimation::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIAnimation::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
bool VDUIAnimation::getBoolValue(unsigned int aCtrl) {
	return mVDSession->getBoolUniformValueByIndex(aCtrl);
}
void VDUIAnimation::toggleValue(unsigned int aCtrl) {
	mVDSession->toggleValue(aCtrl);
}
void VDUIAnimation::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIAnimation::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIAnimation::toggleBass(unsigned int aCtrl) {
	mVDSession->toggleBass(aCtrl);
}
void VDUIAnimation::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIAnimation::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIAnimation::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIAnimation::Run(const char* title) {

	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH + 100), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	ImGui::Begin("Animation", NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		if (ImGui::CollapsingHeader("Effects", NULL, true, true))
		{
			int hue = 0;

			ctrl = mVDSettings->IGLITCH;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("glitch")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

			ctrl = mVDSettings->ITOGGLE;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("toggle")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

			ctrl = mVDSettings->IINVERT;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("invert")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;

			(mVDSettings->iGreyScale) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			mVDSettings->iGreyScale ^= ImGui::Button("greyscale");
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

			if (ImGui::Button("blackout"))
			{
				setValue(1, 0.0);
				setValue(2, 0.0);
				setValue(3, 0.0);
				setValue(4, 0.0);
			}
		}
		if (ImGui::CollapsingHeader("Animation", NULL, true, true))
		{
			// iChromatic
			ctrl = mVDSettings->ICHROMATIC;
			if (ImGui::Button("a##chromatic")) {
				toggleAuto(ctrl);
			}
			ImGui::SameLine();
			if (ImGui::Button("f##chromatic")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##chromatic")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##chromatic")) {
				resetAutoAnimation(ctrl);
			}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("chromatic", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				mVDSession->setFloatUniformValueByIndex(ctrl, localValues[ctrl]);
			}

			// ratio
			ctrl = mVDSettings->IRATIO;
			if (ImGui::Button("a##ratio")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##ratio")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##ratio")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##ratio")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("ratio/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}

			// sobel
			ctrl = mVDSettings->ISOBEL;
			if (ImGui::Button("a##sobel")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##sobel")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##sobel")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##sobel")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("sobel/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// exposure
			ctrl = mVDSettings->IEXPOSURE;
			if (ImGui::Button("a##exposure")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##exposure")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##exposure")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##exposure")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("exposure", &localValues[ctrl], 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// zoom
			ctrl = 12;
			if (ImGui::Button("a##zoom"))
			{
				toggleAuto(ctrl);
			}
			ImGui::SameLine();
			if (ImGui::Button("f##zoom")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##zoom")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##zoom")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("zoom", &localValues[ctrl], 0.1f, minZoom, maxZoom))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			ImGui::DragFloat("minzm", &minZoom, 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
			ImGui::SameLine();
			ImGui::DragFloat("maxzm", &maxZoom, 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));

			// rotation speed 
			ctrl = 9;
			if (ImGui::Button("a##rotationspeed")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##rotationspeed")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##rotationspeed")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("rotationSpeed", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// badTv
			ctrl = 8;
			if (ImGui::Button("a##badtv")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##badtv")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##badtv")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("badTv", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// param1
			ctrl = mVDSettings->IPARAM1;
			if (ImGui::Button("x##param1")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("param1/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// param2
			ctrl = mVDSettings->IPARAM2;
			if (ImGui::Button("x##param2")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("param2/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			sprintf(buf, "XorY");
			mVDSettings->iXorY ^= ImGui::Button(buf);

			// steps
			ctrl = mVDSettings->ISTEPS;
			if (ImGui::Button("x##steps")) { localValues[ctrl] = 16.0f; setValue(ctrl, localValues[ctrl]);}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("steps", &localValues[ctrl], 1.0f, 128.0f))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// pixelate
			ctrl = 15;
			if (ImGui::Button("x##pixelate")) { localValues[ctrl] = 1.0f; setValue(ctrl, localValues[ctrl]);}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("pixelate", &localValues[ctrl], 0.01f, 1.0f))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// trixels
			ctrl = 16;
			if (ImGui::Button("x##trixels")) { localValues[ctrl] = 0.0f; setValue(ctrl, localValues[ctrl]);}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("trixels", &localValues[ctrl], 0.00f, 1.0f))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			ImGui::SliderFloat("ABP Bend", &mVDSettings->mBend, -20.0f, 20.0f);
		}
		ImGui::PopItemWidth();
	}
	ImGui::End();
}
