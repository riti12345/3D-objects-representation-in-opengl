# 3D-objects-representation-in-opengl


#include <windows.h>
#include <gl/gl.h>



#include <math.h>

int	main_window;

#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4

#define FILL 1
#define LINE 2

#define SHRINK 1
#define NORMAL 2

float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;


// Pop up menu identifiers
int fillMenu, shrinkMenu, mainMenu, colorMenu;

// color 
float red = 1.0f, blue=0.5f, green=0.5f;

// scale 
float scale = 1.0f;

// menu status
int menuFlag = 0;
// the camera info  
float eye[3];
float lookat[3];


// pointers for all of the glui controls
GLUI *glui;
GLUI_Rollout		*object_rollout;
GLUI_RadioGroup		*object_type_radio;
GLUI_Rotation		*object_rotation;
GLUI_Translation	*object_xz_trans;
GLUI_Translation	*object_y_trans;

GLUI_Rollout		*anim_rollout;
GLUI_Button			*action_button;

GLUI_Checkbox *draw_floor_check;
GLUI_Checkbox *draw_object_check;
GLUI_Checkbox *use_depth_buffer;

#define CB_DEPTH_BUFFER 0
#define CB_ACTION_BUTTON 1
#define CB_RESET 2

GLfloat step = 0;
GLfloat live_anim_speed = 3;

GLfloat live_anim_color =red;
int live_object_type;
float live_object_rotation[16];
float live_object_xz_trans[2];
float live_object_y_trans;
int live_draw_polygon;
int live_draw_rectangle;
int live_draw_triangle;
int live_draw_square;
int live_use_depth_buffer;






void myGlutIdle(void)
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	glutPostRedisplay();
}


int current_button = -1;
int last_x;
int last_y;

// catch mouse up/down events
void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		current_button = button;
	else
	{
		if (button == current_button)
			current_button = -1;
	}

	last_x = x;
	last_y = y;
}

// catch mouse move events
void myGlutMotion(int x, int y)
{
	// the change in mouse position
	int dx = x-last_x;
	int dy = y-last_y;

	float scale, len, theta;
	float neye[3], neye2[3];
	float f[3], r[3], u[3];

	switch(current_button)
	{
	case GLUT_RIGHT_BUTTON:
		

		break;

	case GLUT_MIDDLE_BUTTON:
		


		break;

	case GLUT_LEFT_BUTTON:
		// rotate

		

		break;
	}


	last_x = x;
	last_y = y;

	glutPostRedisplay();
}


// -----------------------------------
//             KEYBOARD
// -----------------------------------

void processNormalKeys(unsigned char key, int xx, int yy) {

	glutSetMenu(mainMenu);
	switch (key) {
		case 27:
			glutDestroyMenu(mainMenu);
			glutDestroyMenu(fillMenu);
			glutDestroyMenu(colorMenu);
			glutDestroyMenu(shrinkMenu);
			exit(0);
			break;

		case 's':
			if (!menuFlag)
			  glutChangeToSubMenu(2,"Shrink",shrinkMenu);
			break;
		case 'c':
			if (!menuFlag)
				glutChangeToSubMenu(2,"Color",colorMenu);
			break;
              
	}
	if (key == 27)
		exit(0);
      
}

void pressKey(int key, int xx, int yy) {

	switch (key) {
		case GLUT_KEY_UP : deltaMove = 0.5f; break;
		case GLUT_KEY_DOWN : deltaMove = -0.5f; break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_UP :
		case GLUT_KEY_DOWN : deltaMove = 0;break;
	}
}

// -----------------------------------
//             MENUS
// -----------------------------------

void processMenuStatus(int status, int x, int y) {

	if (status == GLUT_MENU_IN_USE)
		menuFlag = 1;
	else
		menuFlag = 0;
}

void processMainMenu(int option) {

}

void processFillMenu(int option) {

	switch (option) {

		case FILL: glPolygonMode(GL_FRONT, GL_FILL); break;
		case LINE: glPolygonMode(GL_FRONT, GL_LINE); break;
	}
}

void processShrinkMenu(int option) {

	switch (option) {

		case SHRINK: scale = 0.5f; break;
		case NORMAL: scale = 1.0f; break;
	}
}

void processColorMenu(int option) {

	switch (option) {
		case RED :
			red = 1.0f;
			green = 0.0f;
			blue = 0.0f; break;
		case GREEN :
			red = 0.0f;
			green = 1.0f;
			blue = 0.0f; break;
		case BLUE :
			red = 0.0f;
			green = 0.0f;
			blue = 1.0f; break;
		case ORANGE :
			red = 1.0f;
			green = 0.5f;
			blue = 0.5f; break;
	}
}

int createPopupMenus() {

	shrinkMenu = glutCreateMenu(processShrinkMenu);

	glutAddMenuEntry("Shrink",SHRINK);
	glutAddMenuEntry("NORMAL",NORMAL);

	fillMenu = glutCreateMenu(processFillMenu);

	glutAddMenuEntry("Fill",FILL);
	glutAddMenuEntry("Line",LINE);

	colorMenu = glutCreateMenu(processColorMenu);
	glutAddMenuEntry("Red",RED);
	glutAddMenuEntry("Blue",BLUE);
	glutAddMenuEntry("Green",GREEN);
	glutAddMenuEntry("Orange",ORANGE);

	mainMenu = glutCreateMenu(processMainMenu);

	glutAddSubMenu("Polygon Mode", fillMenu);
	glutAddSubMenu("Color", colorMenu);
        glutAddSubMenu("Shrink", shrinkMenu);
	// attach the menu to the right button
	 glutAttachMenu (GLUT_RIGHT_BUTTON);

	// this will allow us to know if the menu is active
	glutMenuStatusFunc(processMenuStatus);
        return mainMenu;
}



// the window has changed shapes, fix ourselves up
void myGlutReshape(int	x, int y)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glutPostRedisplay();
}


// draw the scene
void myGlutDisplay(void )
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// projection transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 1000);
	//for setting projection tranformation(left,right,bottom,top,near,far)

	// camera transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);

	
	//
	// draw some stuff
	//

	if (live_draw_polygon)
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(red, green, blue);    // note: color is state and only needs to be set once
		glVertex3f(-10, 0, -10);
		glVertex3f( 10, 0, -10);
		glVertex3f( 10, 0,  10);
		glVertex3f(-10, 0,  10);
		glEnd();
	}
        else if(live_draw_square) 
              {
		glBegin(GL_POLYGON);
               glColor3f(red, green, blue);   
               glVertex3f(2.0,2.0,0.0);
               glVertex3f(7.0,2.0,0.0);
               glVertex3f(7.0,7.0,0.0);
               glVertex3f(2.0,7.0,0.0);
		glEnd();
	}
	 
         else if(live_draw_rectangle) 
              {
		glBegin(GL_POLYGON);
               glColor3f(red, green, blue);   
               glVertex3f(2.50,1.50,0.0);
               glVertex3f(3.50,1.50,0.0);
               glVertex3f(3.50,3.50,0.0);
               glVertex3f(2.50,3.50,0.0);
		glEnd();
	}
	

	glutSwapBuffers(); 
}

// some controls generate a callback when they are changed
void glui_cb(int control)
{

	switch(control)
	{
	case CB_DEPTH_BUFFER:
		if (live_use_depth_buffer)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		break;
	case CB_ACTION_BUTTON:
		if (step < 0)
		{
			step = 0;
			action_button->set_name("Stop");
		}
		else
		{
			step = -1;
			action_button->set_name("Start");
		}
		break;
	case CB_RESET:
		//
		// put your reset callback here
		//
		break;
	}

	glutPostRedisplay();
}
void initGL() {
   glClearColor(0.0, 0.0, 0.0, 1.0); // Set background (clear) color to black
}

// entry point
int main(int argc,	char* argv[])
{

	//
	// create the glut window
	//
         glutInit (&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(1000, 600);
	glutInitWindowPosition(100,100);
	main_window = glutCreateWindow("Glut user Interface");
        initGL();
	//
	// set callbacks
	//
	glutDisplayFunc(myGlutDisplay);
        
	glutIgnoreKeyRepeat(1);
	GLUI_Master.set_glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	


	// init Menus
	createPopupMenus();

       
       glutAttachMenu (GLUT_RIGHT_BUTTON);

	GLUI_Master.set_glutReshapeFunc(myGlutReshape);
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	
	GLUI_Master.set_glutMouseFunc(myGlutMouse);
	glutMotionFunc(myGlutMotion);


	//
	// create the interface subwindow and add widgets
	//
	glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);


	// initialize live variables
	live_object_type = 0;
	live_object_xz_trans[0] = 0;
	live_object_xz_trans[1] = 0;
	live_object_y_trans = 0;
	live_draw_polygon = 1;
	live_draw_rectangle = 1;
        live_draw_square = 1;
	live_use_depth_buffer = 1;

	// quit button
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	// empty space
	glui->add_statictext("");

	// the object rollout
	object_rollout = glui->add_rollout("Object");

	// the radio buttons
	object_type_radio = glui->add_radiogroup_to_panel(object_rollout, &live_object_type);
	glui->add_radiobutton_to_group(object_type_radio, "polygon");
        glui->add_radiobutton_to_group(object_type_radio, "Rectangle");
        glui->add_radiobutton_to_group(object_type_radio, "Square");
        glui->add_checkbox("x ", &live_draw_polygon);
	glui->add_checkbox("y", &live_draw_polygon);
        glui->add_checkbox("z", &live_draw_polygon);

	GLUI_Panel *transform_panel = glui->add_panel_to_panel(object_rollout, "", GLUI_PANEL_NONE);
	object_rotation = glui->add_rotation_to_panel(transform_panel, "Rotation", live_object_rotation);
	object_rotation->reset();

	glui->add_column_to_panel(transform_panel, false);
	object_xz_trans = glui->add_translation_to_panel(transform_panel, "Translate XZ", GLUI_TRANSLATION_XY,live_object_xz_trans);

	glui->add_column_to_panel(transform_panel, false);
	object_y_trans =  glui->add_translation_to_panel(transform_panel, "Translate Y", GLUI_TRANSLATION_Y,&live_object_y_trans);

	object_xz_trans->scale_factor = 0.1f;
	object_y_trans->scale_factor = 0.1f;

	glui->add_button_to_panel(object_rollout, "Reset Object Transform", CB_RESET, glui_cb);

	// empty space
	glui->add_statictext("");

	// the walk control
	anim_rollout = glui->add_rollout("Animation");

	action_button = glui->add_button_to_panel(anim_rollout, "Stop", CB_ACTION_BUTTON, glui_cb);

	GLUI_Spinner *spin_s =

		glui->add_spinner_to_panel(anim_rollout, "Speed", GLUI_SPINNER_FLOAT, &live_anim_speed);

	spin_s->set_float_limits(0.1, 10.0);

	// our checkbox options for deciding what to draw
	glui->add_checkbox("Draw Polygon", &live_draw_polygon);
	glui->add_checkbox("Draw Rectangle", &live_draw_rectangle);
        glui->add_checkbox("Draw Square", &live_draw_square);

	// empty space
	glui->add_statictext("");

	glui->add_checkbox("Use Depth Buffer", &live_use_depth_buffer, CB_DEPTH_BUFFER, glui_cb);
       
       glui->add_statictext("");
        glui->add_spinner_to_panel(anim_rollout, "Color", GLUI_SPINNER_FLOAT, &live_anim_color);
          glui->add_spinner_to_panel(anim_rollout, "Text", GLUI_SPINNER_FLOAT, &live_anim_color);
            glui->add_spinner_to_panel(anim_rollout, "Font", GLUI_SPINNER_FLOAT, &live_anim_color);
            glui->add_spinner_to_panel(anim_rollout, "Text-size", GLUI_SPINNER_FLOAT, &live_anim_speed);
	glui->set_main_gfx_window(main_window);

	// initialize the camera
	eye[0] = 0;
	eye[1] = 4;
	eye[2] = 10;
	lookat[0] = 0;
	lookat[1] = 0;
	lookat[2] = 0;

	// initialize gl
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// give control over to glut
	glutMainLoop();

	return 0;
}











