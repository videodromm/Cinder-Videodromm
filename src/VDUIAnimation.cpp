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

	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	ImGui::Begin("Animation", NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		if (ImGui::CollapsingHeader("Effects", NULL, true, true))
		{
			int hue = 0;

			ctrl = 45;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("glitch")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

			ctrl = 46;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("toggle")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

			ctrl = 47;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("vignette")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;

			ctrl = 48;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("invert")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ImGui::SameLine();

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
			ctrl = 17;
			if (ImGui::Button("a##chromatic")) {
				toggleAuto(ctrl);
			}
			ImGui::SameLine();
			if (ImGui::Button("t##chromatic")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##chromatic")) {
				resetAutoAnimation(ctrl);
			}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("chromatic/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				mVDSession->setFloatUniformValueByIndex(ctrl, localValues[ctrl]);
			}

			// ratio
			ctrl = 11;
			if (ImGui::Button("a##ratio")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##ratio")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##ratio")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("ratio/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// exposure
			ctrl = 14;
			if (ImGui::Button("a##exposure")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##exposure")) { toggleTempo(ctrl); }
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
			if (ImGui::Button("t##zoom")) { toggleTempo(ctrl); }
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
			if (ImGui::Button("t##rotationspeed")) { toggleTempo(ctrl); }
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
			if (ImGui::Button("t##badtv")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##badtv")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("badTv", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// param1
			if (ImGui::Button("x##param1")) { mVDSettings->iParam1 = 1.0f; }
			ImGui::SameLine();
			if (ImGui::SliderFloat("param1/min/max", &mVDSettings->iParam1, 0.01f, 100.0f))
			{
			}
			// param2
			if (ImGui::Button("x##param2")) { mVDSettings->iParam2 = 1.0f; }
			ImGui::SameLine();
			if (ImGui::SliderFloat("param2/min/max", &mVDSettings->iParam2, 0.01f, 100.0f))
			{
			}
			sprintf(buf, "XorY");
			mVDSettings->iXorY ^= ImGui::Button(buf);

			// steps
			ctrl = 10;
			if (ImGui::Button("x##steps")) { localValues[ctrl] = 16.0f; }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("steps", &localValues[ctrl], 1.0f, 128.0f))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// pixelate
			ctrl = 15;
			if (ImGui::Button("x##pixelate")) { localValues[ctrl] = 1.0f; }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("pixelate", &localValues[ctrl], 0.01f, 1.0f))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// trixels
			ctrl = 16;
			if (ImGui::Button("x##trixels")) { localValues[ctrl] = 0.0f; }
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
