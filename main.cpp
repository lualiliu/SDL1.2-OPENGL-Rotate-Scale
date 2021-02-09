/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_opengl.h"
#include <string>

//Rotate
#define OPENGL_ROT 1
#ifdef OPENGL_ROT
#include <GL/gl.h>
#endif
//Screen attributes
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const int SCREEN_BPP = 32;

//The surfaces;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *g_screen = NULL;
//The event structure
SDL_Event event;

//The font that's going to be used
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 123, 21, 123 };

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    #ifndef OPENGL_ROT
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    #else
    screen = SDL_SetVideoMode( SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_BPP, SDL_SWSURFACE|SDL_OPENGL);
    g_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0,0,0,0);
    #endif

    #ifdef OPENGL_ROT
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	/* This allows alpha blending of 2D textures with the scene */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, screen->w, screen->h);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)screen->w, (GLdouble)screen->h, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    glRotatef(90,0,0,-1);
    glTranslatef(-SCREEN_WIDTH,0,0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	/* http://risky-safety.org/~zinnia/sdl/sourcetour/extra1/ */
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4d(1.0f, 1.0f, 1.0f, 1.0f);
    #endif

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "SDL1.2 OSPENGL Scale && Rotate", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "Cubes.jpg" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 28 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( message );

    //Close the font that was used
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Render the text
    message = TTF_RenderText_Solid( font, "Sauce is nice.", textColor );

    //If there was an error in rendering the text
    if( message == NULL )
    {
        return 1;
    }

    //Apply the images to the screen
    apply_surface( 0, 0, background, g_screen );
    apply_surface( 10, 20, message, g_screen );

#ifdef OPENGL_ROT
	const float s_fDrawRate = 1.0f;
	GLuint texture;
	int dx, dy, dw, dh, sx, sy, sw, sh;
	int x, y, w, h;
	float texMinX, texMaxX, texMinY, texMaxY;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGB,
		     g_screen->w, g_screen->h,
		     0,
		     GL_RGB,
		     GL_UNSIGNED_SHORT_5_6_5,
		     g_screen->pixels);
		     
	dx = 0;
	dy = 0;
	dw = screen->h;    //w->h
	dh = screen->w;    //h->w
	sx = 0;
	sy = 0;
	sw = g_screen->w;
	sh = g_screen->h;
	
	x = (int)((float)dx * s_fDrawRate);
	y = (int)((float)dy * s_fDrawRate);
	w = (int)((float)(dw + dx) * s_fDrawRate) - x;
	h = (int)((float)(dh + dy) * s_fDrawRate) - y;

	texMinX = (float) sx       / (float)g_screen->w;
	texMaxX = (float)(sx + sw) / (float)g_screen->w;
	texMinY = (float) sy       / (float)g_screen->h;
	texMaxY = (float)(sy + sh) / (float)g_screen->h;

	/* Show the image on the screen */
	glBindTexture(GL_TEXTURE_2D, texture);	

	glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
	glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
	glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
	glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
    
	glEnd();

	SDL_GL_SwapBuffers( );
	
#else

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }
#endif
    //While the user hasn't quit
    while( quit == false )
    {
        //While there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }

    //Free surfaces and font then quit SDL_ttf and SDL
    clean_up();

    return 0;
}
