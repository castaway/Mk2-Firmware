/*
 TiLDA Mk2

 Task

 The MIT License (MIT)

 Copyright (c) 2014 Electromagnetic Field LTD

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <FreeRTOS_ARM.h>
#include <debug.h>
#include "ColourFunApp.h"
#include "Tilda.h"
#include <glcd.h>
#include <M2tk.h>
#include "GUITask.h"
#include <fonts/allFonts.h>

// Use this as a boilerplate app class. If you create a new one, make sure to also register it in the AppManager
// otherwise it won't show up on the HomeScreen

App* ColourFunApp::New() {
    return new ColourFunApp;
}

ColourFunApp::ColourFunApp() {
    mButtonSubscription = Tilda::createButtonSubscription(LEFT | RIGHT | UP | DOWN); // Define the buttons you're interested in here
}

ColourFunApp::~ColourFunApp() {
    delete mButtonSubscription;
}

String ColourFunApp::getName() const {
        return "Colour Fun";
}

bool ColourFunApp::keepAlive() const {
        return false; // Should this app be suspended or killed when another app is started?
}

void ColourFunApp::task() {
        // Do some setup in here
        Tilda::getGUITask().clearRoot(); // Clean screen

        // GLCD.SetRotation(ROTATION_90);   // Orientation

        GLCD.SelectFont(System5x7);      // Font
        GLCD.CursorToXY(2, 2);           // Position cursor
        GLCD.print("Hello");             // Write text
        GLCD.CursorToXY(2, 12);
        GLCD.print("World");
        GLCD.CursorToXY(2, 34);
        GLCD.print("Try");
        GLCD.CursorToXY(2,44);
        GLCD.print("UP/DOWN");

        int direction = 1;
        unsigned int red, green, blue;
        int hue, saturation, value;
        red = green = blue = hue = 0;
        saturation = 255;
        value = 100; // 0-255
        Tilda::setLedColor({red, green, blue});   // LEDs off
        while(true) {  // Make sure this loop goes on forever
            Button button = mButtonSubscription->waitForPress(300); // Wait for up to a second for a button press
            // Make sure all the buttons you're interested in are defined in the constructor
            if (button == UP) {
              direction = 2;
            } else if (button == DOWN) {
              direction = -2;
            } else if (button == LEFT) {
              direction = -1;
            } else if (button == RIGHT) {
              direction = 1;
            }

            if(direction == 1) {
              hue = hue + 10;
              if(hue > 360)
                hue = 0;
            } else if(direction == -1) {
              hue = hue - 10;
              if(hue < 0) hue = 360;
            } else if(direction == 2) {
              value = value + 5;
              if(value > 100) value = 0;
            } else if(direction == -2) {
              value = value - 5;
              if(value < 0) value = 100;
            }
            HSBToRGB(hue, saturation, value, &red, &green, &blue);
            Tilda::setLedColor({red, green, blue});
        }
}

// This function taken from here:
// http://eduardofv.com/read_post/179-Arduino-RGB-LED-HSV-Color-Wheel-

void HSBToRGB(
    unsigned int inHue, unsigned int inSaturation, unsigned int inBrightness,
    unsigned int *oR, unsigned int *oG, unsigned int *oB )
{
    if (inSaturation == 0)
    {
        // achromatic (grey)
        *oR = *oG = *oB = inBrightness;
    }
    else
    {
        unsigned int scaledHue = (inHue * 6);
        unsigned int sector = scaledHue >> 8; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 8);	// position within the sector
        unsigned int p = (inBrightness * ( 255 - inSaturation )) >> 8;
        unsigned int q = (inBrightness * ( 255 - ((inSaturation * offsetInSector) >> 8) )) >> 8;
        unsigned int t = (inBrightness * ( 255 - ((inSaturation * ( 255 - offsetInSector )) >> 8) )) >> 8;

        switch( sector ) {
        case 0:
            *oR = inBrightness;
            *oG = t;
            *oB = p;
            break;
        case 1:
            *oR = q;
            *oG = inBrightness;
            *oB = p;
            break;
        case 2:
            *oR = p;
            *oG = inBrightness;
            *oB = t;
            break;
        case 3:
            *oR = p;
            *oG = q;
            *oB = inBrightness;
            break;
        case 4:
            *oR = t;
            *oG = p;
            *oB = inBrightness;
            break;
        default:    // case 5:
            *oR = inBrightness;
            *oG = p;
            *oB = q;
            break;
        }
    }
}

void ColourFunApp::afterSuspension() {} // If keepAlive() is true this is being called when the task is suspended
void ColourFunApp::beforeResume() {} // If keepAlive() is true this is being called when the task is resumed
