//Processing sketch to store data passed from Arudino through serial port to an output csv file
//Displays a color coded and value-labelled monitor to the screen

String outFileName="C:/Users/xrayh/Desktop/ArduinoOutputs/testFile.csv";
//boolean USE_DOOR = false; //Whether or not to expect a door sensor readout from the Arduino

import processing.serial.*;
Serial myPort;
PrintWriter output;

void setup()
{

  size(720, 250); //The size of the live monitor, change to (840, 250) if adding door sensor

  textSize(24);
 
  output = createWriter(outFileName);
  
  String portName = Serial.list()[0]; //COM3 should be at index 0
  
  println("PortName= " + portName);
  myPort = new Serial(this, portName, 115200); //Last param, the BAUD value must match that in Arduino sketch envSensors.ide
  
  println("Beginning run... select the status monitor and hit UP to end");
  fill(0);
  text("Click here and hit the UP arrow to end the run and write data to the file", 10, 225 );
  
  output.println("Time [hh:mm:ss],Temp_1 [C],Temp_2 [C],Temp_3 [C],Temp_4 [C],Pressure [hPa],Door,"); //Outout file column labels
}
 

void draw()
{
  while (myPort.available() > 0)
  {
    String str = myPort.readString();
    if (str != null)
    {
      if (str.startsWith("Run"))
       {
         //Extract values from file and update monitor
         String tempStr = tempMonitor(str);
         String presStr = pressureMonitor(str);
         /*String doorStr;
         if (USE_DOOR)
         {
           doorStr = doorMonitor(str);
         }
         else
         {
           doorStr = ",";
         }*/
         
        output.println(tempStr + presStr); //Write values to file in a CSV format
        //output.println(tempStr + presStr + doorStr); //Write values to file in a CSV format
       }
    }
   }
}


//Extracts temperatures from the Arduino ouput string and displays them to the canvas
//Colors can be set with arbitrary thresholds for each RTD
String tempMonitor(String str)
{
 if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 4)
         {
           try {
             String timeStr = strings[0].split("=")[1].trim();
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
             
             return timeStr + "," + temp1Str + "," + temp2Str + "," + temp3Str + "," + temp4Str + ",";
           }
           catch(Exception e)
           {
               println("WARNING: Format error in temps, skipping monitor update");
           }
         }
       } 
       return ",,,,,";
}


String pressureMonitor(String str)
{
  if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 6)
         {
           try 
           {
             String pressureStr = strings[5].split("=")[1].trim();
             String tempStr = strings[6].split("=")[1].trim(); //Room temp, not used for status monitor
             float pressure =  Float.parseFloat(pressureStr);
             float temp = Float.parseFloat(tempStr);
             
             //Change color of rectangle based on pressure
             if (pressure < 1100.0)
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(480, 0, 600, 200);
             fill(0);
             text("Pressure", 500, 20);
             text(pressureStr, 500, 100); 
             text("[hPa]", 500, 180);
             
             if ( temp < 30.0) //Specify arduino box temps here
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(600, 0, 720, 200);
             fill(0);
             text("Box Temp", 610, 20);
             text(tempStr, 620, 100);
             text("[C]", 620, 180);
             
             return pressureStr + ",";
           }
           catch(Exception e)
           {
               println("WARNING: Format error in pressure sensor, skipping monitor update");
               fill(0,0,256);
               rect(480, 0, 600, 200);
               fill(0);
               text("Pressure", 500, 20);       
               rect(600, 0, 720, 200);
               fill(0);
               text("Box Temp", 620, 20);
           }
         }
       } 
       
    return ",";
       
}



//Adds a rectangle to the canvas that is green if door status is CLOSED and red otherwise
String doorMonitor(String str)
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
             rect(720, 0, 840, 200);
             fill(0);
             text("Door", 740, 20);
             text(doorStatus, 740, 100);
             return doorStatus + ",";
           }
           catch(Exception e)
           {
               println("WARNING: Format error in door sensors, skipping monitor update");
               fill(0, 0, 256);
               rect(720, 0, 840, 200);
               fill(0);
               text("Door", 740, 20);
               
           }
         }
       } 
       return ",";
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
