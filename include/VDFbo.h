#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// textures
#include "VDTexture.h"
// textures
#include "VDShaders.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> 	VDFboRef;
	typedef std::vector<VDFboRef>			VDFboList;

	class VDFbo : public VDTexture{
	public:
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList, VDShaderList aShaderList);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList, VDShaderList aShaderList) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation, aTextureList, aShaderList);
		}
		//! returns a shared pointer to this fbo
		VDFboRef						getPtr() { return std::static_pointer_cast<VDFbo>(shared_from_this()); }
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		//! returns the type
		TextureType						getType() { return mType; };
		std::string						getName();
		std::string						getLabel();
		//bool							isFlipH() { return mFlipH; };
		//bool							isFlipV() { return mFlipV; };
		int								getTextureWidth();
		int								getTextureHeight();
		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		XmlTree							toXml() const;
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// shader
		int								loadFragmentShader(string aFilePath);
		string							getFragmentShaderText();
		// textures
		void							setInputTexture(unsigned int aTextureIndex);
		unsigned int					getInputTextureIndex() { return inputTextureIndex; };
		ci::gl::Texture2dRef			getTexture();
		//void							loadImageFile(string aFile, unsigned int aTextureIndex);
		//void							loadAudioFile(string aFile);

	protected:
		std::string						mFboName;
		//bool							mFlipV;
		//bool							mFlipH;
		TextureType						mType;
		std::string						mFilePathOrText;
		//bool							mTopDown;
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		//! default vertex shader
		std::string						mPassthruVextexShaderString;
		//! default fragment shader
		std::string						mFboTextureFragmentShaderString;
		//! passthru shader
		gl::GlslProgRef					mFboTextureShader;
		// include shader lines
		std::string						shaderInclude;
		string							mError;
		// uniforms
		vec3							iChannelResolution0;
	private:
		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;
		//! Shaders
		VDShaderList					mShaderList;
		unsigned int					mShaderIndex;
		//! Fbo
		gl::FboRef						mFbo;
		//! Textures
		VDTextureList					mTextureList;
		unsigned int					inputTextureIndex;
		//! Shaders
		string							mShaderName;
		string							mFragmentShaderText;
		string							mId;
		
	};
}
