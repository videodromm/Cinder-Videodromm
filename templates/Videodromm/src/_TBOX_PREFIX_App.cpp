#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// Mix
#include "VDMix.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

class _TBOX_PREFIX_App : public App {

public:

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void fileDrop(FileDropEvent event) override;
	void update() override;
	void draw() override;
	void cleanup() override;
private:
	VDMixList					mMixes;
	fs::path					mMixesFilepath;


};


void _TBOX_PREFIX_App::setup()
{
		// initialize 
	mMixesFilepath = getAssetPath("") / "mixes.xml";
	if (fs::exists(mMixesFilepath)) {
		// load textures from file if one exists
		mMixes = VDMix::readSettings(loadFile(mMixesFilepath));
	}
	else {
		// otherwise create a texture from scratch
		mMixes.push_back(VDMix::create());
	}
}
void _TBOX_PREFIX_App::fileDrop(FileDropEvent event)
{
	int index = 1;
	string ext = "";
	// use the last of the dropped files
	fs::path mPath = event.getFile(event.getNumFiles() - 1);
	string mFile = mPath.string();
	int dotIndex = mFile.find_last_of(".");
	int slashIndex = mFile.find_last_of("\\");

	if (dotIndex != std::string::npos && dotIndex > slashIndex) ext = mFile.substr(mFile.find_last_of(".") + 1);

	if (ext == "png" || ext == "jpg")
	{
		//mFbos[0]->loadImageFile(index, mFile);
	}
	else if (ext == "glsl")
	{		
		int rtn = mMixes[0]->loadFboFragmentShader(mFile, true);//right = true
	}
}
void _TBOX_PREFIX_App::update()
{

}
void _TBOX_PREFIX_App::cleanup()
{

	// save mix settings
	VDMix::writeSettings(mMixes, writeFile(mMixesFilepath));

	quit();
}
void _TBOX_PREFIX_App::mouseDown(MouseEvent event)
{
	mMixes[0]->setZoom((float)event.getX() / 640.0f);
	mMixes[0]->setCrossfade((float)event.getY() / 480.0f);

}
void _TBOX_PREFIX_App::mouseMove(MouseEvent event)
{
	mMixes[0]->setPosition(event.getX(), event.getY());

}

void _TBOX_PREFIX_App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::draw(mMixes[0]->getRightFboTexture(), Rectf(0, 0, 128, 128));
	gl::draw(mMixes[0]->getLeftFboTexture(), Rectf(128, 0, 256, 128));
	gl::draw(mMixes[0]->getFboTexture(0), Rectf(256, 0, 384, 128));
	gl::draw(mMixes[0]->getFboTexture(1), Rectf(384, 0, 512, 128));
	gl::draw(mMixes[0]->getFboInputTexture(0,0), Rectf(0, 128, 128, 256));
	gl::draw(mMixes[0]->getTexture(), Rectf(128, 128, 256, 256));
}


CINDER_APP(_TBOX_PREFIX_App, RendererGl)
