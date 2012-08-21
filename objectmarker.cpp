/**
This file is used to mark objects for training images.
Put positive samples in folder 'positive' all the images should be in .bmp format and file name should be like '1.bmp', '2.bmp' etc.
After running this program each images will pop up. Select region of interest using mouse (creating rectagle while left click pressed) 
use 'space' to add this data to file and 'enter' to save this data and load next file. Press 'escape' exit.
'info.txt' file will be generated which can be used to create samples for training.
*/

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#define ENTER   10
#define ESC     27
#define SPACE   32
#define CTRL_C  262243


using namespace std;

IplImage* image=0;
IplImage* image2=0;
int start_roi=0;
int roi_x0=0;
int roi_y0=0;
int roi_x1=0;
int roi_y1=0;
int numOfRec=0;

CvRect selection;
CvPoint origin;

String WINDOW_NAME="<SPACE>add <ENTER>save and load next <ESC>exit";



void on_mouse( int event, int x, int y, int flags, void* param ){  
  fflush(stdout);
  
  if(event == CV_EVENT_LBUTTONDOWN){
         start_roi=1;         
         selection.x = selection.y = selection.width = selection.height = 0;
	 roi_x0=x;
	 roi_y0=y;
  }
  if(event == CV_EVENT_LBUTTONUP){
         start_roi = 0;
	 roi_x1=x;
	 roi_y1=y;	
	 image2=cvCloneImage(image);
	 cvRectangle(image2, cvPoint(roi_x0,roi_y0), cvPoint(roi_x1,roi_y1), CV_RGB(255,0,255), 1);        
         selection.x = roi_x0;
         selection.y = roi_y0;
         selection.width = abs(roi_x1-roi_x0);
         selection.height = abs(roi_y1-roi_y0);         
	 cvShowImage("window",image2);
	 cvReleaseImage(&image2);
  }
}

int main(int argc, char** argv){
  char filename[255];
  int iKey=0; 
  cvAddSearchPath("");    
  cvNamedWindow("window",CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback("window",on_mouse,0);  
  char strPrefix[8000]="";
  char strPostfix[8000]="";  
  int start;  
  ofstream output("info.txt");  
  if (argc==2)
  {
      sscanf(argv[1],"%d",&start);
  }
  else
  {
      start = 1;
  }

  printf("Starting from %d\n",start);
  
  for (int i = start; image = 0, sprintf(filename, "positive/%d.bmp",i), image = cvLoadImage(filename,1), image != 0; i++) 
  {
    printf(" Picture: %s \n",filename);
    strcpy(strPrefix, filename);			
    numOfRec=0;
    do
    {
        cvShowImage("window",image);	
        iKey=cvWaitKey(0);
	switch(iKey)
	{
            case ENTER:
                break;
            
            case SPACE: 
                numOfRec++;
                if(roi_x0<roi_x1 && roi_y0<roi_y1)
                {
                    printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n", numOfRec,roi_x0,roi_y0,roi_x1-roi_x0,roi_y1-roi_y0);	
                    char app[100];
                    sprintf(app," %d %d %d %d", roi_x0, roi_y0, roi_x1-roi_x0, roi_y1-roi_y0);
                    strcat(strPostfix, app);
                }
                if(roi_x0>roi_x1 && roi_y0>roi_y1)
                {
                    printf("   %d. rect x=%d\ty=%d\twidth=%d\theight=%d\n", numOfRec,roi_x1,roi_y1,roi_x0-roi_x1,roi_y0-roi_y1);
                    
                    char app[100];
                    sprintf(app," %d %d %d %d", roi_x0, roi_y0, roi_x1-roi_x0, roi_y1-roi_y0);
                    strcat(strPostfix, app);
                }
                break;
                
            case CTRL_C:    
            case ESC:  
                cvReleaseImage(&image);
                cvDestroyWindow("window");
//              output.close();
                break;
        }
    }
    while(iKey!=ESC && iKey!=ENTER && iKey!=CTRL_C); 
    if(numOfRec>0 && (iKey==ENTER))    
    {
        output << strPrefix << " "<< numOfRec << strPostfix <<"\n";
        printf("line appended \n %s %d %s\n", strPrefix, numOfRec, strPostfix);
        memset(strPostfix,0, 8000);
    }
    cvReleaseImage(&image);
  }
  
  output.close();
  cvDestroyWindow("window");
  return 0;
}
