#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <Commctrl.h>
#include "Commdlg.h"


class Video_OP
{

//Constructor & Destructor
public:
	Video_OP():my_on_off(true)
	{}
	virtual ~Video_OP(){}

//Methods

	// loads videofile and assigns a pointer to it, returns true, when successful
	bool Get_Video_from_File(char* file_name);
	//returns frame rate (frames per sec) 
	int Get_Frame_Rate();
	//returns total number of frames
	int Get_Total_Frames();
	//returns width of captured video
	int Get_Width();
	//returns height of captured video
	int Get_Height();
	//returns index of current frame
	int Get_Current_Frame_Index();
	//moves video to frame 
	void Go_to_Frame(int);
	//breaks loop presenting movies
	void Stop_Video();
	//sets up loop to present video
	void Play_Video();
	//sets up loop that runs through movie 'from 'a frame 'to' a frame
	void Play_Video(int from, int to);
    //turns video into avi file; from=first frame to=last frame
	void Write_Video(int from, int to, char* path);
	//subtracts pixel info of current frame from previous frame
	void Subtract_Successive_Frames(int from,int to);
	//calculates optical flow according to the Lucas-Kanade technique 
	void Calc_optical_flow_Lucas_Kanade(int from, int to);

	//EXTRA CODE WHICH IS NOT USED IN APPLICATION
	//calculates contours of image
	void Depict_Contours();
	//releases CvCapture Structure 
	void Unload_Video();
	//gets current frame (as image) from loaded movie and returns index
	int Query_Frame();
	//returns pointer to cvCapture structure
	CvCapture* Get_Pointer_to_Video();
	//returns pointer to grabbed frame
	IplImage* Get_Pointer_of_grabbed_frame();
	//grabs frame of loaded movie 
    void Grab_frame(int at_pos);
	

//Variables
protected:


	CvCapture * my_p_capture;
	char* my_file_name;
	bool my_on_off;
	
	int my_total_frame;
	IplImage  *my_grabbed_frame; 
	IplImage *my_first_frame;
	

};

//###### FILE DIALOG CLASS DEFINITIONS ##########
//Common File Dialog
class FileDialog
{
public: 
//Konstruktor
	FileDialog(HWND parent_handle);
	~FileDialog(){}
//Methods
   bool OpenFile();
   bool SaveFile();
   //returns Char Array of selected path
   char* GetName();
   void PrintPathName(int posX, int posY);
   
private:
	OPENFILENAME my_ofn;
	HWND my_hwnd;
	char szFileName[MAX_PATH];
	char *p_szFileName;
	
};