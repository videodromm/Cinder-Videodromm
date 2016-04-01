#include "VDTextures.h"

using namespace VideoDromm;

VDTextures::VDTextures(VDSettingsRef aVDSettings, VDShadersRef aShadersRef, VDAnimationRef aAnimationRef)
{
	mVDSettings = aVDSettings;
	mVDShaders = aShadersRef;
	mVDAnimation = aAnimationRef;
	CI_LOG_V("VDTextures constructor");


	currentShadaThumbIndex = 0;
	// vertex sphere fbo at index 11
	/*ci::gl::Fbo::Format mFormat;
	mVDFbos[mVDSettings->mVertexSphereFboIndex].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, mFormat);

	//const char* fboNames[] = { "mix", "", "", "", "", "", "", "", "", "", "audio", "", "1", "2", "3", "4" };
	sprintf_s(mVDFbos[1].name, "left");
	sprintf_s(mVDFbos[2].name, "right");
	sprintf_s(mVDFbos[3].name, "warp1");
	sprintf_s(mVDFbos[4].name, "warp2");
	sprintf_s(mVDFbos[5].name, "preview");
	sprintf_s(mVDFbos[6].name, "abp");
	sprintf_s(mVDFbos[7].name, "live");
	sprintf_s(mVDFbos[8].name, "sphere");
	sprintf_s(mVDFbos[9].name, "mesh");
	sprintf_s(mVDFbos[11].name, "vtxsphere");

	for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
	// store it as a 512x2 texture in the first texture
	Texta audioTex;
	sprintf_s(audioTex.name, "Audio");
	audioTex.sequenceIndex = 0;
	audioTex.isSequence = false;
	textas.push_back(audioTex);
	sTextures.push_back(gl::Texture::create(dTexture, 0x1909, 512, 2));//GL_LUMINANCE
	*/
	/*fs::path localFile;
	for (int j = 0; j < mVDSettings->MAX - 1; j++)
	{
		fileName = toString(j) + ".jpg";
		localFile = getAssetPath("") / mVDSettings->mAssetsPath / fileName;
		if (fs::exists(localFile))
		{
			gl::TextureRef img = gl::Texture::create(loadImage(localFile));// TODO , gl::Texture::Format().magFilter(GL_LINEAR).minFilter(GL_LINEAR).loadTopDown());
			//img.setFlipped();
			sTextures.push_back(img);
		}
		else
		{
			//gl::TextureRef img = gl::Texture::create(loadImage(loadAsset("videodromm-logo.png")));
			gl::TextureRef img = gl::Texture::create(loadImage(loadAsset("videodromm-logo.png")), gl::Texture::Format().loadTopDown());
			sTextures.push_back(img);
		}
		Texta tex;
		sprintf_s(tex.name, "tex%d", j);
		tex.sequenceIndex = 0;
		tex.isSequence = false;
		textas.push_back(tex);

	}*/
	// Fbos
	/*
	static const int			TEXTUREMODEINPUT = 0;			// spout
	static const int			TEXTUREMODESHADER = 1;			// shader
	static const int			TEXTUREMODEIMAGE = 2;			// image
	static const int			TEXTUREMODEIMAGESEQUENCE = 3;	// image sequence
	static const int			TEXTUREMODEAUDIO = 4;			// audio spectrum
	static const int			TEXTUREMODETHUMB = 5;			// thumb
	static const int			TEXTUREMODEFBO = 6;				// fbo

	*/
	// mix fbo at index 0
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "mix", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// left fbo at index 1
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "left", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// right fbo at index 2
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "right", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// warp1 fbo at index 3
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "warp1", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// warp2 fbo at index 4
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "warp2", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// preview fbo at index 5
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "preview", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEFBO));
	// audio fbo at index 6
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "audio", mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->TEXTUREMODEAUDIO));
	//thumb fbo
	mVDFbos.push_back(VDFbo::create(mVDSettings, mVDShaders, "thumbfbo0", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->TEXTUREMODETHUMB));

	/*for (int i = 0; i < mVDFbos.size(); i++)
	{

		fileName = toString(i) + ".glsl";
		localFile = getAssetPath("") / mVDSettings->mAssetsPath / fileName;
		if (fs::exists(localFile)) {
			string shaderPath = localFile.string();
			mVDFbos[i]->loadPixelFragmentShader(shaderPath);

		}
	}*/

	/*sprintf_s(textas[1].name, "img1");
	sprintf_s(textas[2].name, "img2");
	sprintf_s(textas[3].name, "img3");
	sprintf_s(textas[4].name, "4pvwFbo");
	sprintf_s(textas[5].name, "5mixFbo");
	sprintf_s(textas[6].name, "6leftFbo");
	sprintf_s(textas[7].name, "7rightFbo");
	sprintf_s(textas[8].name, "8warp1Fbo");
	sprintf_s(textas[9].name, "9warp2Fbo");
	sprintf_s(textas[10].name, "10spout");
	sprintf_s(textas[11].name, "11LiveFbo");*/
	//gl::enableDepthWrite();
}
/*char* VDTextures::getTextureName(int index) {
	return textas[index].name;
	}
	void VDTextures::setTextureName(int index, char* name) {
	sprintf_s(textas[index].name, name);

	}

	const char* VDTextures::getFboName(int index) {

	return mVDFbos[index]->getName().c_str();
	}*/
/*void VDTextures::createWarpInput()
{
WarpInput newWarpInput;
newWarpInput.leftIndex = 0;
newWarpInput.leftMode = 0;
newWarpInput.rightIndex = 0;
newWarpInput.rightMode = 0;
newWarpInput.controlValues[18] = 0.5;
newWarpInput.hasTexture = false;
warpInputs.push_back(newWarpInput);
// init mixTextures
mMixesFbos.push_back(gl::Fbo(mVDSettings->mFboWidth, mVDSettings->mFboHeight));
}

void VDTextures::setAudioTexture(unsigned char *signal)
{
	sTextures[0] = gl::Texture::create(signal, 0x1909, 512, 2);//GL_LUMINANCE, 512, 2);
}*/
void VDTextures::setFboTexture(ci::gl::TextureRef aTexture) {
	for (int i = 0; i < mVDFbos.size(); i++)
	{
		mVDFbos[i]->setTexture(aTexture);
	}
}
/*void VDTextures::setTexture(int index, string fileName)
{
	if (index > mVDSettings->MAX - 1) index = mVDSettings->MAX - 1;
	if (index > 0)
	{
		try
		{
			string pathToAssetFile = (getAssetPath("") / fileName).string();

			if (!fs::exists(pathToAssetFile))
			{
				CI_LOG_V("asset file not found: " + fileName);
			}
			else
			{
				CI_LOG_V("asset found file: " + fileName);
				if (index < sTextures.size())
				{
					sTextures[index] = gl::Texture::create(loadImage(loadAsset(fileName)));
				}
				else
				{
					sTextures.push_back(gl::Texture::create(loadImage(loadAsset(fileName))));
				}
				CI_LOG_V("asset loaded: " + fileName);
			}
		}
		catch (...)
		{
			CI_LOG_V("Load asset error: " + fileName);
		}
	}
}*/
void VDTextures::flipTexture(int index)
{
	/*sTextures[index].setFlipped(!sTextures[index].isFlipped());
	sprintf(buf, "flipTexture index %d after: %d", index, sTextures[index].isFlipped());
	mVDSettings->mMsg = buf;
	mVDSettings->newMsg = true;*/
}
void VDTextures::flipFboV(int index)
{
	//mVDFbos[index]->isFlipV() = !mVDFbos[index]->isFlipV();
}
void VDTextures::flipFboH(int index)
{
	//mVDFbos[index]->isFlipH() = !mVDFbos[index]->isFlipH();
}
/*void VDTextures::setCurrentFboIndex(int aFbo)
{
if (aFbo < mVDFbos.size()) mVDSettings->mCurrentFboIndex = aFbo;
}
ci::gl::Texture VDTextures::getFboThumb(int index)
{
if (index > mThumbFbos.size() - 1) index = mThumbFbos.size() - 1;
return mThumbFbos[index].getTexture();
}

ci::gl::TextureRef VDTextures::getTexture(int index)
{
	if (index > sTextures.size() - 1) index = sTextures.size() - 1;
	return sTextures[index];
}
ci::gl::TextureRef VDTextures::getWarpTexture(int index)
{
if (index > mVDSettings->MAX - 1) index = mVDSettings->MAX - 1;
return mVDSettings->mWarpFbos[index]->getTexture();
}
void VDTextures::setTexture(int index, ci::gl::TextureRef texture)
{
	if (index < sTextures.size())
	{
		sTextures[index] = texture;
	}
	}*/
ci::gl::TextureRef VDTextures::getInputTexture(int index)
{
	if (index > mVDInputTextures.size() - 1) index = mVDInputTextures.size() - 1;
	return mVDInputTextures[index]->getTexture();
}
ci::gl::TextureRef VDTextures::getFboTexture(int index)
{
	if (index > mVDFbos.size() - 1) index = mVDFbos.size() - 1;
	return mVDFbos[index]->getTexture();
}
GLuint VDTextures::getFboTextureId(int index)
{
	if (index > mVDFbos.size() - 1) index = mVDFbos.size() - 1;
	return mVDFbos[index]->getId();
}

/*ci::gl::FboRef VDTextures::getFbo(int index)
{
// fbo
return mVDFbos[index].fbo;
}
GLuint VDTextures::getShaderThumbTextureId(int index)
{
if (index > mThumbFbos.size() - 1) index = mThumbFbos.size() - 1;
return  mThumbFbos[index].fbo->getId();
}*/
string VDTextures::getTextureName(int index) {
	if (index > mVDFbos.size() - 1) index = mVDFbos.size() - 1;
	return mVDFbos[index]->getName();
}

void VDTextures::loadImageFile(int index, string aFile)
{
	try
	{
		// try loading image file
		fs::path imageFile = aFile;
		if (fs::exists(imageFile)) {
			if (index > 0) mVDInputTextures.push_back(VDInputTexture::create(mVDSettings, index, aFile, true));
			//sTextures[index] = gl::Texture::create(loadImage(aFile));
			mVDSettings->isUIDirty = true;		
		}
	}
	catch (...)
	{
		CI_LOG_V("Error loading image: " + aFile);
	}
}

void VDTextures::loadMovieFile(int index, string aFile)
{
	//try
	//{
	//	mMovieIndex = index;
	//	// try loading video file
	//	mMovie = qtime::MovieGl::create(aFile);
	//	mMovie->setLoop();
	//	mMovie->play();
	//}
	//catch (...)
	//{
	//	CI_LOG_V("Error loading video: " + aFile);
	//	mMovie->reset();

	//}
}

void VDTextures::update()
{
	//if (mMovie) {
	//	if (mMovie->hasVisuals()) {
	//		if (mMovie->isPlaying()) {
	//			// binding crashes later on:  sTextures[mMovieIndex] = mMovie->getTexture();
	//		}
	//	}
	//}
}
/*void VDTextures::renderWarpFbos()
{
TODO	mVDShaders->getWarpShader();
if (mWarp.textureMode == 0)
{
// 0 for input texture
getSenderTexture(mWarp.textureIndex)->bind(0);
}
else
{
// 1 for shader
getFboTexture(mWarp.textureIndex)->bind(0);
}
mVDShaders->getWarpShader()->uniform("iAlpha", mVDSettings->controlValues[4]);
gl::draw(mMesh);

}*/
void VDTextures::renderShadaThumbFbo()
{
	// start profiling
	/*auto start = Clock::now();
	gl::ScopedFramebuffer fbScp(mVDFbos[currentShadaThumbIndex]->getFboRef());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mVDFbos[currentShadaThumbIndex]->getFboRef()->getSize());
	gl::setMatricesWindow(mVDFbos[currentShadaThumbIndex]->getFboRef()->getWidth(), mVDFbos[currentShadaThumbIndex]->getFboRef()->getHeight());

	aShader = mVDShaders->getShader(currentShadaThumbIndex).shader;
	gl::ScopedGlslProg currentShader(aShader);
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	aShader->uniform("iResolution", vec3(mVDFbos[currentShadaThumbIndex]->getFboRef()->getWidth(), mVDFbos[currentShadaThumbIndex]->getFboRef()->getHeight(), 1.0));
	aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mVDSettings->iZoomLeft);
	aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
	aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
	aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
	aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
	aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
	aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
	aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
	aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", mVDSettings->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mVDSettings->mRenderXY);
	aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iFlipH", mVDFbos[currentShadaThumbIndex]->isFlipH());
	aShader->uniform("iFlipV", mVDFbos[currentShadaThumbIndex]->isFlipV());
	for (size_t m = 0; m < mVDSettings->MAX; m++)
	{
	getTexture(m)->bind(m);
	}

	gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));

	auto end = Clock::now();
	auto nsdur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	int micro = nsdur.count();
	mVDShaders->setShaderMicroSeconds(currentShadaThumbIndex, micro);*/

	// increment shada thumb index
	currentShadaThumbIndex++;
	// mThumbFbos must equal active mFragmentShaders size
	/*
	TODO
	if (mThumbFbos.size() < mVDShaders->getCount()) {
	FrameBuffa tfb;
	tfb.fbo = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
	tfb.name = "new";
	tfb.isFlipV = false;
	tfb.isFlipH = false;
	mThumbFbos.push_back(tfb);
	}
	if (currentShadaThumbIndex > mVDShaders->getCount() - 1)
	{
	currentShadaThumbIndex = 0;
	}*/
}

void VDTextures::draw()
{
	// start profiling
	auto start = Clock::now();
	mVDSettings->iSeed = randFloat();
	//renderWarpFbos();
	if (mVDSettings->mRenderThumbs) renderShadaThumbFbo();
#pragma region left
	/**********************************************
	* library FBOs
	* start of mLibraryFbos[mVDSettings->mLeftFboIndex]
	*/

	/*mVDFbos[mVDSettings->mLeftFboIndex]->getFboRef()->bindFramebuffer();
	//gl::setViewport(mVDFbos[mVDSettings->mLeftFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
	gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

	aShader = mVDShaders->getShader(mVDSettings->mLeftFragIndex).shader;
	aShader->bind();
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mVDSettings->iZoomLeft);
	aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
	aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
	aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
	aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
	aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
	aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
	aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
	aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", mVDSettings->controlValues[11]);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mVDSettings->mLeftRenderXY);
	aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iFlipH", mVDFbos[mVDSettings->mLeftFboIndex]->isFlipH());
	aShader->uniform("iFlipV", mVDFbos[mVDSettings->mLeftFboIndex]->isFlipV());

	for (size_t m = 0; m < mVDSettings->MAX; m++)
	{
	getTexture(m)->bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	// stop drawing into the FBO
	mVDFbos[mVDSettings->mLeftFboIndex]->getFboRef()->unbindFramebuffer();

	for (size_t m = 0; m < mVDSettings->MAX; m++)
	{
	getTexture(m)->unbind();
	}

	//aShader->unbind();
	sTextures[6] = mVDFbos[mVDSettings->mLeftFboIndex]->getTexture();*/
	/*
	* end of mLibraryFbos[mVDSettings->mLeftFboIndex]
	***********************************************/
#pragma endregion left
#pragma region right
	/***********************************************
	* start of mLibraryFbos[mVDSettings->mRightFboIndex]
	*/

	/*mVDFbos[mVDSettings->mRightFboIndex]->getFboRef()->bindFramebuffer();
	//gl::setViewport(mVDFbos[mVDSettings->mRightFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
	gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

	aShader = mVDShaders->getShader(mVDSettings->mRightFragIndex).shader;
	aShader->bind();
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iZoom", mVDSettings->iZoomRight);
	aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
	aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
	aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
	aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
	aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
	aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
	aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
	aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mVDSettings->mRightRenderXY);
	aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iFlipH", mVDFbos[mVDSettings->mRightFboIndex]->isFlipH());
	aShader->uniform("iFlipV", mVDFbos[mVDSettings->mRightFboIndex]->isFlipV());

	for (size_t m = 0; m < mVDSettings->MAX; m++)
	{
	getTexture(m)->bind(m);
	}
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	// stop drawing into the FBO
	mVDFbos[mVDSettings->mRightFboIndex]->getFboRef()->unbindFramebuffer();

	for (size_t m = 0; m < mVDSettings->MAX; m++)
	{
	getTexture(m)->unbind();
	}

	//aShader->unbind();
	sTextures[7] = mVDFbos[mVDSettings->mRightFboIndex]->getTexture();*/
	/*
	* end of mLibraryFbos[mVDSettings->mRightFboLibraryIndex]
	***********************************************/
#pragma endregion right
#pragma region warp
	if (mVDSettings->mMode == mVDSettings->MODE_WARP)
	{
		/***********************************************
		* start of mVDFbos[mVDSettings->mWarp1FboIndex]
		*/

		/*mVDFbos[mVDSettings->mWarp1FboIndex]->getFboRef()->bindFramebuffer();
		//gl::setViewport(mVDFbos[mVDSettings->mWarp1FboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
		gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

		aShader = mVDShaders->getShader(mVDSettings->mWarp1FragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mVDSettings->iZoomRight);
		aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
		aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
		aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
		aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
		aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
		aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
		aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
		aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
		aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
		aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
		aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
		aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
		aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
		aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
		aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
		aShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mVDSettings->mWarp1RenderXY);
		aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
		aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
		aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
		aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
		aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
		aShader->uniform("iExposure", mVDSettings->controlValues[14]);
		aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
		aShader->uniform("iFade", (int)mVDSettings->iFade);
		aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
		aShader->uniform("iLight", (int)mVDSettings->iLight);
		aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		aShader->uniform("iTransition", mVDSettings->iTransition);
		aShader->uniform("iAnim", mVDSettings->iAnim.value());
		aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
		aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
		aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
		aShader->uniform("iDebug", (int)mVDSettings->iDebug);
		aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
		aShader->uniform("iFps", mVDSettings->iFps);
		aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
		aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
		aShader->uniform("iBeat", mVDSettings->iBeat);
		aShader->uniform("iSeed", mVDSettings->iSeed);
		aShader->uniform("iFlipH", mVDFbos[mVDSettings->mWarp1FboIndex]->isFlipH());
		aShader->uniform("iFlipV", mVDFbos[mVDSettings->mWarp1FboIndex]->isFlipV());

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		// stop drawing into the FBO
		mVDFbos[mVDSettings->mWarp1FboIndex]->getFboRef()->unbindFramebuffer();

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->unbind();
		}

		//aShader->unbind();
		sTextures[8] = mVDFbos[mVDSettings->mWarp1FboIndex]->getTexture();*/

		/*
		* end of mVDFbos[mVDSettings->mWarp1FboIndex]
		***********************************************/

		/***********************************************
		* start of mVDFbos[mVDSettings->mWarp2FboIndex]
		*/
		/*
		mVDFbos[mVDSettings->mWarp2FboIndex]->getFboRef()->bindFramebuffer();
		//gl::setViewport(mVDFbos[mVDSettings->mWarp2FboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
		gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

		aShader = mVDShaders->getShader(mVDSettings->mWarp2FragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mVDSettings->iZoomRight);
		aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
		aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
		aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
		aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
		aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
		aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
		aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
		aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
		aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
		aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
		aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
		aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
		aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
		aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
		aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
		aShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mVDSettings->mWarp2RenderXY);
		aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
		aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
		aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
		aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
		aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
		aShader->uniform("iExposure", mVDSettings->controlValues[14]);
		aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
		aShader->uniform("iFade", (int)mVDSettings->iFade);
		aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
		aShader->uniform("iLight", (int)mVDSettings->iLight);
		aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		aShader->uniform("iTransition", mVDSettings->iTransition);
		aShader->uniform("iAnim", mVDSettings->iAnim.value());
		aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
		aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
		aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
		aShader->uniform("iDebug", (int)mVDSettings->iDebug);
		aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
		aShader->uniform("iFps", mVDSettings->iFps);
		aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
		aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
		aShader->uniform("iBeat", mVDSettings->iBeat);
		aShader->uniform("iSeed", mVDSettings->iSeed);
		aShader->uniform("iFlipH", mVDFbos[mVDSettings->mWarp2FboIndex]->isFlipH());
		aShader->uniform("iFlipV", mVDFbos[mVDSettings->mWarp2FboIndex]->isFlipV());

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		// stop drawing into the FBO
		mVDFbos[mVDSettings->mWarp2FboIndex]->getFboRef()->unbindFramebuffer();

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->unbind();
		}

		//aShader->unbind();
		sTextures[9] = mVDFbos[mVDSettings->mWarp2FboIndex]->getTexture();*/

		/*
		* end of mVDFbos[mVDSettings->mWarp2FboIndex]
		***********************************************/
	}
#pragma endregion warp
#pragma region preview
	if (mVDSettings->mPreviewEnabled)
	{
		/***********************************************
		* start of mLibraryFbos[mVDSettings->mCurrentPreviewFboIndex]
		*/
		/*mVDFbos[mVDSettings->mCurrentPreviewFboIndex]->getFboRef()->bindFramebuffer();
		//gl::setViewport(mVDFbos[mVDSettings->mCurrentPreviewFboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
		gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

		aShader = mVDShaders->getShader(mVDSettings->mPreviewFragIndex).shader;
		aShader->bind();
		aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		//aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mVDSettings->controlValues[22]);
		aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
		aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
		aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
		aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
		aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
		aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
		aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
		aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
		aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
		aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
		aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
		aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
		aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
		aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
		aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
		aShader->uniform("iRatio", mVDSettings->controlValues[11]);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mVDSettings->mPreviewFragXY);
		aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
		aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
		aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
		aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
		aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
		aShader->uniform("iExposure", mVDSettings->controlValues[14]);
		aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
		aShader->uniform("iFade", (int)mVDSettings->iFade);
		aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
		aShader->uniform("iLight", (int)mVDSettings->iLight);
		aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		aShader->uniform("iTransition", mVDSettings->iTransition);
		aShader->uniform("iAnim", mVDSettings->iAnim.value());
		aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
		aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
		aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
		aShader->uniform("iDebug", (int)mVDSettings->iDebug);
		aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
		aShader->uniform("iFps", mVDSettings->iFps);
		aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
		aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
		aShader->uniform("iBeat", mVDSettings->iBeat);
		aShader->uniform("iSeed", mVDSettings->iSeed);
		aShader->uniform("iFlipH", mVDFbos[mVDSettings->mCurrentPreviewFboIndex]->isFlipH());
		aShader->uniform("iFlipV", mVDFbos[mVDSettings->mCurrentPreviewFboIndex]->isFlipV());

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		// stop drawing into the FBO
		mVDFbos[mVDSettings->mCurrentPreviewFboIndex]->getFboRef()->unbindFramebuffer();

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->unbind();
		}

		//aShader->unbind();

		sTextures[4] = mVDFbos[mVDSettings->mCurrentPreviewFboIndex]->getTexture();*/
		/*
		* end of mLibraryFbos[mVDSettings->mCurrentPreviewFboIndex]
		***********************************************/
	}

#pragma endregion preview
#pragma region livecode
	if (mVDSettings->mLiveCode)
	{
		/***********************************************
		* start of mLibraryFbos[mVDSettings->mLiveFboIndex]
		*/
		/*mVDFbos[mVDSettings->mLiveFboIndex]->getFboRef()->bindFramebuffer();
		//gl::setViewport(mVDFbos[mVDSettings->mLiveFboIndex].fbo.getBounds());

		// clear the FBO
		gl::clear(Color(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));
		gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

		aShader = mVDShaders->getLiveShader();
		aShader->bind();
		aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
		aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
		aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
		aShader->uniform("iZoom", mVDSettings->controlValues[22]);
		aShader->uniform("iChannel0", mVDSettings->iChannels[0]);
		aShader->uniform("iChannel1", mVDSettings->iChannels[1]);
		aShader->uniform("iChannel2", mVDSettings->iChannels[2]);
		aShader->uniform("iChannel3", mVDSettings->iChannels[3]);
		aShader->uniform("iChannel4", mVDSettings->iChannels[4]);
		aShader->uniform("iChannel5", mVDSettings->iChannels[5]);
		aShader->uniform("iChannel6", mVDSettings->iChannels[6]);
		aShader->uniform("iChannel7", mVDSettings->iChannels[7]);
		aShader->uniform("iAudio0", 0);
		aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
		aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
		aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
		aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
		aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
		aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
		aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
		aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
		aShader->uniform("iRatio", mVDSettings->controlValues[11]);
		aShader->uniform("width", 1);
		aShader->uniform("height", 1);
		aShader->uniform("iRenderXY", mVDSettings->mPreviewFragXY);
		aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
		aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
		aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
		aShader->uniform("iCrossfade", mVDSettings->controlValues[21]);
		aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
		aShader->uniform("iExposure", mVDSettings->controlValues[14]);
		aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
		aShader->uniform("iFade", (int)mVDSettings->iFade);
		aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
		aShader->uniform("iLight", (int)mVDSettings->iLight);
		aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
		aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
		aShader->uniform("iTransition", mVDSettings->iTransition);
		aShader->uniform("iAnim", mVDSettings->iAnim.value());
		aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
		aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
		aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
		aShader->uniform("iDebug", (int)mVDSettings->iDebug);
		aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
		aShader->uniform("iFps", mVDSettings->iFps);
		aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
		aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
		aShader->uniform("iBeat", mVDSettings->iBeat);
		aShader->uniform("iSeed", mVDSettings->iSeed);
		aShader->uniform("iFlipH", mVDFbos[mVDSettings->mLiveFboIndex]->isFlipH());
		aShader->uniform("iFlipV", mVDFbos[mVDSettings->mLiveFboIndex]->isFlipV());

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->bind(m);
		}
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		// stop drawing into the FBO
		mVDFbos[mVDSettings->mLiveFboIndex]->getFboRef()->unbindFramebuffer();

		for (size_t m = 0; m < mVDSettings->MAX; m++)
		{
		getTexture(m)->unbind();
		}

		//aShader->unbind();

		sTextures[11] = mVDFbos[mVDSettings->mLiveFboIndex]->getTexture();*/
		/*
		* end of mVDFbos[mVDSettings->mLiveFboIndex]
		***********************************************/
	}

#pragma endregion livecode
#pragma region mix
	/***********************************************
	* mix 2 FBOs begin
	* first render the 2 frags to fbos (done before)
	* then use them as textures for the mix shader
	*/
	/*
	// draw using the mix shader
	mVDFbos[mVDSettings->mMixFboIndex]->getFboRef()->bindFramebuffer();
	//gl::setViewport(mVDFbos[mVDSettings->mMixFboIndex].fbo.getBounds());

	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

	aShader = mVDShaders->getMixShader();
	aShader->bind();
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	//20140703 aShader->uniform("iResolution", vec3(mVDSettings->mRenderResoXY.x, mVDSettings->mRenderResoXY.y, 1.0));
	aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", mVDSettings->controlValues[11]);//check if needed: +1;//mVDSettings->iRatio);
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mVDSettings->mRenderXY);
	aShader->uniform("iZoom", mVDSettings->controlValues[22]);
	aShader->uniform("iAlpha", mVDSettings->controlValues[4]);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iChromatic", mVDSettings->controlValues[10]);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[18]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iTrixels", mVDSettings->controlValues[16]);
	aShader->uniform("iGridSize", mVDSettings->controlValues[17]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iRedMultiplier", mVDSettings->iRedMultiplier);
	aShader->uniform("iGreenMultiplier", mVDSettings->iGreenMultiplier);
	aShader->uniform("iBlueMultiplier", mVDSettings->iBlueMultiplier);
	aShader->uniform("iFlipH", mVDFbos[mVDSettings->mMixFboIndex]->isFlipH());
	aShader->uniform("iFlipV", mVDFbos[mVDSettings->mMixFboIndex]->isFlipV());
	aShader->uniform("iParam1", mVDSettings->iParam1);
	aShader->uniform("iParam2", mVDSettings->iParam2);
	aShader->uniform("iXorY", mVDSettings->iXorY);
	aShader->uniform("iBadTv", mVDSettings->iBadTv);

	sTextures[6]->bind(0);
	sTextures[7]->bind(1);
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	// stop drawing into the FBO
	mVDFbos[mVDSettings->mMixFboIndex]->getFboRef()->unbindFramebuffer();
	sTextures[6]->unbind();
	sTextures[7]->unbind();

	//aShader->unbind();
	sTextures[5] = mVDFbos[mVDSettings->mMixFboIndex]->getTexture();
	*/
	//}
	/***********************************************
	* mix 2 FBOs end
	*/
#pragma endregion mix

	auto end = Clock::now();
	auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	//std::cout << msdur.count() << "ms, " << nsdur.count() << "µs" << std::endl;
	sprintf_s(previewTime, "%2d", msdur.count());

}


VDTextures::~VDTextures()
{
	CI_LOG_V("VDTextures destructor");
	//sTextures.clear();
	//sequenceTextures.clear();
}

/*
void VDTextures::setSenderTextureSize(int index, int width, int height)
{
	sTextures[10] = gl::Texture::create(width, height);
}

int VDTextures::createSpoutTexture(char name[256], unsigned int width, unsigned int height)
{
	// replace spout image at index 10
	CI_LOG_V("createSpoutTexture, replace: " + toString(name));

	memcpy(&spoutSenderName[0], name, strlen(name) + 1);
	sTextures[10] = gl::Texture::create(width, height);
	return 10;
}*/