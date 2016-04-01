#include "VDShaders.h"

using namespace VideoDromm;

VDShaders::VDShaders(VDSettingsRef aVDSettings)
{
	mVDSettings = aVDSettings;

	CI_LOG_V("VDShaders constructor");
	liveError = true;
	isLiveShaderSetup = false;
	previousFileName = "0";
	currentFileName = "0";
	mShaderIndex = 4;
	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;
	validVert = false;
	validFrag = false;
	try
	{
		fs::path vertexFile = getAssetPath("") / "passthru.vert";
		if (fs::exists(vertexFile)) {
			mPassthruVextexShaderString = loadString(loadAsset("passthru.vert"));
		}
		else
		{
			CI_LOG_V("passthru.vert does not exist, should quit");
		}
		fs::path fragFile = getAssetPath("") / "passthru.frag";
		if (fs::exists(fragFile)) {
			mPassthruFragmentShaderString = loadString(loadAsset("passthru.frag"));
		}
		else
		{
			CI_LOG_V("passthru.frag does not exist, should quit");
		}
		mPassThruShader = gl::GlslProg::create(mPassthruVextexShaderString, mPassthruFragmentShaderString);
		mWarpShader = gl::GlslProg::create(mPassthruVextexShaderString, mPassthruFragmentShaderString);
		validVert = true;
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile passthru shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load passthru shader:" + string(e.what()));
	}
	//load mix shader
	try
	{
		fs::path mixFragFile = getAssetPath("") / "mix.frag";
		if (fs::exists(mixFragFile))
		{
			mMixShader = gl::GlslProg::create(loadAsset("passthru.vert"), loadFile(mixFragFile));
		}
		else
		{
			CI_LOG_V("mix.frag does not exist, should quit");

		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load shader:" + string(e.what()));
	}

	// shadertoy include
	//vs = loadString(loadAsset("live.vert"));
	shaderInclude = loadString(loadAsset("shadertoy.inc"));


	// live frag file
	liveFragFile = getAssetPath("") / "live.frag";
	if (fs::exists(liveFragFile))
	{
		// Load our shader and test if it is correctly compiled
		try
		{
			mVDSettings->mShaderToLoad = loadString(loadAsset("live.frag"));
			mLiveShader = gl::GlslProg::create(mPassthruVextexShaderString, mVDSettings->mShaderToLoad);
			liveError = false;
		}
		catch (gl::GlslProgCompileExc exc){
			console() << exc.what() << endl;
		}
	}
	else
	{
		CI_LOG_V("live.frag does not exist");
	}
	//fileName = "default.frag";
	fs::path localFile; //= getAssetPath("") / "shaders" / fileName;
	// load 0.glsl to 4.glsl from assets folder
	for (size_t m = 0; m < 5; m++)
	{
		fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / fileName;
		mFragFileName = fileName;
		mFragFile = localFile.string();
		loadPixelFragmentShader(localFile.string());
	}
	// load 5.glsl to 15.glsl from assets folder
	for (size_t m = 5; m < mVDSettings->MAX; m++)
	{
		fileName = toString(m) + ".glsl";
		localFile = getAssetPath("") / mVDSettings->mAssetsPath / fileName;
		mFragFileName = fileName;
		mFragFile = localFile.string();
		loadPixelFragmentShader(localFile.string());
	}
	// init with passthru shader if something goes wrong	
	for (size_t m = mFragmentShaders.size(); m < 8; m++)
	{
		Shada newShader;
		newShader.shader = gl::GlslProg::create(mPassthruVextexShaderString, mPassthruFragmentShaderString);
		newShader.name = "passthru.frag";
		newShader.active = true;
		mFragmentShaders.push_back(newShader);
	}
	mCurrentPreviewShader = 0;
	mCurrentRenderShader = 0;

}

void VDShaders::setupLiveShader()
{
	if (!isLiveShaderSetup)
	{
		//load live shader
		try
		{
			//wd::watch(liveFragFile, [this](const fs::path &livePath)
			/*wd::watch(liveFragFile, static_cast < function < void(const fs::path &frag)>>([this](const fs::path & liveFragFile)
			{
			//this->loadLiveShader();

			}));*/
			isLiveShaderSetup = true;

		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile shader:" + string(exc.what()));
		}
		catch (const std::exception &e)
		{

			mError = string(e.what());
			CI_LOG_V("unable to load shader:" + string(e.what()));
		}
		if (liveError)
		{
			mVDSettings->mMsg = mError;
			mVDSettings->newMsg = true;
			// revert to mix.frag, TODO better quit if mix.frag does not exit
			fs::path mixFragFile = getAssetPath("") / "mix.frag";
			mLiveShader = gl::GlslProg::create(loadAsset("live.vert"), loadFile(mixFragFile));
		}

	}
}
string VDShaders::loadLiveShader(string frag)
{
	string rtn = "";
	// Load our shader and test if it is correctly compiled
	liveError = true;
	try
	{
		mLiveShader = gl::GlslProg::create(mPassthruVextexShaderString, frag.c_str());
		liveError = false;
	}
	catch (gl::GlslProgCompileExc exc)
	{
		rtn = string(exc.what());
	}
	return rtn;
}

string VDShaders::getFragStringFromFile(string fileName)
{
	string rtn = "";
	try
	{
		rtn = loadString(loadAsset(fileName));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(fileName + " unable to load string from file:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}
	return rtn;
}
string VDShaders::getFragError()
{
	return mError;
}

VDShaders::~VDShaders()
{
	CI_LOG_V("VDShaders destructor");
}

string VDShaders::getFileName(string aFilePath)
{
	string fName;
	if (aFilePath.find_last_of("\\") != std::string::npos)
	{
		fName = aFilePath.substr(aFilePath.find_last_of("\\") + 1);
	}
	else
	{
		fName = aFilePath;
	}
	return fName;
}
string VDShaders::getNewFragFileName(string aFilePath)
{
	return getFileName(aFilePath) + ".frag";
}

void VDShaders::renderPreviewShader()
{
	mVDSettings->controlValues[18] = mVDSettings->controlValues[21];
	mFragmentShaders[mCurrentRenderShader] = mFragmentShaders[mCurrentPreviewShader];
}
void VDShaders::removePixelFragmentShaderAtIndex(int index)
{
	mFragmentShaders[index].shader = mPassThruShader;
	mFragmentShaders[index].name = "passthru";
	mFragmentShaders[index].active = false;

}
int VDShaders::loadPixelFragmentShaderAtIndex(string aFilePath, int index)
{
	int rtn = -1;
	// reset 
	mVDSettings->iFade = false;
	mVDSettings->controlValues[22] = 1.0f;
	if (index > mFragmentShaders.size() - 1)
	{
		// search inactive shader
		// default to the last element
		index = mFragmentShaders.size() - 1;
		for (int i = 0; i < mFragmentShaders.size() - 1; i++)
		{
			if (!mFragmentShaders[i].active) index = i;
		}
	}
	try
	{
		fs::path fr = aFilePath;
		string name = "unknown";
		string mFile = fr.string();
		if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
		mFragFileName = name;
		if (fs::exists(fr))
		{
			validFrag = false;
			std::string fs = shaderInclude + loadString(loadFile(aFilePath));
			rtn = setGLSLStringAtIndex(fs, name, index);
			if (rtn > -1)
			{
				mVDSettings->mMsg = name + " success loadPixelFragmentShaderAtIndex:" + toString(index);
				mVDSettings->newMsg = true;
			}
		}
		else
		{
			CI_LOG_V(mFragFile + " does not exist:" + aFilePath);
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V(aFilePath + " unable to load/compile shader:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(aFilePath + " unable to load shader:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}

	return rtn;
}
int VDShaders::loadPixelFragmentShader(string aFilePath)
{
	int rtn = -1;
	// reset 
	mVDSettings->iFade = false;
	mVDSettings->controlValues[22] = 1.0f;
	try
	{
		fs::path fr = aFilePath;
		string name = "unknown";
		string mFile = fr.string();
		if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
		mFragFileName = name;
		if (fs::exists(fr))
		{
			validFrag = false;
			std::string fs = shaderInclude + loadString(loadFile(aFilePath));
			rtn = setGLSLString(fs, name);
			if (rtn > -1)
			{
				CI_LOG_V(mFragFile + " loaded and compiled");
				mVDSettings->mMsg = name + " loadPixelFragmentShader success";
				mVDSettings->newMsg = true;
				//mFragmentShadersNames[rtn] = name;
			}
		}
		else
		{
			CI_LOG_V(mFragFile + " does not exist");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V(aFilePath + " unable to load/compile shader err:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V(aFilePath + " unable to load shader err:" + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}

	return rtn;
}

int VDShaders::setGLSLString(string pixelFrag, string name)
{
	int foundIndex = -1;

	try
	{
		Shada newShader;
		newShader.shader = gl::GlslProg::create(mPassthruVextexShaderString, pixelFrag);
		newShader.name = name;
		newShader.active = true;
		// searching first index of not running shader
		if (mFragmentShaders.size() < mVDSettings->MAX)
		{
			mFragmentShaders.push_back(newShader);
			foundIndex = mFragmentShaders.size() - 1;
		}
		else
		{
			bool indexFound = false;
			while (!indexFound)
			{
				foundIndex++;
				if (foundIndex != mVDSettings->mLeftFragIndex && foundIndex != mVDSettings->mRightFragIndex && foundIndex != mVDSettings->mPreviewFragIndex) indexFound = true;
				if (foundIndex > mFragmentShaders.size() - 1) indexFound = true;
			}

			// load the new shader
			mFragmentShaders[foundIndex].shader = newShader.shader;
			mFragmentShaders[foundIndex].name = name;
			mFragmentShaders[foundIndex].active = true;
		}
		//preview the new loaded shader
		mVDSettings->mPreviewFragIndex = foundIndex;
		CI_LOG_V("setGLSLString success");
		mVDSettings->mMsg = name + " setGLSLString success";
		mVDSettings->newMsg = true;
		mError = "";
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc exc)
	{
		validFrag = false;
		// TODO CI_LOG_E("Problem Compiling ImGui::Renderer shader " << exc.what());
		foundIndex = -1;
		mError = string(exc.what());
		mVDSettings->mMsg = "setGLSLString file: " + name + " error:" + mError;
		mVDSettings->newMsg = true;
		CI_LOG_V(mVDSettings->mMsg);
	}
	return foundIndex;
}

void VDShaders::setShaderMicroSeconds(int index, int micro)
{
	mFragmentShaders[index].microseconds = micro;
}
// find index for insert/update in mFragmentShaders
int VDShaders::findFragmentShaderIndex(int index, string name) {

	int foundIndex = -1;
	// search inactive shader
	for (int i = 0; i < mFragmentShaders.size() - 1; i++)
	{
		if (!mFragmentShaders[i].active) foundIndex = i;
		if (mFragmentShaders[i].name == name) foundIndex = i;
	}
	// if inactive shader or shader with a same name not found
	if (foundIndex == -1) {
		// check if index is out of bounds
		if (index > mFragmentShaders.size() - 1) {
			Shada newShader;
			newShader.name = name;
			//newShader.shader = gl::GlslProg::create(mPassthruVextexShader, currentFrag.c_str());
			newShader.active = true;
			mFragmentShaders.push_back(newShader);
			foundIndex = mFragmentShaders.size() - 1;
		}
		else {
			// if we found an inactive shader, we replace it
			foundIndex = index;
		}
	}
	return foundIndex;
}

int VDShaders::setGLSLPixelShaderAtIndex(gl::GlslProgRef pixelFrag, string name, int index) {
	int foundIndex = findFragmentShaderIndex(index, name);

	// load the new shader
	mFragmentShaders[foundIndex].shader = pixelFrag;
	mFragmentShaders[foundIndex].name = name;
	mFragmentShaders[foundIndex].active = true;

	//preview the new loaded shader
	mVDSettings->mPreviewFragIndex = foundIndex;
	CI_LOG_V("setGLSLStringAtIndex success");
	mError = "";
	mVDSettings->mMsg = name + " setGLSLStringAtIndex success";
	mVDSettings->newMsg = true;
	validFrag = true;

	return foundIndex;
}

int VDShaders::setGLSLStringAtIndex(string pixelFrag, string name, int index)
{
	int foundIndex = findFragmentShaderIndex(index, name);
	try
	{
		// load the new shader
		mFragmentShaders[foundIndex].shader = gl::GlslProg::create(mPassthruVextexShaderString, pixelFrag);
		mFragmentShaders[foundIndex].name = name;
		mFragmentShaders[foundIndex].active = true;

		//preview the new loaded shader
		mVDSettings->mPreviewFragIndex = index;
		CI_LOG_V("setGLSLStringAtIndex success");
		mError = "";
		mVDSettings->mMsg = name + " setGLSLStringAtIndex success";
		mVDSettings->newMsg = true;
		validFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		CI_LOG_V("setGLSLStringAtIndex error: " + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}
	return foundIndex;
}

bool VDShaders::setFragString(string pixelFrag)
{
	//currentFrag = pixelFrag;
	try
	{
		if (mVDSettings->iTransition > 0)
		{
			//doTransition();
		}
		else
		{
			mFragmentShaders[mCurrentPreviewShader].shader = gl::GlslProg::create(mPassthruVextexShaderString, pixelFrag);
			mFragmentShaders[mCurrentPreviewShader].name = "some.frag";
			mFragmentShaders[mCurrentPreviewShader].active = true;

			//if (mVDSettings->mDirectRender) renderPreviewShader();// mFragmentShaders[mCurrentRenderShader] = gl::GlslProg(NULL, currentFrag.c_str());
			CI_LOG_V("setFragString success");
			mError = "";
			validFrag = true;
		}
		// save as current.frag for code editor not refreshed but ok to load before live code is enabled
		/* */
		if (mVDSettings->iDebug)
		{
			fs::path currentFile = getAssetPath("") / "shaders" / "current.frag";
			ofstream mCurrentFrag(currentFile.string(), std::ofstream::binary);
			mCurrentFrag << pixelFrag;
			mCurrentFrag.close();
			CI_LOG_V("current live editor mix saved:" + currentFile.string());

			mixFileName = previousFileName + "-" + currentFileName + ".frag";
			fs::path defaultFile = getAssetPath("") / "shaders" / "default" / mixFileName;
			ofstream mDefaultFrag(defaultFile.string(), std::ofstream::binary);
			mDefaultFrag << pixelFrag;
			mDefaultFrag.close();
			CI_LOG_V("default mix saved:" + defaultFile.string());
		}
		// if codeeditor mCodeEditor->setValue( pixelFrag );// CHECK 

	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validFrag = false;
		mError = string(exc.what());
		CI_LOG_V("setFragString error: " + mError);
		mVDSettings->mMsg = mError;
		mVDSettings->newMsg = true;
	}

	return validFrag;
}


bool VDShaders::loadTextFile(string aFilePath)
{
	bool success = false;
	string rtn = "";
	try
	{
		fs::path txtFile = aFilePath;
		if (fs::exists(txtFile))
		{
			DataSourceRef dsr = loadFile(txtFile);
			rtn = loadString(loadFile(txtFile));
			CI_LOG_V(aFilePath + " content:" + rtn);
		}
		success = true;
	}
	catch (const std::exception &e)
	{
		CI_LOG_V(fileName + " unable to load string from text file:" + string(e.what()));
	}
	return success;
	/* TODO
	// parse json
	if ( parseFragJson( mFile ) )
	{
	if ( mUserInterface->mCodeEditor )
	{
	string s = fragBegin + fragGlobalFunctions + fragFunctions + fragMainHeader + fragMainLines + fragEnd;
	mUserInterface->mCodeEditor->setValue( s );
	mUserInterface->mCodeEditor->write( mFile + "-" + ci::toString( (int)getAverageFps() ) + ".frag" );
	}
	*/
}
void VDShaders::createThumbsFromDir(string filePath)
{
	int i = 0;
	string ext = "";
	fs::path p(filePath);
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))// && mFragmentShaders.size() < mVDSettings->MAX
		{
			string fileName = it->path().filename().string();
			int dotIndex = fileName.find_last_of(".");

			if (dotIndex != std::string::npos)
			{
				ext = fileName.substr(dotIndex + 1);
				if (ext == "glsl")
				{
					try
					{
						std::string fs = shaderInclude + loadString(loadFile(it->path()));

						Shada newShader;
						newShader.shader = gl::GlslProg::create(mPassthruVextexShaderString, fs.c_str());
						newShader.name = fileName;
						newShader.active = true;
						mFragmentShaders.push_back(newShader);
						CI_LOG_V("createThumbsFromDir loaded and compiled " + fileName);

						/*mVDSettings->mPreviewFragIndex = mFragmentShaders.size() - 1;*/
					}
					catch (gl::GlslProgCompileExc &exc)
					{
						validFrag = false;
						mError = string(exc.what());
						CI_LOG_V("createThumbsFromDir error: " + mError + " on " + fileName);
					}
					//sequenceTextures.push_back(ci::gl::Texture(loadImage(filePath + fileName)));
				}
			}
		}
	}
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
