package com.testopencv.haveimgfun;  
  
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import android.annotation.SuppressLint;
import android.app.Activity;  
import android.graphics.Bitmap;  
import android.graphics.Bitmap.Config;  
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;  
import android.os.Bundle;  
import android.os.Environment;
import android.widget.Button;  
import android.widget.Toast;
import android.util.Log;
import android.view.View;  
import android.widget.ImageView;  
  
public class HaveImgFun extends Activity {  
    /** Called when the activity is first created. */ 
	private static final String FOLDER_NAME = "/carDetection/";			// folder that stores all external files
	private static final String [] FILE_LIST = {"checkcas.xml","cas1.xml","cas2.xml","cas3.xml","cas4.xml"};		// file names
	private static final String RES_FILE = "resImg.jpg";		// name of result image (also stored in the folder)
	
	// UI widgets
    private ImageView imgView; 
    private Button btnNDK, btnRestore;  
  
    private String folderPath = "";		// path of the folder in the phone
    
    @Override  
    public void onCreate(Bundle savedInstanceState) {  
        super.onCreate(savedInstanceState);  
        setContentView(R.layout.activity_have_img_fun);  
  
        this.setTitle("Vehicle Detection");  						// init UI widgets
        btnRestore = (Button) this.findViewById(R.id.btnRestore);  
        btnRestore.setOnClickListener(new ClickEvent());  
        btnNDK = (Button) this.findViewById(R.id.btnNDK);  
        btnNDK.setOnClickListener(new ClickEvent());  
        imgView = (ImageView) this.findViewById(R.id.ImageView01);  
        
        folderPath = Environment.getExternalStorageDirectory().toString() + FOLDER_NAME;		// create the folder
        File file = new File(folderPath);
		if (!file.exists()) {
			file.mkdir();
			Log.i("on create","file folder created");
		}
        for(int i=0; i<FILE_LIST.length; i++){
            copyFile(FILE_LIST[i],i,folderPath+FILE_LIST[i]);		// copy needed files to the folder
        }
        
        Bitmap img = ((BitmapDrawable) getResources().getDrawable(  	// show the original image
                R.drawable.photo)).getBitmap();  
        imgView.setImageBitmap(img);  
    }  
  
    /* behavior when user clicks the buttons */
    class ClickEvent implements View.OnClickListener {  	
        public void onClick(View v) { 
        	// click the "process" button
            if (v == btnNDK) {  
                long current = System.currentTimeMillis();  	// setup timer
                
                Bitmap img1 = ((BitmapDrawable) getResources().getDrawable(  		// transform the image to int array
                        R.drawable.photo)).getBitmap();  
                int w = img1.getWidth(), h = img1.getHeight();  
                int[] pix = new int[w * h];  
                img1.getPixels(pix, 0, w, 0, 0, w, h); 
                
                int result = LibImgFun.ImgFun(pix, w, h);  		// native function (input the int array of original image, it outputs the number of recognized cars in the image)
                
                Toast.makeText(getApplicationContext(), "There are "+result+" cars in the image",Toast.LENGTH_SHORT).show();	// show the result as Toast
                
                imgView.setImageBitmap(BitmapFactory.decodeFile(folderPath+RES_FILE));                // show the result image 
                
                long performance = System.currentTimeMillis() - current; 				// show related information (image size, and process time) in the title field 
                HaveImgFun.this.setTitle("width:" + String.valueOf(img1.getWidth())  
                        + "  hight:" + String.valueOf(img1.getHeight()) + "  time:"  
                        + String.valueOf(performance) + "ms");  
            } 
            // click the "restore" button
            else if (v == btnRestore) {  							
                Bitmap img2 = ((BitmapDrawable) getResources().getDrawable(  
                        R.drawable.photo)).getBitmap();  
                imgView.setImageBitmap(img2);  
                HaveImgFun.this.setTitle("Vehicle Detection");  
            }  
        }  
    }
    
    // copy xml file (in assets folder) to external storage (So the JNI native code will be able to read these files)
    @SuppressLint("ShowToast")
	public void copyFile(String oldPath, int num, String newPath) {   
        try {   
            int bytesum = 0;   
            int byteread = 0;   
            File oldfile = new File(oldPath);     
            InputStream inStream = getResources().getAssets().open(oldPath); 		// input xml in assests folder
            FileOutputStream fs = new FileOutputStream(newPath);   
            byte[] buffer = new byte[1444];   
            int length;   
            while ( (byteread = inStream.read(buffer)) != -1) {   
                bytesum += byteread; 				// file size  
                //System.out.println(bytesum);   
                fs.write(buffer, 0, byteread);   
            }   
            inStream.close();   
            Log.i("copyFIle","Classifier " + num + " created in: " + newPath);
        }   
        catch (Exception e) {   
            Log.i("copyFIle","Classifier " + num + " failed to be created to: " + newPath);  
            e.printStackTrace(); 
        }   
   
    }
} 