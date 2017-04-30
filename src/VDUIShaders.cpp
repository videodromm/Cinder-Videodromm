#include "VDUIShaders.h"

using namespace VideoDromm;

VDUIShaders::VDUIShaders(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	try
	{
		fs::path vertexFile = getAssetPath("") / "passthru.vert";
		if (fs::exists(vertexFile)) {
			mPassthruVextexShaderString = loadString(loadAsset("passthru.vert"));
			CI_LOG_V("passthru.vert loaded");
		}
		else
		{
			CI_LOG_V("passthru.vert does not exist, should quit");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load passthru vertex shader file:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load passthru vertex shader:" + string(e.what()));
	}
}
VDUIShaders::~VDUIShaders() {

}

void VDUIShaders::Run(const char* title) {
	static int shaderToEdit = -1;

	xPos = mVDSettings->uiMargin;
	yPos = mVDSettings->uiYPosRow3;
	for (int s = 0; s < mVDSession->getShadersCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		//int hue = 0;
		sprintf(buf, "%s##sh%d", mVDSession->getShaderName(s).c_str(), s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDSession->getShaderThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) mVDSession->getShaderThumb(s);
			// edit
			if (shaderToEdit == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "E##se%d", s);
			if (ui::Button(buf)){
				if (s == shaderToEdit) {
					// if the same button pressed we hide the editor
					shaderToEdit = -1;
				}
				else{
					shaderToEdit = s;
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Edit shader");
			ui::SameLine();

			if (s > 2)
			{
				sprintf(buf, "X##del%d", s);
				if (ui::Button(buf)) mVDSession->removeShader(s);
				if (ui::IsItemHovered()) ui::SetTooltip("Remove shader");
				ui::SameLine();
			}

			// thumb
			sprintf(buf, "T##st%d", s);
			if (ui::Button(buf)){
				mVDSession->updateShaderThumbFile(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Create thumb");
			ui::SameLine();

            // send with websocket
			sprintf(buf, "WS##ws%d", s);
			if (ui::Button(buf)) mVDSession->sendFragmentShader(s);

			for (unsigned int f = 0; f < mVDSession->getFboListSize(); f++) {
				if (f > 0 && (f % 6 != 0)) ui::SameLine();
				if (mVDSession->getFboFragmentShaderIndex(f) == s) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 1.0f, 0.5f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.0f, 0.8f, 0.8f));
				sprintf(buf, "%d##sf%d", f, s);
				if (ui::Button(buf)) mVDSession->setFboFragmentShaderIndex(f, s);
				if (ui::IsItemHovered()) ui::SetTooltip("Set shader to fbo");
				ui::PopStyleColor(3);
			}
			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
		xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		if (xPos > mVDSettings->mRenderWidth)
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiLargePreviewH + mVDSettings->uiMargin;
		}
		// editor
#pragma region Editor
		if (shaderToEdit == s) {
			mVDSettings->mMsg = "Editing shader " + toString(shaderToEdit) + " name " + mVDSession->getShaderName(shaderToEdit);
			ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW * 3, mVDSettings->uiLargeH), ImGuiSetCond_FirstUseEver);
			sprintf(buf, "Editor - %s - %d##edit%d", mVDSession->getShaderName(shaderToEdit).c_str(), shaderToEdit, shaderToEdit);
			ui::Begin(buf);
			{
				size_t const MAX = 32768; // maximum number of chars
				static char mShaderText[MAX] =
					"uniform vec3 iResolution;\n"
					"uniform vec3 iColor;\n"
					"uniform float iGlobalTime;\n"
					"uniform sampler2D iChannel0;\n"
					"uniform sampler2D iChannel1;\n"
					"uniform sampler2D iChannel2;\n"
					"uniform vec3 spectrum;\n"
					"\n"
					"out vec4 oColor;\n"
					"void main(void) {\n"
					"\tvec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
					"\tvec4 t0 = texture(iChannel0, uv);\n"
					"\tvec4 t1 = texture(iChannel1, uv);\n"
					"\tvec4 t2 = texture(iChannel2, uv);\n"
					"\toColor = vec4(t0.x, t1.y, cos(iGlobalTime), 1.0);\n"
					"}\n";
				// check if shader text needs to be loaded in the editor
				if (mVDSettings->shaderEditIndex != shaderToEdit) {
					/* ptr error
					
					mFboTextureFragmentShaderString = mVDSession->getFragmentShaderString(shaderToEdit);
					mVDSettings->shaderEditIndex = shaderToEdit;
					// delete content
					memset(&mShaderText[0], 0, sizeof(mShaderText));
					// copy content from string
					std::copy(mFboTextureFragmentShaderString.begin(), (mFboTextureFragmentShaderString.size() >= MAX ? mFboTextureFragmentShaderString.begin() + MAX : mFboTextureFragmentShaderString.end()), mShaderText);
				*/
				}

				ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ui::PopStyleVar();
				ui::TextColored(ImColor(255, 0, 0), mError.c_str());
				//ui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "uniform");
				sprintf(buf, "Src - %s##src%d", mVDSession->getShaderName(shaderToEdit).c_str(), shaderToEdit);
				if (ui::InputTextMultiline(buf, mShaderText, IM_ARRAYSIZE(mShaderText), ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_AllowTabInput)) {
					// text changed // TODO height ? mVDSettings->uiYPosRow2 - 200.0f
					CI_LOG_V("text changed");
					try
					{
						aShader = gl::GlslProg::create(mPassthruVextexShaderString, mShaderText);
						// 20161209 problem on Mac aShader->setLabel("live");
						CI_LOG_V("live.frag loaded and compiled");
						mFboTextureFragmentShaderString = mShaderText;
						stringstream sParams;
						sParams << "/*{ \"title\" : \"" << getElapsedSeconds() << "\" }*/ " << mFboTextureFragmentShaderString;
						mVDSession->wsWrite(sParams.str());
						//OK mVDRouter->wsWrite("/*{ \"title\" : \"live\" }*/ " + mFboTextureFragmentShaderString);
						mError = "";
						// compiles, update the shader for display
						mVDSession->setFragmentShaderString(shaderToEdit, mFboTextureFragmentShaderString);
					}
					catch (gl::GlslProgCompileExc &exc)
					{
						mError = string(exc.what());
						CI_LOG_V("unable to load/compile live fragment shader:" + string(exc.what()));
					}
					catch (const std::exception &e)
					{
						mError = string(e.what());
						CI_LOG_V("unable to load live fragment shader:" + string(e.what()));
					}
				}
				else {
					// nothing changed 
				}
			}
			ui::End();
		}
#pragma endregion Editor
	}

}
