#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/Tween.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm {

typedef std::shared_ptr<class VDTriangle>      VDTriangleRef;
typedef std::vector<VDTriangleRef>             VDTriangleList;
typedef VDTriangleList::iterator               VDTriangleIter;
typedef VDTriangleList::const_iterator         VDTriangleConstIter;
typedef VDTriangleList::reverse_iterator       VDTriangleReverseIter;
typedef VDTriangleList::const_reverse_iterator VDTriangleConstReverseIter;

class VDTriangle : public std::enable_shared_from_this<VDTriangle> {

  public:
	VDTriangle();
	virtual ~VDTriangle() {}

	//! returns a shared pointer to this VDTriangle
	VDTriangleRef getPtr() { return shared_from_this(); }
	static VDTriangleRef create() { return std::make_shared<VDTriangle>(); }

	//! to json
	ci::JsonTree toJson() const;
	//! from json
	virtual void fromJson(const ci::JsonTree &json);
	unsigned int					getAFboIndex() { return mAFboIndex; };
	unsigned int					getBFboIndex() { return mBFboIndex; };
	unsigned int					getAShaderIndex() { return mAShaderIndex; };
	unsigned int					getBShaderIndex() { return mBShaderIndex; };
	std::string						getAShaderFilename() { return mAShaderFilename; };
	std::string						getBShaderFilename() { return mBShaderFilename; };
	ci::Anim<float>					ABCrossfade;	// from 0 A to 1 B can be automated via timeline
	unsigned int					getMixFboIndex() { return mMixFboIndex; };
	std::string						getName() { return mName; };
	bool							isActive() { return mActive; };
	void							toggleTriangleActive() { mActive = !mActive; };
	bool							isDeleted() { return mDeleted; };
	void							toggleDeleteTriangle() { mDeleted = !mDeleted; };
	void							setAFboIndex(unsigned int aFboIndex) { mAFboIndex = aFboIndex; };
	void							setBFboIndex(unsigned int aFboIndex) { mBFboIndex = aFboIndex; }; 
	void							setAShaderIndex(unsigned int aShaderIndex) { mAShaderIndex = aShaderIndex; };
	void							setBShaderIndex(unsigned int aShaderIndex) { mBShaderIndex = aShaderIndex; };
	void							setAShaderFilename(std::string aShaderFilename) { mAShaderFilename = aShaderFilename; }; 
	void							setBShaderFilename(std::string aShaderFilename) { mBShaderFilename = aShaderFilename; };
	void							setMixFboIndex(unsigned int aMixFboIndex) { mMixFboIndex = aMixFboIndex; };
	void							setName(std::string aName) { mName = aName; };
	void							setPoint(unsigned int aIndex, vec2 aPoint) { mVertices[aIndex] = aPoint; };
	//! get the width of the content in pixels
	int getWidth() const { return mWidth; };
	//! get the height of the content in pixels
	int getHeight() const { return mHeight; };
	//! get the width and height of the content in pixels
	ci::ivec2 getSize() const { return ci::ivec2( mWidth, mHeight ); };
	//! get the width and height of the content in pixels
	ci::Area getBounds() const { return ci::Area( 0, 0, mWidth, mHeight ); };
	//! set the width of the content in pixels
	void setWidth( int w ) { setSize( w, mHeight ); }
	//! set the height of the content in pixels
	void setHeight( int h ) { setSize( mWidth, h ); }
	//! set the width and height of the content in pixels
	void setSize( const ci::ivec2 &size ) { setSize( size.x, size.y ); }
	//! set the width and height of the content in pixels
	void setSize( int w, int h );

	//! draws a triangled texture
	void draw( const ci::gl::Texture2dRef &texture );
	//! draws a specific area of a triangled texture
	void draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea );
	//! draws a specific area of a triangled texture to a specific region
	//void draw( const ci::gl::Texture2dRef &texture, const ci::Area &srcArea, const ci::Rectf &destRect );
	//! draws a warped texture
	void draw(const ci::gl::Texture2dRef &texture, const ci::Area &srcArea, const ci::Rectf &destRect);
	//void setTexCoords(float x1, float y1, float x2, float y2);
	void draw();
	//! adjusts both the source area and destination rectangle so that they are clipped against the VDTriangle's content
	bool clip( ci::Area &srcArea, ci::Rectf &destRect ) const;

	//! set the width and height in pixels of the content of all VDTriangles
	static void setSize( const VDTriangleList &VDTriangles, int w, int h );
	//! set the width and height in pixels of the content of all VDTriangles
	static void setSize( const VDTriangleList &VDTriangles, const ci::ivec2 &size ) { setSize( VDTriangles, size.x, size.y ); }

	//! read a settings xml file and pass back a vector of VDTriangles
	static VDTriangleList readSettings( const ci::DataSourceRef &source );
	//! write a settings xml file
	static void writeSettings( const VDTriangleList &VDTriangles, const ci::DataTargetRef &target );

	//! read a settings json file and pass back a vector of VDTriangles
	static VDTriangleList load(const ci::DataSourceRef &source);
	//! write a settings json file
	static void save(const VDTriangleList &VDTriangles, const ci::DataTargetRef &target);

	//! handles resize events for multiple VDTriangles
	static bool handleResize( VDTriangleList &VDTriangles );
	static bool handleResize( VDTriangleList &VDTriangles, const ci::ivec2 &size );

	void resize();
	void resize( const ci::ivec2 &size );

  protected:
	int      mWidth;
	int      mHeight;
	ci::vec2 mWindowSize;
	unsigned mSelected;
	unsigned int					mAFboIndex;
	unsigned int					mAShaderIndex;		// index of the shader used by the fbo A
	unsigned int					mBFboIndex;			// index of the fbo B
	unsigned int					mBShaderIndex;		// index of the shader used by the fbo B
	unsigned int					mMixFboIndex;		// index of the fbo mixing A and B
	std::string						mName;				// name of the VDTriangle
	std::string						mAShaderFilename;	// file name of the A shader
	std::string						mBShaderFilename;	// file name of the B shader
	bool							mActive;			// active
	bool							mDeleted;			// to be deleted
	ci::vec2						mStartPt, mVertices[3];
	std::vector<ci::vec2>			mPoints;
	gl::GlslProgRef					mGlsl;
  private:
	ci::vec2 mOffset;

	//! instanced control points
	/*typedef struct Data {
		ci::vec2 position;
		float    scale;
		float    reserved;
		ci::vec4 color;

		Data() {}
		Data( const ci::vec2 &pt, const ci::vec4 &clr, float scale )
		    : position( pt )
		    , scale( scale )
		    , color( clr )
		{
		}
	} Data;*/

	ci::gl::VboRef   mInstanceDataVbo;
	ci::gl::BatchRef mInstancedBatch;
};

} // namespace VideoDromm
