#include "VDUIColor.h"

using namespace VideoDromm;

VDUIColor::VDUIColor(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	for (int c = 0; c < 28; c++)
	{
		localValues[c] = 1.0f;
	}

}
VDUIColor::~VDUIColor() {

}
float VDUIColor::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIColor::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIColor::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIColor::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIColor::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIColor::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIColor::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIColor::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ImGui::Begin("Color");
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// foreground color
		color[0] = getValue(1);
		color[1] = getValue(2);
		color[2] = getValue(3);
		color[3] = getValue(4);
		ImGui::ColorEdit4("f", color);
		for (int i = 0; i < 4; i++)
		{
			if (getValue(i + 1) != color[i])
			{
				setValue(i + 1, color[i]);
			}
		}

		// background color
		backcolor[0] = getValue(19);
		backcolor[1] = getValue(20);
		backcolor[2] = getValue(21);
		ImGui::ColorEdit3("g", backcolor);
		for (int i = 0; i < 3; i++)
		{
			if (getValue(i + 19) != backcolor[i])
			{
				setValue(i + 19, backcolor[i]);
			}

		}
		// color multipliers
		// red x
		ctrl = 5;
		if (ImGui::Button("a##redx")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##redx")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##redx")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("red x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, localValues[ctrl]);
		}
		// green x
		ctrl = 6;
		if (ImGui::Button("a##greenx")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##greenx")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##greenx")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("green x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, localValues[ctrl]);
		}
		// blue x
		ctrl = 7;
		if (ImGui::Button("a##bluex")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##bluex")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##bluex")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("blue x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, localValues[ctrl]);
		}
		// iContour
		ctrl = 26;
		if (ImGui::Button("a##contour")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##contour")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##contour")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);

		ImGui::PopItemWidth();
	}
	ImGui::End();
}
