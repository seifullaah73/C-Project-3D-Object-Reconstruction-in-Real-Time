#include<vector>
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<cstdlib>
#include<iostream>
#include<omp.h>
//#include<glut.h>
#include<GL/freeglut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include"reconstruction.h"

using namespace cv;
using namespace std;

bool finish=false;

//Initializes 3D rendering
void initRendering() {
    //Makes 3D drawing work when something is in front of something else
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	glShadeModel(GL_SMOOTH); //Enable smooth shading
}

//Called when the window is resized
void handleResize(int w, int h) {
    //Tell OpenGL how to convert from coordinates to pixel values
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
    
    //Set the camera perspective
    glLoadIdentity(); //Reset the camera
    gluPerspective(45.0,                  //The camera angle
                   (double)w / (double)h, //The width-to-height ratio
                   1.0,                   //The near z clipping coordinate
                   200.0);                //The far z clipping coordinate
}

float _angle = -70.0f;
vector<Point3f> vertexpoints;
const float camlen=480.0;
const float camwidth=640.0;
bool finalrender;

//Draws the 3D scene
void drawScene() {
    //Clear information from last draw
	float vx=0.0,vy=0.0,vz=0.0,size=0.0;
	float vertsize=float(vertexpoints.size());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0f, 0.0f, -8.0f);
	
	//Add ambient light
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//Add positioned light
	GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//Add directed light
	GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	// dont need if only one shape glPushMatrix();  //opengl uses matrices, saves the transform state
	
	glRotatef(_angle, 0.0f, 1.0f, 0.0f);	//rotate by 70 degree anticlockwise
	glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 0.0f);	//set color of shape orangey
	if(finalrender==false)
	{
		glBegin(GL_POINTS); //Begin points coordinates		
		for(int v=0;v<vertsize;v++)
		{
			vx=float(vertexpoints[v].x);
			vy=float(vertexpoints[v].y);
			vz=float(vertexpoints[v].z);
			glVertex3f((-1.5+(3.0*(vx/30.0))),(-1.0+(2.0*(vy/30.0))),(-1.5+(3.0*(vz/30.0))));
			//idea behind this is to limit the size between -1.5 to 1.5 for x and z direction and -1 to 1 for y direction
		}
		
		glEnd(); //End points coordinates
	}
	else
	{
		glBegin(GL_QUADS);	
		for(int v=0;v<vertsize;v++)
		{
			//front
			vx=float(vertexpoints[v].x);
			vy=float(vertexpoints[v].y);
			vz=float(vertexpoints[v].z);
			if((vx>0)&&(vy>0)&&(vz>0))
			{		
				//front
				glNormal3f(0.0f, 0.0f, 1.0f);
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
			
				//Right
				glNormal3f(1.0f, 0.0f, 0.0f);
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
		    
				//Back
				glNormal3f(0.0f, 0.0f, -1.0f);
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
		    			    
				//Left
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				
				//top
				glNormal3f(0.0f, 1.0f, 0.0f);
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((1.0f+(2.0*(vx/30.0)))+((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));

				//bottom
				glNormal3f(0.0f, -1.0f, 0.0f);
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)));
				glVertex3f(((1.5f+(3.0*(vx/30.0)))+((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));
				glVertex3f(((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)), ((-1.0f+(2.0*(vx/30.0)))-((1/30.0)/2)), ((-1.5f+(3.0*(vx/30.0)))-((1/30.0)/2)));				
				
				glEnd();
	
			}
		}
	}
	glutSwapBuffers(); //Send the 3D scene to the screen
}

void Update(int value)
{
	_angle+=2.0f; //increase by 20 degrees 
	if(_angle > 360){
		_angle -= 360;
	}

	glutPostRedisplay(); //redisplay new picture
	glutTimerFunc(25, Update, 0); //call update function every 25ms.decrease and it will call it quicker and will rotate faster
}

						
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(400,400);

	Mat image;
	Mat Rot, Tran;
	int numcorners;
	
	Mat icovar;
	Scalar meanmat;
	double covar[3][3]={{35.2189, 146.3495, 105.9640},{146.3495,801.1402,527.6974},{105.9640,527.6974,553.3654}};
	meanmat[0]=15.5662;
	meanmat[1]=118.3597;
	meanmat[2]=48.5153;

	Mat covmat(3,3,CV_64F,covar);
	
	Mat mask = Mat::zeros(camlen, camwidth, CV_8UC1);	//create matrix same size as image which is 480 by 640 based on the webcam capture
	icovar=inversemat(covmat);		//determinant of covariance matrix is zero. SOLVED
	
	float distance = 250;
	int elemsize=3;
	
	Mat element = getStructuringElement(0, Size( 2*elemsize + 1, 2*elemsize+1 ), Point( elemsize, elemsize ) );
	
	Mat corners;
	cout<<"3D Image Reconstruction Program v1.0"<<endl;
	cout<<"-----------------------------------------------------------"<<endl;
	cout<<"Produced by: Seifullaah Sherrief"<<endl;
	cout<<"\nWelcome to the 3D Image reconstruction program, which will convert an object"<<endl;
	cout<<"into a 3D model. You will require the calibration chart with the four marked "<<endl;
	cout<<"circle corners, an object and a green coloured background to place the chart "<<endl;
	cout<<"and object in front. please refer to the User manual for operational\ninstructions."<<endl;
	cout<<"Press 'r' when ready to render and press 'esc' for the final 3D model"<<endl;
	cout<<"-------------------------------------------------------------------------"<<endl;
		
	vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;

	vector<Point3f> obj;
	vector<Point2f> img;
    
	vector<Point3f> threedpoint;
	vector<Point2f> projectedpoints;
	
	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;
	
	intrinsic.ptr<float>(0)[0] = 1;
	intrinsic.ptr<float>(1)[1] = 1;
	Mat silhouette;
	int objtemp=0;
	VideoCapture webcam;
	webcam.open(-1);	
	
	bool render=false;
	int tempry=0;

	//distance for note purpose
	//rectangle horizontally dot to dot 2620 vertically 1750mm
	//square horizontally dot to do 1733 vertically 1750mm
	
	//int sz[] = {lenx,leny,lenz};
	//Mat threedimension(3,sz,CV_32F,Scalar::all(1.0));  //create 3dim matrix, type 32 filled with 1s.
	double threedimension[30][30][30];
    
	for(int i=0; i<30; i++)
	{
		for(int j=0; j<30; j++)
		{
			for(int k=0;k<30;k++)
			{
				threedimension[i][j][k]=1.0;
			}
		}
	}

	cout<<"Enter number of corners to detect (must be greater than 4) e.g 5: "<<endl;
    cin>>numcorners;
	
	if(!webcam.isOpened())
	{
		cout<<"\nThe Camera is being used by another application, make sure all applications using the camera are closed and try running this program again."<<endl;
		system("PAUSE");
		return 0;
	}

	obj.push_back(Point3f(0,0,0));
	obj.push_back(Point3f(30.0,0,0));
	obj.push_back(Point3f(0,30.0,0));
	obj.push_back(Point3f(30.0,30.0,0));
	
	glutCreateWindow("Temporary Visual of 3D Model");
	initRendering();
	while(1)
	{
		//copy webcam stream to image
		webcam>>image;
		glutKeyboardFunc(handleKeypress);
		glutReshapeFunc(handleResize);
		int key=waitKey(1);
		if(key=='r'){render=true;}
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				silhouette=imagesegmentation(image,icovar,meanmat,distance,mask,element); 	
			}
			#pragma omp section
			{
				corners=Cornerdetect(image,corners,numcorners);
			}			
		}

		if(corners.rows>0)
		{
			for(int i=0;i<4;i++)
			{
					//draws circle on image, at centre at point, color, thickness, line type, 
					circle(image,corners.at<Point2f>(i),3,CV_RGB(255,0,0),1,8,0);
					//obj.push_back(Point3f(float(objtemp/2), float(objtemp%2), 0.0f));		//setting up the units of calibration
					img.push_back(corners.at<Point2f>(i));		
					objtemp++;
			}
			if(objtemp==4)
			{
				image_points.push_back(img);
				object_points.push_back(obj);
				calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs); 
				Rot=rvecs[0];
				Tran=tvecs[0];
								
				if(render)
				{
					for(float l=0.0;l<30.0;l++)
					{
						for(float w=0.0;w<30.0;w++)
						{
							for(float h=0.0;h<30.0;h++)
							{
								threedpoint.push_back(Point3f(l,w,h));		
							}
						}
					}
				
					projectPoints(threedpoint,Rot,Tran,intrinsic,distCoeffs,projectedpoints);

					// Allocate the vector
					vertexpoints.resize(projectedpoints.size(),Point3f(0,0,0));

					#pragma omp parallel for
					for(int index=0;index<projectedpoints.size();index++)
					{
						int dx = threedpoint[index].x, dy=threedpoint[index].y,dz=threedpoint[index].z;
						double check = threedimension[dx][dy][dz];
						if(check==1)
						{
							vertexpoints[index] = Point3f(dx,dy,dz);
							if(float(mask.at<uchar>(projectedpoints[index]))==255.0)
							{
								threedimension[dx][dy][dz]=0.0;
							}							
						}
					}
					glutDisplayFunc(drawScene);
					glutTimerFunc(25, Update, 0); //call update function every 25ms.decrease and it will call it quicker and will rotate faster
					glutMainLoopEvent();	//from here you can specifiy your own glutmainloop event which ones to do, this will do one iteration and continue through
											//out the while loop hopefully
					if(tempry==1)
					{break;}
					tempry++;
				}
				
			}
			imshow("original", image);
			waitKey(30);
			imshow("mask",mask);
			waitKey(30);			//this is to give the processor some time to display the image
			//rendering over here with the displays, loop through each points which is a one to get a list of vectors (vertexpoints) then draw using those points by looping through it
			//after rendering clear it so it doesn't stack on top		
		}
		objtemp=0;
		img.clear();
		image_points.clear();
		object_points.clear();
		vertexpoints.clear();

	}
	webcam.release();
	destroyWindow("original");
	destroyWindow("mask");
	cout<<"The Final 3D Model is Being Rendered and will be displayed Shortly."<<endl;
	cout<<"\nThank you for using the 3D Image Reconstruction Software"<<endl;
	finalrender=true;
	//finalrender is for switching between different rendering 
	//check to see if any of the neighbour voxels are zero and then just push those vertices to the vertex and render triangle fan
	//we have the final 3d array
	
	glutReshapeFunc(handleResize);
	glutDisplayFunc(drawScene);
	glutTimerFunc(25, Update, 0); //call update function every 25ms.decrease and it will call it quicker and will rotate faster
	glutMainLoop();
	return 0;
}