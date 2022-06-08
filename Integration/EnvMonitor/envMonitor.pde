//Processing sketch to store data passed from Arudino through serial port to an output csv file
//Displays a color coded and value-labelled monitor to the screen

String outFileName="C:/Users/xrayh/Desktop/Arduino_Outputs/testFile.csv";
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
             String tempCSStr = strings[1].split("=")[1].trim();
             float tempCS = Float.parseFloat(tempCSStr);
             String tempCRStr = strings[2].split("=")[1].trim();
             float tempCR = Float.parseFloat(tempCRStr);
             String tempBPStr = strings[3].split("=")[1].trim();
             float tempBP = Float.parseFloat(tempBPStr);
             String tempIonStr = strings[4].split("=")[1].trim();
             float tempIon = Float.parseFloat(tempIonStr);
             //println(temp1Str + " " + temp2Str + " " + temp3Str + " " + temp4Str);
             
             //Change color of rectangle based on temperature thresholds
             if (tempCS < 15.55) // 60 deg F
               fill(219, 3, 252); //Magenta 
	     else if (tempCS < 21.11) //70 deg F
	       fill(0, 256, 0); //Green
	     else if (tempCS < 23.89) //75F
	       fill(252, 223, 3); //Yellow
             else
               fill(256, 0, 0); //Red
             rect(0, 0, 120, 200);
             fill(0);
             text("CS Temp", 20, 20); //Coolant supply temp
             text(tempCSStr, 20, 100);
             text("[C]", 20, 180);

             if (tempCR < 15.55) //60F
	       fill(219, 3, 252); //Magenta
	     else if (tempCR < 26.67) //80F
               fill(0,256,0);
	      else if (tempCR < 37.78) //100F
	       fill(252, 223, 3); //Yellow
	     else
               fill(256, 0, 0);
             rect(120, 0, 240, 200);
             fill(0);
             text("CR Temp", 140, 20); //Coolant return temp
             text(tempCRStr, 140, 100);
             text("[C]", 140, 180);

	     if (tempBP < 15.55) //60F
	       fill(219, 3, 252); //Magenta
	     else if (tempBP < 26.67) //80F
               fill(0,256,0);
	      else if (tempBP < 37.78) //100F
	       fill(252, 223, 3); //Yellow
	     else
               fill(256, 0, 0);
             rect(240, 0, 360, 200);
             fill(0);
             text("BP Temp", 260, 20); //Base plate temp
             text(tempBPStr, 260, 100);
             text("[C]", 260, 180);
             
             if (tempIon < 15.55) //60F
	       fill(219, 3, 252); //Magenta
	     else if (tempIon < 26.67) //80F
               fill(0,256,0); //Green
	     else if (tempIon < 32.22) //90F
	        fill(252, 223, 3); //Yellow
             else
               fill(256, 0, 0); //Red
             rect(360, 0, 480, 200);
             fill(0);
             text("Ion Temp", 380, 20); //Ion chamber teemp
             text(tempIonStr, 380, 100);
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
             text("Baro Temp", 610, 20); //Barometer (Arduino box) temp
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
