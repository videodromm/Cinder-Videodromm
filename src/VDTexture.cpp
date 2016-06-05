#include "VDTexture.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDTexture::VDTexture(TextureType aType)
		: mPath("")
		, mName("")
		, mTopDown(false)
		, mFlipV(false)
		, mFlipH(true)
		, mWidth(640)
		, mHeight(480)
	{
		mBoundsLocked = true;
		mXLeft = 0;
		mYTop = 0;
		mXRight = mOriginalWidth = mWidth;
		mYBottom = mOriginalHeight = mHeight;
		mAreaWidth = mWidth;
		mAreaHeight = mHeight;
		mFolder = "";
		if (mName.length() == 0) {
			mName = mPath;
		}
		// init the texture whatever happens next
		if (mPath.length() > 0) {
			mTexture = ci::gl::Texture::create(ci::loadImage(mPath), ci::gl::Texture::Format().loadTopDown(mTopDown));
			mInputSurface = Surface(loadImage(mPath));
		}
		else {
			mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
			mInputSurface = Surface(mWidth, mHeight, true);
		}
	}
	VDTexture::~VDTexture(void) {

	}
	VDTextureList VDTexture::readSettings(VDAnimationRef aVDAnimation, const DataSourceRef &source)
	{
		XmlTree			doc;
		VDTextureList	vdtexturelist;
		bool            isValid = true;
		// try to load the specified xml file
		try { doc = XmlTree(source); }
		catch (...) { return vdtexturelist; }

		// check if this is a valid file 
		bool isOK = doc.hasChild("textures");
		//if (!isOK) return vdtexturelist;

		//
		if (isOK) {

			XmlTree texturesXml = doc.getChild("textures");

			// iterate textures
			for (XmlTree::ConstIter child = texturesXml.begin("texture"); child != texturesXml.end(); ++child) {
				// create texture of the correct type
				std::string texturetype = child->getAttributeValue<std::string>("texturetype", "unknown");
				XmlTree detailsXml = child->getChild("details");

				//std::string path = detailsXml.getAttributeValue<std::string>("path", "");
				if (texturetype == "image") {
					TextureImageRef t(new TextureImage());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "imagesequence") {
					TextureImageSequenceRef t(new TextureImageSequence());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "movie") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureMovieRef t(new TextureMovie());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// movie not supported on this platform
					CI_LOG_V("movie not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "camera") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureCameraRef t(new TextureCamera());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// camera not supported on this platform
					CI_LOG_V("camera not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "shared") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureSharedRef t(new TextureShared());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// shared textures not supported on this platform
					CI_LOG_V("shared textures not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "audio") {
					TextureAudioRef t(new TextureAudio(aVDAnimation));
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else {
					// unknown texture type
					CI_LOG_V("unknown texture type");
				}
			}
			if (!isValid)
			{
				TextureImageRef t(new TextureImage());
				XmlTree		xml;
				xml.setTag("details");
				xml.setAttribute("path", "0.jpg");
				xml.setAttribute("width", 640);
				xml.setAttribute("height", 480);
				t->fromXml(xml);
				vdtexturelist.push_back(t);
			}
		}
		else {
			// malformed XML
			CI_LOG_V("malformed XML");
			TextureImageRef t(new TextureImage());
			XmlTree		xml;
			xml.setTag("details");
			xml.setAttribute("path", "0.jpg");
			xml.setAttribute("width", 640);
			xml.setAttribute("height", 480);
			t->fromXml(xml);
			vdtexturelist.push_back(t);
		}

		return vdtexturelist;
	}

	void VDTexture::writeSettings(const VDTextureList &vdtexturelist, const ci::DataTargetRef &target) {

		// create config document and root <textures>
		XmlTree			doc;
		doc.setTag("textures");
		doc.setAttribute("version", "1.0");

		// 
		for (unsigned i = 0; i < vdtexturelist.size(); ++i) {
			// create <texture>
			XmlTree			texture;
			texture.setTag("texture");
			texture.setAttribute("id", i + 1);
			switch (vdtexturelist[i]->mType) {
			case IMAGE: texture.setAttribute("texturetype", "image"); break;
			case IMAGESEQUENCE: texture.setAttribute("texturetype", "imagesequence"); break;
			case MOVIE: texture.setAttribute("texturetype", "movie"); break;
			case SHARED: texture.setAttribute("texturetype", "shared"); break;
			case CAMERA: texture.setAttribute("texturetype", "camera"); break;
			case AUDIO: texture.setAttribute("texturetype", "audio"); break;
			default: texture.setAttribute("texturetype", "unknown"); break;
			}
			// details specific to texture type
			texture.push_back(vdtexturelist[i]->toXml());

			// add texture doc
			//texture.setAttribute("path", vdtexturelist[i]->mPath);
			doc.push_back(texture);
		}

		// write file
		doc.write(target);
	}
	XmlTree	VDTexture::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("path", mPath);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);

		return xml;
	}

	void VDTexture::fromXml(const XmlTree &xml)
	{
	}
	void VDTexture::loadFromFullPath(string aPath) {
		// initialize texture
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
	}
	void VDTexture::lockBounds(bool lock, unsigned int aWidth, unsigned int aHeight) {
		mBoundsLocked = lock;
		mAreaWidth = aWidth;
		mAreaHeight = aHeight;
	}
	void VDTexture::setXLeft(int aXleft) { 
		mXLeft = aXleft; 
		if (mBoundsLocked) {
			mXRight = mXLeft+ mAreaWidth;
		}
	};
	void VDTexture::setYTop(int aYTop) { 
		mYTop = aYTop; 
		if (mBoundsLocked) {
			mYBottom = mYTop + mAreaHeight;
		}
	};
	void VDTexture::setXRight(int aXRight) {
		mXRight = aXRight; 
		if (mBoundsLocked) {
			mXLeft = mXRight - mAreaWidth;
		}
	};
	void VDTexture::setYBottom(int aYBottom) { 
		mYBottom = aYBottom; 
		if (mBoundsLocked) {
			mYTop = mYBottom - mAreaHeight;
		}	
	}
	bool VDTexture::isTopDown() {
		return mTopDown;
	}
	void VDTexture::toggleTopDown() { 
		mTopDown = !mTopDown;
	}
	bool VDTexture::getLockBounds() {
		return mBoundsLocked;
	}
	void VDTexture::toggleLockBounds() {
		mBoundsLocked = !mBoundsLocked;
	};
	unsigned int VDTexture::getTextureWidth() {
		return mWidth;
	};

	unsigned int VDTexture::getTextureHeight() {
		return mHeight;
	};
	unsigned int VDTexture::getOriginalWidth() {
		return mOriginalWidth;
	};
	unsigned int VDTexture::getOriginalHeight() {
		return mOriginalHeight;
	};
	ci::ivec2 VDTexture::getSize() {
		return mTexture->getSize();
	}

	ci::Area VDTexture::getBounds() {
		return mTexture->getBounds();
	}

	GLuint VDTexture::getId() {
		return mTexture->getId();
	}

	std::string VDTexture::getName(){
		return mName;
	}

	ci::gl::TextureRef VDTexture::getTexture() {
		return mTexture;
	}
	/*
	**   Child classes
	*/
	/*
	**   TextureImage for jpg, png
	*/
	TextureImage::TextureImage() {
		mType = IMAGE;
	}
	XmlTree	TextureImage::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("topdown", mTopDown);
		return xml;
	}

	void TextureImage::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		// retrieve attributes specific to this type of texture
		mTopDown = xml.getAttributeValue<bool>("topdown", "true"); // default true
		mPath = xml.getAttributeValue<string>("path", "");
		mFolder = xml.getAttributeValue<string>("folder", "");
		mName = mPath;
		if (mPath.length() > 0) {
			fs::path fullPath = getAssetPath("") / mFolder / mPath;
			loadFromFullPath(fullPath.string());			
		}
	}
	void TextureImage::loadFromFullPath(string aPath) {
		if (fs::exists(aPath)) {
			mTexture = ci::gl::Texture::create(loadImage(aPath)); //loadAsset(mPath)
			mInputSurface = Surface(loadImage(aPath));
		}
		else {
			mTexture = ci::gl::Texture::create(mWidth, mHeight);
			mInputSurface = Surface(mWidth, mHeight, true);
		}
		mXLeft = 0;
		mYTop = 0;
		mXRight = mOriginalWidth = mTexture->getWidth();
		mYBottom = mOriginalHeight = mTexture->getHeight();
	}

	ci::gl::Texture2dRef TextureImage::getTexture() {
		Area area(mXLeft, mYTop, mXRight, mYBottom);
		mProcessedSurface = mInputSurface.clone(area);
		mTexture = gl::Texture2d::create(mProcessedSurface, ci::gl::Texture::Format().loadTopDown(mTopDown));
		return mTexture;
	}

	TextureImage::~TextureImage(void) {
	}

	/*
	** TextureImageSequence
	*/
	TextureImageSequence::TextureImageSequence() {
		// constructor
		mType = IMAGESEQUENCE;
		playheadFrameInc = 1;
		mLoadingFilesComplete = true;
		mLoadingPaused = false;
		mFramesLoaded = 0;
		mCurrentLoadedFrame = 0;
		mNextIndexFrameToTry = 0;
		mPlaying = false;
		mSpeed = 1;
		mExt = "png";
		mPrefix = "none";
		mNextIndexFrameToTry = 0;
		mPlayheadPosition = 0;
		mNumberOfDigits = 4;
		mSyncToBeat = false;

	}
	void TextureImageSequence::loadFromFullPath(string aPath)
	{
		bool noValidFile = true; // if no valid files in the folder, we keep existing vector
		string anyImagefileName = "0.jpg";
		string folder = "";
		string fileName;
		fs::path fullPath = aPath;
		if (fs::exists(fullPath)) {
			try {
				bool firstIndexFound = false;
				// loading 2000 files takes a while, I load only the first one
				for (fs::directory_iterator it(fullPath); it != fs::directory_iterator(); ++it)
				{
					if (fs::is_regular_file(*it))
					{
						fileName = it->path().filename().string();
						mName = fileName;
						if (fileName.find_last_of(".") != std::string::npos) {
							int dotIndex = fileName.find_last_of(".");
							mExt = fileName.substr(dotIndex + 1);
							if (mExt == "png" || mExt == "jpg") {
								anyImagefileName = fileName;
							}
							// get the prefix for the image sequence
							// the files are named from p0000.jpg to p2253.jpg for instance
							// sometimes only 3 digits : l000 this time
							// find the first digit
							int firstDigit = fileName.find_first_of("0123456789");
							// if valid image sequence (contains a digit)
							if (firstDigit > -1) {
								mNumberOfDigits = dotIndex - firstDigit;
								int prefixIndex = fileName.find_last_of(".") - mNumberOfDigits;//-4 or -3
								mPrefix = fileName.substr(0, prefixIndex);
								if (!firstIndexFound) {
									firstIndexFound = true;
									mNextIndexFrameToTry = std::stoi(fileName.substr(prefixIndex, dotIndex));
								}
							}
						}
						// only if proper image sequence
						if (firstIndexFound) {
							if (mExt == "png" || mExt == "jpg") {
								if (noValidFile) {
									// we found a valid file
									noValidFile = false;
									mSequenceTextures.clear();
									// TODO only store folder relative to assets, not full path 
									size_t found = fullPath.string().find_last_of("/\\");
									mPath = fullPath.string().substr(found + 1);

									// reset playhead to the start
									//mPlayheadPosition = 0;
									mLoadingFilesComplete = false;
									loadNextImageFromDisk();
									mPlaying = true;
								}
							}
						}

					}
				}
				CI_LOG_V("successfully loaded " + mPath);
			}
			catch (Exception &exc) {
				CI_LOG_EXCEPTION("error loading ", exc);
			}

			// init: if no valid file found we take the default 0.jpg
			if (noValidFile) {
				if (anyImagefileName.length() > 0) {
					mTexture = ci::gl::Texture::create(loadImage(loadAsset(anyImagefileName)), ci::gl::Texture::Format().loadTopDown(mTopDown));
					mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(loadAsset(anyImagefileName)), gl::Texture::Format().loadTopDown(mTopDown)));
					mLoadingFilesComplete = true;
					mFramesLoaded = 1;
				}
			}
		}
	}
	void TextureImageSequence::fromXml(const XmlTree &xml)
	{
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		// retrieve attributes specific to this type of texture
		mPath = xml.getAttributeValue<string>("path", "");
		mName = mPath;
		mTopDown = xml.getAttributeValue<bool>("topdown", "false");
		if (mPath.length() > 0) {
			fs::path fullPath = getAssetPath("") / mPath;// TODO / mVDSettings->mAssetsPath
			loadFromFullPath(fullPath.string());
		}
	}
	XmlTree	TextureImageSequence::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("topdown", mTopDown);
		return xml;
	}
	void TextureImageSequence::loadNextImageFromDisk() {
		if (!mLoadingPaused) {

			if (!mLoadingFilesComplete) {
				// thank you Omar!
				char restOfFileName[32];
				if (mNumberOfDigits == 4) {

					sprintf(restOfFileName, "%04d", mNextIndexFrameToTry);
				}
				else {

					sprintf(restOfFileName, "%03d", mNextIndexFrameToTry);
				}
				string fileNameToLoad = mPrefix + restOfFileName + "." + mExt;
				fs::path fileToLoad = getAssetPath("") / mPath / fileNameToLoad;
				if (fs::exists(fileToLoad)) {
					// start profiling
					auto start = Clock::now();

					mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(fileToLoad), gl::Texture::Format().loadTopDown()));
					mCurrentLoadedFrame = mFramesLoaded;
					mFramesLoaded++;
					auto end = Clock::now();
					auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					int milli = msdur.count();

					mStatus = fileToLoad.string() + " loaded in ms " + toString(milli);
				}
				else {

					mStatus = fileToLoad.string() + " does not exist";
					if (mFramesLoaded > 0) {
						// if frames have been loaded we hit the last file of the image sequence at this point
						mStatus = "last image loaded";
						mLoadingFilesComplete = true;
					}
				}

				// increment counter for next filename
				mNextIndexFrameToTry++;
				if (mNextIndexFrameToTry > 9999 && mNumberOfDigits == 4) mLoadingFilesComplete = true;
				if (mNextIndexFrameToTry > 999 && mNumberOfDigits == 3) mLoadingFilesComplete = true;
			}
		}
	}
	void TextureImageSequence::updateSequence() {
		int newPosition;
		// TODO check: getTexture()
		if (mSequenceTextures.size() > 0) {
			// Call on each frame to update the playhead
			if (mPlaying) {
				newPosition = mPlayheadPosition + (playheadFrameInc * mSpeed);
				if (newPosition < 0) newPosition = mSequenceTextures.size() - 1;
				if (newPosition > mSequenceTextures.size() - 1) newPosition = 0;
			}
			else {
				if (mSyncToBeat) {
					//newPosition = (int)(((int)(mVDAnimation->iBar / mVDAnimation->iBeatIndex)) % mSequenceTextures.size());
					// TODO newPosition = (int)(((int)(mVDSettings->iBeat / mVDAnimation->iBeatIndex)) % mSequenceTextures.size());
				}
				else {
					newPosition = mPlayheadPosition;
				}
			}
			mPlayheadPosition = max(0, min(newPosition, (int)mSequenceTextures.size() - 1));
		}
	}

	ci::gl::Texture2dRef TextureImageSequence::getTexture() {

		if (mSequenceTextures.size() > 0) {

			if (mPlayheadPosition > mFramesLoaded) {
				//error
				mPlayheadPosition = 0;
			}
			if (!mLoadingFilesComplete) loadNextImageFromDisk();

			if (mPlaying)  {
				updateSequence();
			}
			mTexture = mSequenceTextures[mPlayheadPosition];
		}
		return mTexture;

	}
	// play/pause sequence
	void TextureImageSequence::playPauseSequence() {

		mPlaying = !mPlaying;
	}
	// sync to beat
	void TextureImageSequence::syncToBeat() {

		mSyncToBeat = !mSyncToBeat;
	}

	// Stops playback and resets the playhead to zero
	void TextureImageSequence::stopSequence() {

		mPlaying = false;
		mPlayheadPosition = 0;
	}

	int TextureImageSequence::getMaxFrame() {

		return mFramesLoaded;
	}
	int TextureImageSequence::getPlayheadPosition() {

		return mPlayheadPosition;
	}
	// Seek to a new position in the sequence
	void TextureImageSequence::setPlayheadPosition(int position) {

		mPlayheadPosition = max(0, min(position, (int)mSequenceTextures.size() - 1));
		if (!mLoadingFilesComplete) {
			loadNextImageFromDisk();
		}
	}

	void TextureImageSequence::reverseSequence() {
		mSpeed *= -1;
	}
	int TextureImageSequence::getSpeed() {

		return mSpeed;
	}
	void TextureImageSequence::setSpeed(int speed) {
		mSpeed = speed;
	}
	bool TextureImageSequence::isLoadingFromDisk() {

		return mLoadingFilesComplete;
	}

	void TextureImageSequence::toggleLoadingFromDisk() {

		mLoadingPaused = !mLoadingPaused;
	}
	TextureImageSequence::~TextureImageSequence(void) {

	}


	/*
	** ---- TextureMovie ------------------------------------------------
	*/
	TextureMovie::TextureMovie() {
		mType = MOVIE;

	}
	void TextureMovie::fromXml(const XmlTree &xml)
	{
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		// retrieve attributes specific to this type of texture
		mTopDown = xml.getAttributeValue<bool>("topdown", "false");
		mPath = xml.getAttributeValue<string>("path", "");
		if (mPath.length() > 0) {
			fs::path fullPath = getAssetPath("") / mPath;// TODO / mVDSettings->mAssetsPath
			if (fs::exists(fullPath)) {
				loadFromFullPath(fullPath.string());
			}
		}
		mName = mPath;
	}
	XmlTree	TextureMovie::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("topdown", mTopDown);
		return xml;
	}
	void TextureMovie::loadFromFullPath(string aPath)
	{
		try {
			mMovie.reset();
			// load up the movie, set it to loop, and begin playing
#if defined( CINDER_MSW )
			mMovie = qtime::MovieGlHap::create(aPath);
#endif
#if defined( CINDER_MAC )
			mMovie = qtime::MovieGl::create( aPath );
#endif
			if ( mName.length() > 0) mName = aPath;
			mLoopVideo = (mMovie->getDuration() < 30.0f);
			mMovie->setLoop(mLoopVideo);
			mMovie->play();
			mWidth = mMovie->getWidth();
			mHeight = mMovie->getHeight();
			mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		}
		catch (ci::Exception &e)
		{
			console() << string(e.what()) << std::endl;
			console() << "Unable to load the movie." << std::endl;
		}

	}
	ci::gl::Texture2dRef TextureMovie::getTexture() {
		if (mMovie) {
			mTexture = mMovie->getTexture();
			// if codec is not recognized the texture is empty, return an initialized texture
			if (!mTexture) {
				mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
			}
		}
		return mTexture;
	}
	TextureMovie::~TextureMovie(void) {
	}

	/*
	** ---- TextureCamera ------------------------------------------------
	*/
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	TextureCamera::TextureCamera() {
		mType = CAMERA;
		mFirstCameraDeviceName = "";
		printDevices();

		try {
			mCapture = Capture::create(640, 480);
			mCapture->start();
		}
		catch (ci::Exception &exc) {
			CI_LOG_EXCEPTION("Failed to init capture ", exc);
		}
	}
	void TextureCamera::fromXml(const XmlTree &xml) {
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		// retrieve attributes specific to this type of texture
		mPath = xml.getAttributeValue<string>("path", "");
		mName = "camera";
	}
	XmlTree	TextureCamera::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("camera", mFirstCameraDeviceName);
		xml.setAttribute("topdown", mTopDown);
		xml.setAttribute("path", mPath);
		return xml;
	}
	ci::gl::Texture2dRef TextureCamera::getTexture() {
		if (mCapture && mCapture->checkNewFrame()) {
			if (!mTexture) {
				// Capture images come back as top-down, and it's more efficient to keep them that way
				mTexture = gl::Texture::create(*mCapture->getSurface(), gl::Texture::Format().loadTopDown());
			}
			else {
				mTexture->update(*mCapture->getSurface());
			}
		}
		return mTexture;
	}
	void TextureCamera::printDevices() {
		for (const auto &device : Capture::getDevices()) {
			console() << "Device: " << device->getName() << " "
#if defined( CINDER_COCOA_TOUCH )
				<< (device->isFrontFacing() ? "Front" : "Rear") << "-facing"
#endif
				<< endl;
			mFirstCameraDeviceName = device->getName();
		}
	}
	TextureCamera::~TextureCamera(void) {

	}
#endif

	/*
	** ---- TextureShared ------------------------------------------------
	*/
	TextureShared::TextureShared() {
		mType = SHARED;
#if defined( CINDER_MSW )
		initialized = false;
		g_Width = 320; // set global width and height to something
		g_Height = 240; // they need to be reset when the receiver connects to a sender

#endif
#if defined( CINDER_MAC )
		mClientSyphon.setup();
		mClientSyphon.set("", "Videodromm client");
		mClientSyphon.bind();
#endif
	}
	void TextureShared::fromXml(const XmlTree &xml)
	{
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		// retrieve attributes specific to this type of texture
		mTopDown = xml.getAttributeValue<bool>("topdown", "false");
		mPath = xml.getAttributeValue<string>("path", "");
		mName = "shared";
	}
	XmlTree	TextureShared::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("topdown", mTopDown);
		return xml;
	}

	ci::gl::Texture2dRef TextureShared::getTexture() {
#if defined( CINDER_MSW )
		unsigned int width, height;

		// -------- SPOUT -------------
		if (!initialized) {

			// This is a receiver, so the initialization is a little more complex than a sender
			// The receiver will attempt to connect to the name it is sent.
			// Alternatively set the optional bUseActive flag to attempt to connect to the active sender. 
			// If the sender name is not initialized it will attempt to find the active sender
			// If the receiver does not find any senders the initialization will fail
			// and "CreateReceiver" can be called repeatedly until a sender is found.
			// "CreateReceiver" will update the passed name, and dimensions.
			SenderName[0] = NULL; // the name will be filled when the receiver connects to a sender
			width = g_Width; // pass the initial width and height (they will be adjusted if necessary)
			height = g_Height;

			// Optionally set for DirectX 9 instead of default DirectX 11 functions
			// spoutreceiver.SetDX9(true);  

			// Initialize a receiver
			if (spoutreceiver.CreateReceiver(SenderName, width, height, true)) { // true to find the active sender
				// Optionally test for texture share compatibility
				// bMemoryMode informs us whether Spout initialized for texture share or memory share
				bMemoryMode = spoutreceiver.GetMemoryShareMode();

				// Is the size of the detected sender different from the current texture size ?
				// This is detected for both texture share and memoryshare
				if (width != g_Width || height != g_Height) {
					// Reset the global width and height
					g_Width = width;
					g_Height = height;
					// Reset the local receiving texture size
					mTexture = gl::Texture::create(g_Width, g_Height);
					mWidth = g_Width;
					mHeight = g_Height;
				}
				initialized = true;
			}
			else {
				// Receiver initialization will fail if no senders are running
				// Keep trying until one starts
			}
		} // endif not initialized
		// ----------------------------
		if (initialized) {
			if (spoutreceiver.ReceiveTexture(SenderName, width, height, mTexture->getId(), mTexture->getTarget())) {
				//  Width and height are changed for sender change so the local texture has to be resized.
				if (width != g_Width || height != g_Height) {
					// The sender dimensions have changed - update the global width and height
					g_Width = width;
					g_Height = height;
					// Update the local texture to receive the new dimensions
					mTexture = gl::Texture::create(g_Width, g_Height);
					// reset render window
					setWindowSize(g_Width, g_Height);
					return mTexture; // quit for next round
				}
				// received OK
			}
		}
#endif
#if defined( CINDER_MAC )
		mClientSyphon.draw(vec2(0.f, 0.f));
#endif
		return mTexture;
	}
	TextureShared::~TextureShared(void) {
#if defined( CINDER_MSW )
		spoutreceiver.ReleaseReceiver();
#endif

	}

	/*
	** ---- TextureAudio ------------------------------------------------
	*/
	TextureAudio::TextureAudio(VDAnimationRef aVDAnimation) {
		mVDAnimation = aVDAnimation;
		mType = AUDIO;
		initialized = false;
		mName = "audio";

		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
		mTexture = gl::Texture::create(dTexture, GL_RED, 512, 2, fmt);

		mVDAnimation->controlValues[13] = 1.0f; //audioMultFactor
		/*mData = new float[1024];
		for (int i = 0; i < 1024; i++)
		{
		mData[i] = 0;
		}
		for (int i = 0; i < 4; i++)
		{
		iFreqs[i] = i;
		}*/
	}
	XmlTree	TextureAudio::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("topdown", mTopDown);
		xml.setAttribute("uselinein", mVDAnimation->mUseLineIn);

		return xml;
	}

	void TextureAudio::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		// retrieve attributes specific to this type of texture
		mTopDown = xml.getAttributeValue<bool>("topdown", "false");
		mVDAnimation->mUseLineIn = xml.getAttributeValue<bool>("uselinein", "true");
		mName = (mVDAnimation->mUseLineIn) ? "line in" : "wave";
		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
		mTexture = gl::Texture::create(dTexture, GL_RED, 512, 2, fmt); 
	}
	void TextureAudio::loadFromFullPath(string aPath)
	{
		CI_LOG_V("TextureAudio::loadFromFullPath: " + aPath);
		try {
			if (fs::exists(aPath)) {
				mName = aPath;
				auto ctx = audio::master();
				mSourceFile = audio::load(loadFile(aPath), audio::master()->getSampleRate());
				mSamplePlayerNode = ctx->makeNode(new audio::FilePlayerNode(mSourceFile, false));
				mSamplePlayerNode->setLoopEnabled(false);
				mSamplePlayerNode >> mMonitorWaveSpectralNode >> ctx->getOutput();
				mSamplePlayerNode->enable();

				mSamplePlayerNode->seek(0);

				auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>(mSamplePlayerNode);
				CI_ASSERT_MSG(filePlayer, "expected sample player to be either BufferPlayerNode or FilePlayerNode");

				filePlayer->setSourceFile(mSourceFile);

				mSamplePlayerNode->start();
				mVDAnimation->mUseLineIn = false;
			}
		}
		catch (...) {
			CI_LOG_W("could not open wavefile");
		}
	}

	ci::gl::Texture2dRef TextureAudio::getTexture() {

		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		if (!initialized) {
			CI_LOG_V("TextureAudio::getTexture() init");
			auto ctx = audio::Context::master();
			if (mVDAnimation->mUseLineIn) {
				// linein
				CI_LOG_W("trying to open mic/line in, if no line follows in the log, the app crashed so put UseLineIn to false in the textures.xml file");
				mLineIn = ctx->createInputDeviceNode(); //crashes if linein is present but disabled, doesn't go to catch block
				CI_LOG_V("mic/line in opened");

				auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(512).windowSize(512);
				mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
				mLineIn >> mMonitorLineInSpectralNode;
				mLineIn->enable();
			}
			// also initialize wave monitor
			auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(2048).windowSize(1024);
			mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));

			ctx->enable();
			initialized = true;
		}

		if (mVDAnimation->mUseLineIn) {
			mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
		}
		else {
			if (mSamplePlayerNode) mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
		}

		if (!mMagSpectrum.empty()) {

			unsigned char signal[kBands];
			mVDAnimation->maxVolume = 0.0f;//mIntensity
			size_t mDataSize = mMagSpectrum.size();
			if (mDataSize > 0 && mDataSize < 2048) {
				float db;
				for (size_t i = 0; i < mDataSize; i++) {
					float f = mMagSpectrum[i];
					db = audio::linearToDecibel(f);
					f = db *mVDAnimation->controlValues[13];// audioMultFactor;
					if (f > mVDAnimation->maxVolume)
					{
						mVDAnimation->maxVolume = f;
					}
					//mData[i] = f;
					int ger = f;
					signal[i] = static_cast<unsigned char>(ger);
					switch (i) {
					case 11:
						mVDAnimation->iFreqs[0] = f;
						break;
					case 13:
						mVDAnimation->iFreqs[1] = f;
						break;
					case 15:
						mVDAnimation->iFreqs[2] = f;
						break;
					case 18:
						mVDAnimation->iFreqs[3] = f;
						break;
					case 25:
						mVDAnimation->iFreqs[4] = f;
						break;
					case 30:
						mVDAnimation->iFreqs[5] = f;
						break;
					case 35:
						mVDAnimation->iFreqs[6] = f;
						break;
					default:
						break;
					}
				}
				// store it as a 512x2 texture
				mTexture = gl::Texture::create(signal, GL_RED, 512, 2, fmt);
			}
		}
		else {
			// generate random values
			for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
			mTexture = gl::Texture::create(dTexture, GL_RED, 512, 2, fmt);
		}

		return mTexture;
	}
	TextureAudio::~TextureAudio(void) {
	}

} // namespace VideoDromm
