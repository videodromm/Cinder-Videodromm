
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"


using namespace ci;
using namespace ci::app;

class _TBOX_PREFIX_App : public App {

public:

    void setup();
    void draw();
    void update();
    void mouseDown(MouseEvent event);
};

// -------- SPOUT -------------
void _TBOX_PREFIX_App::setup()
{
    
}

void _TBOX_PREFIX_App::update()
{
    

}



void _TBOX_PREFIX_App::mouseDown(MouseEvent event)
{
    
}
// ----------------------------


void _TBOX_PREFIX_App::draw()
{

   
}


CINDER_APP(_TBOX_PREFIX_App, RendererGl)
