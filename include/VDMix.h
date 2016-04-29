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
// Fbos
#include "VDFbo.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDMix instance
	typedef std::shared_ptr<class VDMix> 	VDMixRef;
	typedef std::vector<VDMixRef>			VDMixList;

	class VDMix : public std::enable_shared_from_this < VDMix > {
	public:
		typedef enum { MIX } MixType;
		VDMix(MixType aType = MIX);
		~VDMix(void);
		static VDMixRef create() { return std::make_shared<VDMix>(); }
		//! returns a shared pointer to this fbo
		VDMixRef						getPtr() { return shared_from_this(); }
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		//! returns the type
		MixType							getType() { return mType; };
		std::string						getName();
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };
		int								getTextureWidth();
		int								getTextureHeight();
		// textures
		void							loadImageFile(string aFile, unsigned int aFboIndex, unsigned int aTextureIndex, bool right);
		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		XmlTree							toXml() const;
		//! read a xml file and pass back a vector of VDMixs
		static VDMixList				readSettings(const ci::DataSourceRef &source);
		//! write a xml file
		static void						writeSettings(const VDMixList &VDMixlist, const ci::DataTargetRef &target);
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// shader
		int								loadFboFragmentShader(string aFilePath, bool right);
		// fbos
		ci::gl::Texture2dRef			getTexture();
		ci::gl::Texture2dRef			getRightFboTexture();
		ci::gl::Texture2dRef			getLeftFboTexture();
		ci::gl::Texture2dRef			getFboTexture(unsigned int aFboIndex);
		ci::gl::Texture2dRef			getFboInputTexture(unsigned int aFboIndex, unsigned int aFboInputTextureIndex);
		int								getFboTextureWidth(unsigned int aFboIndex);
		int								getFboTextureHeight(unsigned int aFboIndex);
		unsigned int					getInputTexturesCount(unsigned int aFboIndex);
		unsigned int					getFboCount() { return mFbos.size(); };
		string							getFboName(unsigned int aFboIndex);
		string							getInputTextureName(unsigned int aFboIndex, unsigned int aTextureIndex);
		// uniforms
		void							setCrossfade(float aCrossfade);
	protected:
		std::string						mName;
		bool							mFlipV;
		bool							mFlipH;
		MixType							mType;
		std::string						mFbosPath;
		//bool							mTopDown;
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		//! default vertex shader
		std::string						mPassthruVextexShaderString;
		//! default fragment shader
		std::string						mMixFragmentShaderString;
		//! mix shader
		gl::GlslProgRef					mMixShader;
		// include shader lines
		std::string						shaderInclude;
		string							mError;
		// uniforms
		vec3							iChannelResolution0;
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		//! Fbo
		gl::FboRef						mMixFbo, mLeftFbo, mRightFbo;
		void							renderLeftFbo();
		void							renderRightFbo();
		VDFboList						mFbos;
		fs::path						mFbosFilepath;
		//! Shaders
		string							mMixShaderName;
	};
}
