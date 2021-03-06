// Ni2SimpleViewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;
using namespace openni;
using namespace cv;

enum showOp {
    DEPTH = 1,
    IMAGE = 2,
    IR = 4
};

int getUserInput() {
    int option = 0;
    cout << "1) Depth only" << endl;
    cout << "2) Image only" << endl;
    cout << "3) IR only" << endl;
    cout << "4) Depth and Image" << endl;
    cout << "5) Depth and IR" << endl;
    cout << "6) Image and IR" << endl;
    cout << "7) All" << endl;
    cout << "0) Exit" << endl;
    cout << "Please input your choice : ";
    cin >> option;
    switch (option) {
    case 1:
        return DEPTH;
        break;
    case 2:
        return IMAGE;
        break;
    case 3:
        return IR;
        break;
    case 4:
        return (DEPTH + IMAGE);
        break;
    case 5:
        return (DEPTH + IR);
        break;
    case 6:
        return (IMAGE + IR);
        break;
    case 7:
        return (DEPTH + IMAGE + IR);
        break;
    case 0:
        return 0;
        break;
    default:
        return getUserInput();
        break;
    };
}

int _tmain(int argc, _TCHAR* argv[])
{
    int option = getUserInput();
    if ( 0 == option ) {
        cout << "Exit program!" << endl;
        return 0;
    }

    if ( STATUS_OK != OpenNI::initialize() ) {
        cout << "After initialization: " << OpenNI::getExtendedError() << endl;
        return 1;
    }

    Device devDevice;
    if ( STATUS_OK != devDevice.open( ANY_DEVICE) ) {
        cout << "Cannot open device: " << OpenNI::getExtendedError() << endl;
        return 1;
    }

    VideoMode mode;
    VideoStream vsDepth;
    VideoStream vsColor;
    VideoStream vsIR;

    if ( option & DEPTH ) {
        if ( STATUS_OK != vsDepth.create( devDevice, SENSOR_DEPTH ) ) {
            cout << "Cannot create depth stream on device: " << OpenNI::getExtendedError() << endl;
            return 1;
        } else {
            mode = vsDepth.getVideoMode();
            cout << "depth video mode - FPS=" << mode.getFps() << ", X=" << mode.getResolutionX() << ", Y=" << mode.getResolutionY() << endl;
            vsDepth.start();
        }
    }

    if ( option & IMAGE ) {
        if ( STATUS_OK != vsColor.create( devDevice, SENSOR_COLOR ) ) {
            cout << "Cannot create color stream on device: " << OpenNI::getExtendedError() << endl;
            return 1;
        } else {
            mode = vsColor.getVideoMode();
            cout << "color video mode - FPS=" << mode.getFps() << ", X=" << mode.getResolutionX() << ", Y=" << mode.getResolutionY() << endl;
            vsColor.start();
        }
    }

    if ( option & IR ) {
        if ( STATUS_OK != vsIR.create( devDevice, SENSOR_IR ) ) {
            cout << "Cannot create IR stream on device: " << OpenNI::getExtendedError() << endl;
            return 1;
        } else {
            mode = vsIR.getVideoMode();
            cout << "IR video mode - FPS=" << mode.getFps() << ", X=" << mode.getResolutionX() << ", Y=" << mode.getResolutionY() << endl;
            vsIR.start();
        }
    }

    VideoFrameRef depth_frame;
    VideoFrameRef color_frame;
    VideoFrameRef ir_frame;
    bool quit = false;
    bool capture = false;
    bool showText = true;
    bool mirror = false;
    vector<int> quality;
    quality.push_back(CV_IMWRITE_PNG_COMPRESSION);
    quality.push_back(0);
    while ( true ) {
        if ( (option & DEPTH) && vsDepth.isValid() ) {
            if ( STATUS_OK == vsDepth.readFrame( &depth_frame) ) {
                Mat imgDepth ( depth_frame.getHeight(), depth_frame.getWidth(), CV_16UC1, (void*)depth_frame.getData() );
                Mat img8bitDepth;
                imgDepth.convertTo( img8bitDepth, CV_8U, 255.0 / 4096 );
                if ( mirror ) {
                    flip(img8bitDepth, img8bitDepth, 1);
                }
                if ( showText ) {
                    putText(img8bitDepth, string("FrameID:") + to_string(depth_frame.getFrameIndex()), Point(5, 20), FONT_HERSHEY_DUPLEX, (depth_frame.getWidth()>320)?1.0:0.5, Scalar(255, 255, 255));
                }
                imshow( "Depth view", img8bitDepth );
                if ( capture ) {
                    imwrite( "depth_" + std::to_string(depth_frame.getFrameIndex()) + ".png", img8bitDepth, quality );
                }
            }
        }

        if ( ( option & IMAGE ) && vsColor.isValid() ) {
            if ( STATUS_OK == vsColor.readFrame( &color_frame) ) {
                Mat imgColor( color_frame.getHeight(), color_frame.getWidth(), CV_8UC3, (void*)color_frame.getData() );
                Mat imgBGRColor;
                cvtColor( imgColor, imgBGRColor, CV_RGB2BGR );
                if ( mirror ) {
                    flip(imgBGRColor, imgBGRColor, 1);
                }
                if ( showText ) {
                    putText(imgBGRColor, string("FrameID:") + to_string(color_frame.getFrameIndex()), Point(5, 20), FONT_HERSHEY_DUPLEX, (color_frame.getWidth()>320)?1.0:0.5, Scalar(200, 0, 0));
                }
                imshow( "Color view", imgBGRColor );
                if ( capture ) {
                    imwrite( "image_" + std::to_string(color_frame.getFrameIndex()) + ".png", imgBGRColor, quality );
                }
            }
        }

        if ( ( option & IR ) && vsIR.isValid() ) {
            if ( STATUS_OK == vsIR.readFrame( &ir_frame ) ) {
                Mat imgIR ( ir_frame.getHeight(), ir_frame.getWidth(), CV_16UC1, (void*)ir_frame.getData() );
                Mat img8bitIR;
                imgIR.convertTo( img8bitIR, CV_8U, 255.0 / 4096 );
                if ( mirror ) {
                    flip(img8bitIR, img8bitIR, 1);
                }
                if ( showText ) {
                    putText(img8bitIR, string("FrameID:") + to_string(ir_frame.getFrameIndex()), Point(5, 20), FONT_HERSHEY_DUPLEX, (ir_frame.getWidth()>320)?1.0:0.5, Scalar(200, 0, 0));
                }
                imshow( "IR view", img8bitIR );
                if ( capture ) {
                    imwrite( "ir_" + std::to_string(ir_frame.getFrameIndex()) + ".png", img8bitIR, quality );
                }
            }
        }
        int keyInput = waitKey( 1 );
        if ( keyInput != -1 ) {
            switch ( keyInput ) {
            case 'Q': // Q = 81
            case 'q': // q = 113
                //q for exit
                quit = true;
                break;
            case 'C': // C = 67
            case 'c': // c = 99
                // depth
                capture = true;
                break;
            case 'F': // F = 70
            case 'f': // f = 102
                showText = (showText)?false:true;
                break;
            case 'M': // M = 77
            case 'm': // m = 109
                mirror = (mirror)?false:true;
                break;
            default:
                break;
            }
        } else {
            capture = false;
        }
        if ( quit ) {
            break;
        }
    }
    if ( option & DEPTH ) vsDepth.destroy();
    if ( option & IMAGE ) vsColor.destroy();
    if ( option & IR ) vsIR.destroy();
 
    devDevice.close();
    OpenNI::shutdown();

    return 0;
}

