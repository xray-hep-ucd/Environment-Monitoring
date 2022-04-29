//Processing sketch to store data passed from Arudino through serial port to an output file

String outFileName="C:/Users/xrayh/Desktop/Arduino_Outputs/testFile.txt";


import processing.serial.*;
Serial myPort;
PrintWriter output;

void setup()
{
  size(720, 250);
  textSize(24);
  
 
  
  output = createWriter(outFileName);
  
  String portName = Serial.list()[0]; //COM3 should be at index 0
  
  println("PortName= " + portName);
  myPort = new Serial(this, portName, 115200); //Last param, the BAUD value must match that in Arduino sketch envSensors.ide
  
  println("Beginning run... select the status monitor and hit UP to end");
  fill(0);
  text("Click here and hit the UP arrow to end the run and write data to the file", 10, 225 );
}


void draw()
{
  while (myPort.available() > 0)
  {
    String str = myPort.readString();
    if (str != null)
    {
       output.println(str); //Write arduino output to file
       //Extract values from file and update monitor
       tempMonitor(str);
       pressureMonitor(str);
       doorMonitor(str);
       
    }
   }
}

//Extracts temperatures from the Arduino ouput string and displays them to the canvas
//Colors can be set with arbitrary thresholds for each RTD
void tempMonitor(String str)
{
 if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 4)
         {
           try {
             String temp1Str = strings[1].split("=")[1].trim();
             float temp1 = Float.parseFloat(temp1Str);
             String temp2Str = strings[2].split("=")[1].trim();
             float temp2 = Float.parseFloat(temp2Str);
             String temp3Str = strings[3].split("=")[1].trim();
             float temp3 = Float.parseFloat(temp3Str);
             String temp4Str = strings[4].split("=")[1].trim();
             float temp4 = Float.parseFloat(temp4Str);
             //println(temp1Str + " " + temp2Str + " " + temp3Str + " " + temp4Str);
             
             //Change color of rectangle based on temperature thresholds
             if (temp1 < 22.6)
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(0, 0, 120, 200);
             fill(0);
             text("Temp 1", 20, 20);
             text(temp1Str, 20, 100);
             text("[C]", 20, 180);
             
             if (temp2 < 30)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(120, 0, 240, 200);
             fill(0);
             text("Temp 2", 140, 20);
             text(temp2Str, 140, 100);
             text("[C]", 140, 180);
             
             if (temp3 < 22.6)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(240, 0, 360, 200);
             fill(0);
             text("Temp 3", 260, 20);
             text(temp3Str, 260, 100);
             text("[C]", 260, 180);
             
             if (temp4 < 22.6)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(360, 0, 480, 200);
             fill(0);
             text("Temp 4", 380, 20);
             text(temp4Str, 380, 100);
             text("[C]", 380, 180);
             
  
           }
           catch(Exception e)
           {
               println("WARNING: Format error in temps, skipping monitor update");
           }
         }
       } 
}


void pressureMonitor(String str)
{
  if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 6)
         {
           try 
           {
             String pressureStr = strings[5].split("=")[1].trim();
             //String tempStr = strings[6].split("=")[1].trim(); //Room temp, not used for status monitor
             float pressure =  Float.parseFloat(pressureStr);
             
             //Change color of rectangle based on door closure status
             if (pressure > 30.0)
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(480, 0, 600, 200);
             fill(0);
             text("Pressure", 500, 20);
             text(pressureStr, 500, 100); 
             text("[hPa]", 500, 180);
           }
           catch(Exception e)
           {
               println("WARNING: Format error in pressure sensor, skipping monitor update");
               fill(0,0,256);
               rect(480, 0, 600, 200);
               fill(0);
               text("Pressure", 500, 20);
               
           }
         }
       } 
}



//Adds a rectangle to the canvas that is green if door status is CLOSED and red otherwise
void doorMonitor(String str)
{
    if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 7)
         {
           try 
           {
             String doorStatus = strings[7].split("=")[1].trim();
             
             //Change color of rectangle based on door closure status
             if (doorStatus.equals("CLOSED"))
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(600, 0, 720, 200);
             fill(0);
             text("Door", 620, 20);
             text(doorStatus, 620, 100);        
           }
           catch(Exception e)
           {
               println("WARNING: Format error in door sensors, skipping monitor update");
               fill(0, 0, 256);
               rect(600, 0, 720, 200);
               fill(0);
               text("Door", 620, 20);
               
           }
         }
       } 
}

//Pressing the UP arrow while the canvas is selected writes data to the file
void keyPressed()
{
  if (key == CODED)
  {
   if (keyCode == UP)
   {
     output.flush();
     output.close();
     println("Ending run");
     println("Output file has been saved to " + outFileName);
     exit();
   }
  }
  
}
