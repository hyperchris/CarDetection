# CarDetection
Version: 1.0

##Requirements
**Smartphone:** Android 4.0 or above  
**Developing Tool:**  
- Eclipse with ADT, Android NDK, and OpenCV For Android installed
- A computer with OpenCV installed. Linux OS is recommended.

##Function  
The app can read in a given image and return the positions of recognized cars. 

##Notes
**How the code works:**  
After configuring everything on the OS and Eclipse, I created a folder called "jni" in the project's root dir. There I put all cpp files. For the cpp code, it must include <jni.h>. After that, I added "Android.mk" and "Application.mk" to jni dir, which are used for instructing Eclipse to compile the cpp files. Then I converted the project to a C/C++ project, added an environment of NDKROOT to Eclipse, and added a path in GNC C++ compiler to reach OpenCV for Android. Then code can run after all these steps. If you are interested in the computer vision part, please refer to [this tutorial](http://abhishek4273.com/2014/03/16/traincascade-and-car-detection-using-opencv/) to learn how the car detector works on your desktop/laptop.