#include "OpenCV_Video.h"


bool Video_OP::Get_Video_from_File(char* file_name)
{

   // checks if filename is available
	if(!file_name)
		return false;
   my_p_capture = cvCaptureFromFile(file_name);
   // checks if capturing has been successful; 
   // function returns NULL if codec for video type is not installed on machine
   if (!my_p_capture)
	   return false;
   // gets video's number of frames 
   my_total_frame =  ( int )cvGetCaptureProperty( my_p_capture,CV_CAP_PROP_FRAME_COUNT );
   return true;
}

int Video_OP::Get_Frame_Rate()
{
  // OpenCV code that returns framerate (frames per second )
   return ( int )cvGetCaptureProperty( my_p_capture, CV_CAP_PROP_FPS );
}

int Video_OP::Get_Total_Frames()
{
 // returns total length of video (in frames)
 return my_total_frame;
}

int Video_OP::Get_Current_Frame_Index()
{
  // returns the index of next frame to be decoded (as frame number)
  return (int)cvGetCaptureProperty( my_p_capture,CV_CAP_PROP_POS_FRAMES );
}

int Video_OP::Get_Height()
{
 // returns height of video in pixels
  return (int)cvGetCaptureProperty(my_p_capture, CV_CAP_PROP_FRAME_HEIGHT);
}


int Video_OP::Get_Width()
{
	//returns width of video in pixels
	return (int)cvGetCaptureProperty(my_p_capture, CV_CAP_PROP_FRAME_WIDTH);
}

void Video_OP::Go_to_Frame(int frame)
{
   // OpenCV code that drives movie to a particular frame
   int x = cvSetCaptureProperty( my_p_capture, CV_CAP_PROP_POS_FRAMES, (double) frame );

}

void Video_OP::Grab_frame(int at_pos)
{
   // grabs current frame of captured video
   // and pushes pointer to the next frame
   // so that a subsequent call returns the next frame
    this->Go_to_Frame(at_pos);
     cvNamedWindow( "grabbed frame", 0 );
     my_grabbed_frame = cvQueryFrame( my_p_capture);
	 cvShowImage( "grabbed frame", my_grabbed_frame );


}


void Video_OP::Stop_Video()
{
  //Interrupts 'while' loop
  //in Play_Movie, Opitcal flow etc.
   my_on_off = false;

}

void Video_OP::Play_Video()
{

	this->Play_Video(0,this->my_total_frame);
	 
}


void Video_OP::Play_Video(int from, int to)
{
    if(to < 0 || to <= from || from < 0){
	    return;
	    }
	if( to ==0) 
		  to = this->Get_Total_Frames();

	  my_on_off = true;
	  int key =0;
	  int frame_counter = from;
	  
	 
	 
	  // checks if file has been loaded successfully
	  if( !my_p_capture ) return;    
	   
	    // retrieves frames per second (fps) to set speed of presentation
	    int fps = this->Get_Frame_Rate();
		
		// creates window, in which movie is displayed //see below (A)
		cvNamedWindow( "video", CV_WINDOW_AUTOSIZE ); 
		cvNamedWindow("flipped video",CV_WINDOW_AUTOSIZE);

		// sets pointer to frame where the video should start from 
		 this->Go_to_Frame(from); //or alternative see below
		//int x = cvSetCaptureProperty( my_p_capture, CV_CAP_PROP_POS_FRAMES, (double) from );

		// gets first frame, to have access to video properties
		 my_grabbed_frame = cvQueryFrame( my_p_capture);
		// checks  if frame is available
	      if( !my_grabbed_frame ) return;

		IplImage* flipped_frame;
		// allocates memory for flipped image
		flipped_frame = cvCreateImage(cvSize(this->Get_Width(),this->Get_Height()), my_grabbed_frame->depth, my_grabbed_frame->nChannels);
		

		// loop, which is stopped by setting my_on_off = false; 
	      while(my_on_off == true && frame_counter <= to) { 
            // gets a frame 
	       my_grabbed_frame = cvQueryFrame( my_p_capture);
		   
	       // checks if frame is available
	        if( !my_grabbed_frame ) break;
	        
	  
		    // turns image (or frame) upside down
			cvConvertImage( my_grabbed_frame, flipped_frame,CV_CVTIMG_FLIP );

			// turns grabbed image into grey scale image
		    // cvCvtColor( my_grabbed_frame, flipped_frame, CV_RGB2GRAY );

	        // displays grabbed image and flipped image
	        cvShowImage( "flipped video", flipped_frame );
	        cvShowImage("video",my_grabbed_frame);
	       
			frame_counter++;

			// if 25 frames per second; 1000 (-> 1000 ms -> 1sec) /25 = 40 ms; 
			// means => do this every 40 ms 
	        // key = cvWaitKey( 1000 / 70 ); speeds up movie
            // key = cvWaitKey( 1000 / 10 ); slows movie down
			key = cvWaitKey(1000 /fps);
		  }

		// releases pointer for movie capturing 
	    cvReleaseCapture( &my_p_capture ); 
		
		// frees memory used for image pointer
		cvReleaseImage(&flipped_frame);
		// closes windows, in which images are depicted 
	    cvDestroyWindow("video" );
		cvDestroyWindow("flipped video" );


}

void Video_OP::Subtract_Successive_Frames(int from, int to)  
{
	  if(to < 0 || to <= from || from < 0){
	    return;
	    }
	  if( to ==0) 
		  to = this->Get_Total_Frames();

       my_on_off = true;
	   int key =0;
	   int frame_counter = from; 
	 
	  // checks if file has been loaded successfully
	  if( !my_p_capture ) return;    
	   
		 
		
		// creates window, in which movie is displayed 
		cvNamedWindow( "movie", CV_WINDOW_AUTOSIZE ); 
		// creates window that displays subtracted frames
		cvNamedWindow("subtraction",CV_WINDOW_AUTOSIZE);
		
		cvNamedWindow("previous frame",CV_WINDOW_AUTOSIZE);
		// depicts average of found pixels
		cvNamedWindow("pixels identified",CV_WINDOW_AUTOSIZE);
		// window for first frame
		//cvNamedWindow("First Frame",CV_WINDOW_AUTOSIZE);
		  
		// grabs first frame for subsequent subtraction process
		this->my_first_frame = cvQueryFrame( my_p_capture);
		  
		// retrieves frames per second (fps), to define speed of presentation
		int fps = this->Get_Frame_Rate();

		// creates pointer to grey scale image (has the properties of first frame)
		IplImage* first_frame_gray = cvCreateImage(cvSize(my_first_frame->width,my_first_frame->height), IPL_DEPTH_8U, 1);
		// turns first frame into grey scale frame
		cvCvtColor( my_first_frame, first_frame_gray, CV_RGB2GRAY );
		//not used
		//cvShowImage("first frame",my_first_frame);


		 // Image for running frame
		 IplImage* running_frame_gray = NULL;
		 // Image for subtracted frames
		 IplImage* subtracted_frame = NULL;
		 // Image pointer for cloned frame
		 IplImage* cloned_frame = NULL;
		 // clones first frame 
		 cloned_frame =cvCloneImage(first_frame_gray);
         // Image to depict mean of pixels
		 IplImage* pixels_frame = cvCreateImage(cvSize(my_first_frame->width,my_first_frame->height),  IPL_DEPTH_8U, 1);
		
		  // fills image with a predefined color (here: white)
		  cvSet(pixels_frame , CV_RGB(255,255,255),0);

		  // cvScalar contains vals [0] to [3]; [1] to [3] correspond to the RGB model
	      // in grayscale  (1 channel) images val[0] returns brightness
	      CvScalar color_channels;
		  
		  // variables to calculate mean
	       double pixel_count =0, sum_x =0, sum_y =0,mean_x =0 , mean_y =0 ;
			 

		// loop, which is stopped by setting this->my_on_off = false; see Stop_Video() method;
	      while(my_on_off == true && frame_counter <= to) { 

			  //resets variables after each loop; see below (B)
			  pixel_count =0;
			  sum_x =0;
			  sum_y = 0;
		 
              // gets a frame 
	          my_grabbed_frame = cvQueryFrame( my_p_capture);
		  

		      // checks  if frame is available
		      if( !my_grabbed_frame ) break;

		        // defines type of image arrays
		        running_frame_gray = cvCreateImage(cvSize(my_first_frame->width,my_first_frame->height), 
                 IPL_DEPTH_8U, 1);
		        subtracted_frame = cvCreateImage(cvSize(my_first_frame->width,my_first_frame->height), 
                 IPL_DEPTH_8U, 1);
		    
		       // converts grabbed frame to gray frame
		        cvCvtColor(my_grabbed_frame, running_frame_gray, CV_RGB2GRAY );

		       // substracts img(t) from img(t-1) and stores the result in subtracted_frame
                cvAbsDiff(cloned_frame,running_frame_gray, subtracted_frame );
	        

			 // loops through image array and accesses pixels with cvGetAt or cvGet2D and calculates mean (see B)
				for (double i = 0; i < subtracted_frame->width; i++){
					for (double u = 0; u < subtracted_frame->height; u++)
					{
						// gets info of pixel color (greyscale value ranging from 0 to 255 (=white))
						 color_channels =  cvGet2D(subtracted_frame,u,i);
						// 100 is threshold; if colorvalue is higher than 100, then it is used for calculations below
						 if( color_channels.val[0] > 100){
						    pixel_count++;
						   //sums up x and y coordinate of pixels above threshold
						     sum_x += i;
						     sum_y += u;
						
	                      }
					}
				 }
	
			  // calculates mean coordinates by dividing all pixles by the coordinates of each dimension
	             mean_x = sum_x/pixel_count;
	             mean_y = sum_y/pixel_count;

              // if calculation yields a useful value then a draw a circle at that spot in pixels_frame image			
			     if (mean_x < subtracted_frame->width && mean_y < subtracted_frame->height)
              // cvCircle(IplImage,CvPoint,int radius,CvColor,int thickness, int line_type, int shift);
	             cvCircle(pixels_frame, cvPoint(mean_x,mean_y ),4,cvScalar(0,255,0,0), 2,8,0);

            // counts loop steps and frames processed
	        frame_counter++;
	   
			 // displays subtracted frames in window;
			 cvShowImage("subtraction",subtracted_frame);
	        // displays unchanged movie image
	         cvShowImage( "movie", my_grabbed_frame );
        
			 cvShowImage("previous frame", cloned_frame);
			// for depicting pixels found
			 cvShowImage("pixels identified",pixels_frame);

			
			// clones current frame, so it can serve as previous frame in next step
			cloned_frame =cvCloneImage(running_frame_gray);
			
		    // releases image arrays; in next step new information will be stored in them
	        cvReleaseImage(&subtracted_frame);
		    cvReleaseImage(&running_frame_gray);
		  
			// sets time delay for loop
            key = cvWaitKey( 1000 / fps );
		  }

		// cleaning up

           cvReleaseCapture( &my_p_capture ); 

		cvReleaseImage(&cloned_frame);
		cvReleaseImage(&first_frame_gray);
	    cvReleaseImage(&pixels_frame);
	   
	   cvDestroyWindow("subtraction");
	   cvDestroyWindow("movie");
	   cvDestroyWindow("previous frame");
	   cvDestroyWindow("pixels identified");
	   

}


void Video_OP::Calc_optical_flow_Lucas_Kanade(int from, int to)
{
	 if(to < 0 || to <= from || from < 0){
	    return;
	    }
	if( to == 0) 
		  to = this->Get_Total_Frames();

	  my_on_off = true;

	  int frame_counter = from;
	  int key =0, count =0, flags =0, win_size = 10;
	  // explanations given below; 
	  IplImage *image = 0, *grey = 0,  *pyramid = 0, *prev_pyramid = 0, *swap_temp, *running_frame, *prev_grey;

	  // maximum number of points for Shi Tomasi algorithm (invoked in cvGoodFeaturesToTrack();)
	  const int MAX_COUNT = 500;
	  // CvPoint2D32f is an Array of 32 bit points to contain the features found 
	  // by the Shi Tomasi algorithm; see below cvGoodFeaturesToTrack
      CvPoint2D32f* points[2] = {0,0}, *swap_points;
	
	  // needed for Lucas-Kanade function
	  char* status = 0;

	  // checks if file has been loaded successfully
	  if( !my_p_capture ) return;    
	   
	    // retrieves frames per second (fps), to set speed of presentation
	    int fps = this->Get_Frame_Rate();

		// sets up windows to present information derived by calculations
		cvNamedWindow("flow", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("prev",CV_WINDOW_AUTOSIZE);
		cvNamedWindow("current",CV_WINDOW_AUTOSIZE);
        cvNamedWindow("first frame",CV_WINDOW_AUTOSIZE);

		 // sets up all nescessary data; cvQueryFrame to access info of video
		 running_frame = cvQueryFrame(this->my_p_capture);
		// creates grey-scale image that stores info from previous frame
		  prev_grey =  cvCreateImage( cvGetSize(running_frame), IPL_DEPTH_8U, 1 );
		 // turns image of original frame into greyscale image 
		 cvCvtColor(running_frame, prev_grey, CV_BGR2GRAY );

		  // allocates memory for further images
            image = cvCreateImage( cvGetSize(running_frame), running_frame->depth, 3 );
            image->origin = running_frame->origin;
            grey = cvCreateImage( cvGetSize(running_frame), IPL_DEPTH_8U, 1 );
            
			// Image pyramids are nescessary for the Lucas Kanade algorithm to work correctly 
			// (see book 'Learning OpenCV' on page 330)
            pyramid = cvCreateImage( cvGetSize(running_frame), IPL_DEPTH_8U, 1 );
            prev_pyramid = cvCreateImage( cvGetSize(running_frame), IPL_DEPTH_8U, 1 );

			// allocates memory for CvPoint2D32F Arrays
            points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
            points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
           
            flags = 0;

			// grabbed frame is being copied into frame as defined above
			 cvCopy( running_frame, image, 0 );
            // image is turned into grey scale picture and stored in prev_grey image
            cvCvtColor( image, prev_grey, CV_BGR2GRAY ); 

			// temporary image information and eigenvalues are stored in images below
            IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );

			// quality level (minimal eigenvalue for cut off) should not exceed 1; typical values are 0.10 or 0.01
            double quality = 0.01;
			// min_distance guarantees that two returned points are not within a certian range 
            double min_distance = 10;

            count = MAX_COUNT;
			// finds features to track and stores them in array (here: points[0])
			// most parameters are decribed above; 
			// mask (here 0) contains info about pixels the algorithm considers as useful
			// block_size (here 3) is the region around a given pixel 
			// use harris (here 0) is Harris definition; can be used used instead of Shi Tomasi (if not 0)
			// k is a weighting coefficient
            cvGoodFeaturesToTrack(prev_grey, eig, temp, points[0], &count,
                                   quality, min_distance, 0, 3, 0, 0 ); //k 0.04

			// refines search for features obtained by routines like cvGoodFeaturesToTrack()
			// first input is a single one channel grayscale image (8 bit)
			// followed by the CvPoint2D32f array filled by cvGoodFeaturesToTrack()
			// subpixel calculation requires window size from which equations will be generated
			// cv(-1,-1) means that no zero zone required (see book 'Learning OpenCV' on page 321)
			// cvTermCriteria defines accuracy of subpixel computations; when it will stop 
            cvFindCornerSubPix( prev_grey, points[0], count,
                cvSize(win_size,win_size), cvSize(-1,-1),
                cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

            cvReleaseImage( &eig );
            cvReleaseImage( &temp );

			// loop draws found features onto the first frame 
			for (int i = 0; i < count; i++)
			{
			    
		      CvPoint p1 = cvPoint( cvRound( points[0][i].x ), cvRound( points[0][i].y ) );

			  cvCircle(image,p1,1,CV_RGB(255,0,0),1,8,0);
			
			}
			cvShowImage("first frame",image);

			 // needed for cvCalcOpticalFlowPyrLK
			 status = (char*)cvAlloc(MAX_COUNT);

			// loop, which is stopped by setting on_off = false; see Stop_Video() method; 
	      while(my_on_off == true && frame_counter <= to) { 
			    
                int i, k, c;

               running_frame = cvQueryFrame( my_p_capture );
               if( !running_frame )
               break;

		       cvCopy( running_frame, image, 0 );
			   cvCvtColor( image, grey, CV_BGR2GRAY ); 
        
		       if (count > 0)
               {
				  // first arguments are current frame and previous frame (as single channel 8 bit)
                  // followed by Image arrays to store image pyramids
                  // points ([0] previous [1] current features) are CvPoint2D32F arrays which store info about motion and location of pixels
                  // window to calculate coherent motion, is given by winsize
				  // level equals (here 5) depth of pyramids used (0 would mean no pyramid)
                  // status will be 1 if point found in next image; and 0 if it is not found
                  // cvTermCriteria tells algorithm when to stop
                  cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
                  points[0], points[1], count, cvSize(win_size,win_size), 5, status, 0,
                  cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
            

			        // depicts flow by drawing lines between successive frames
			        for( int i=0; i < count; i++ ){
	                  CvPoint p0 = cvPoint( cvRound( points[0][i].x ), cvRound( points[0][i].y ) );
		              CvPoint p1 = cvPoint( cvRound( points[1][i].x ), cvRound( points[1][i].y ) );

				      // draws circle onto prev_grey image and grey image
			          cvCircle(prev_grey,p0,5,CV_RGB(0,255,0),1,8,0);
			          cvCircle(grey,p1,5,CV_RGB(0,255,0),1,8,0);

				      // draws red lines between found features of successive frames 
		              cvLine( image, p0, p1, CV_RGB(255,0,0), 2 );
			        }
	             }// end if 

			
		  cvShowImage("prev",prev_grey);
		  // grey image becomes new previous grey image for next step in loop
		  prev_grey = cvCloneImage(grey);
		  // similar operations as above, positions of pyramids and points-arrays are swapped
          CV_SWAP( prev_pyramid, pyramid, swap_temp );
          CV_SWAP( points[0], points[1], swap_points );
        
          cvShowImage( "flow", image );
		  cvShowImage("current",grey);
		

      	frame_counter++;

		key = cvWaitKey(40);
	 }//end while loop

		 // cleaning up
	    cvReleaseCapture( &my_p_capture ); 
		
		cvReleaseImage(&grey);
		cvReleaseImage(&prev_grey);
		cvReleaseImage(&pyramid);
		cvReleaseImage(&prev_pyramid);

	
	   cvDestroyWindow("flow");
	   cvDestroyWindow("prev");
	   cvDestroyWindow("current");
	   cvDestroyWindow("first frame");


}

void Video_OP::Write_Video(int from, int to, char* path)
{
	 if(to < 0 || to <= from || from < 0){
	    return;
	    }
	 if( to ==0) 
		  to = this->Get_Total_Frames();

	int key =0;
	int isColor = 1;//if 0, then frame is black white
	int frame_counter = from;
	my_on_off = true;

	 if( !my_p_capture ) 
			 return;

	this->Go_to_Frame(from );

	// query first frame to get video info
	 my_grabbed_frame = cvQueryFrame( my_p_capture);
	 
	 // check  if frame is available
	        if( !my_grabbed_frame ) return;
     
	 double fps = this->Get_Frame_Rate();

      CvSize size = cvSize(
       (int)cvGetCaptureProperty( this->my_p_capture, CV_CAP_PROP_FRAME_WIDTH) ,
       (int)cvGetCaptureProperty(this->my_p_capture, CV_CAP_PROP_FRAME_HEIGHT) );
   
   
	 cvNamedWindow("write to avi",0);
	    
	   // sets properties of image pointer which captures video frames
        IplImage* frame_to_write =  cvCreateImage(size,my_grabbed_frame->depth,my_grabbed_frame->nChannels);

	   // creates Video Writer
	    CvVideoWriter *writer = cvCreateVideoWriter(path, CV_FOURCC('M','J','P','G'), //corresponding codec must be installed on machine
         fps,size, isColor);

	/*
	Codecs that are supported

	CV_FOURCC('P','I','M','1')    = MPEG-1 codec
    CV_FOURCC('M','J','P','G')    = motion-jpeg codec 
    CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
    CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
    CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
    CV_FOURCC('U', '2', '6', '3') = H263 codec
    CV_FOURCC('I', '2', '6', '3') = H263I codec
    CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec

	*/
	     
		
	//creates loop, which is stopped by setting my_on_off = false; see Stop_Video() method
	     while(my_on_off == true && frame_counter < to) { 
                    
			
		  // for demonstration purposes; frame capturing done by two function calls
		  // this can be done in one step by using cvQueryFrame (e.g.see Play_Movie) 
          cvGrabFrame(this->my_p_capture); // captures a frame
		   frame_to_write =cvRetrieveFrame(my_p_capture);  // retrieves the captured frame 
		   if( !frame_to_write) break;
		  
		   //EXTRA CODE FOR TESTING
		   //  cvCopy(frame_grabbed,frame_to_write, NULL);

		  // adds the frame to the file
         int x = cvWriteFrame(writer,frame_to_write); 
		  
		  frame_counter++;
		 
		   cvShowImage("write to avi", frame_to_write); 
          key=cvWaitKey(20);          
         }
      
		 // releasing the video writer plus cvCapture structure plus Image plus closing window

		  cvReleaseVideoWriter(&writer);
		  cvDestroyWindow( "write to avi" );
		  cvReleaseCapture(&my_p_capture);
		 // produces error and I don't know why 
       // cvReleaseImage(&frame_to_write);
		
}

void Video_OP::Unload_Video()
{
    // releases memory 
	   if(!my_p_capture) return ;
	    cvReleaseCapture( &my_p_capture ); 
		cvReleaseImage(&this->my_grabbed_frame);
	    
}


int Video_OP::Query_Frame()
{

   
	  // checks if file has been loaded successfully
	  if( !my_p_capture ) return  -1;   	
	     
            // gets a frame 
	        my_grabbed_frame = cvQueryFrame( my_p_capture);
			// check  if frame is available
	        if( !my_grabbed_frame ) return -1;
			//returns current frame index

			return this->Get_Current_Frame_Index();
	       
}

IplImage * Video_OP::Get_Pointer_of_grabbed_frame()
{

  return my_grabbed_frame;
}

CvCapture * Video_OP::Get_Pointer_to_Video()
{

  return my_p_capture;
}


void Video_OP::Depict_Contours()
{
  
		my_on_off = true;
	  int key =0;

	  CvMemStorage* g_storage = cvCreateMemStorage(0);
	  CvSeq* contours = 0;

	 
	  // checks if file has been loaded successfully
	  if( !my_p_capture ) return;    
	   
	    // retrieves frames per second (fps) to set speed of presentation
	    int fps = this->Get_Frame_Rate();
		
		// creates windows in which movie is displayed 
		cvNamedWindow( "movie", 0 ); //see below (A)
		cvNamedWindow("contours only");

		 my_grabbed_frame = cvQueryFrame( my_p_capture);
		IplImage* frame_gray = cvCreateImage(cvSize(my_grabbed_frame->width,my_grabbed_frame->height), 
              IPL_DEPTH_8U, 1);

		// s loop, which is stopped by setting my_on_off = false; 
	      while(my_on_off == true) { 
            // gets a frame 
	       my_grabbed_frame = cvQueryFrame( my_p_capture);
	       
	        // check  if frame is available
             if( !my_grabbed_frame ) break;
	  
            //converts frame into grey-scale frame
		     cvCvtColor( my_grabbed_frame, frame_gray, CV_RGB2GRAY );
	 
			
			int	g_thresh = 100;
	       
	         cvThreshold( frame_gray, frame_gray, g_thresh, 255, CV_THRESH_BINARY );
	         cvFindContours( frame_gray, g_storage, &contours );
	           cvZero( frame_gray );
	       if( contours ){
		    cvDrawContours(frame_gray,contours,cvScalarAll(255),cvScalarAll(255),100 );
		   }
			 
			cvShowImage("movie",my_grabbed_frame);
	        cvShowImage("contours only", frame_gray);
			
			key = cvWaitKey(1000 /fps);
		  }

		
	   cvReleaseCapture( &my_p_capture ); 
		cvReleaseImage(&frame_gray);
	    

	   cvDestroyWindow( "movie" );
	   cvDestroyWindow( "contours only" );

}


//################# COMMON FILE DIALOG CLASS METHODS ##########################
FileDialog::FileDialog(HWND parent_handle)
{
	my_hwnd = parent_handle;
	//char szFileName[MAX_PATH] ="";
	strcpy(szFileName,"");
	ZeroMemory(&my_ofn,sizeof(my_ofn));

	my_ofn.lStructSize = sizeof(my_ofn);
	my_ofn.hwndOwner = parent_handle;
	my_ofn.lpstrFilter = "All Files (*.*)\0*.*";
	my_ofn.lpstrFile = szFileName;
	my_ofn.nMaxFile = MAX_PATH;
	my_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//my_ofn.lpstrDefExt ="txt";
	//p_szFileName = szFileName;
	//strcpy(p_szFileName,szFileName);
	p_szFileName = szFileName;
}

bool FileDialog::OpenFile()
{
 bool tof = GetOpenFileName(&my_ofn);
 if(tof == false)
 {
   MessageBox(my_hwnd,"Didn't work out right","SORRY",MB_OK);
   return false;
 }
 return true;
}
bool FileDialog::SaveFile()
{
  
  if(!GetSaveFileName(&my_ofn))
  {
     MessageBox(my_hwnd,"Didn't work","SORRY",MB_OK);
	 return false;
  }
  return true;
}
char* FileDialog::GetName()
{
  return p_szFileName;
	// return szFileName;
}

void FileDialog::PrintPathName(int posX, int posY)
{
    HDC hDC = GetDC(my_hwnd);
	int length = strlen(this->GetName());
	TextOut(hDC,posX,posY,this->GetName(),length);
	ReleaseDC(my_hwnd,hDC);
	UpdateWindow(my_hwnd);
}
//END COMMON FILE DIALOG



