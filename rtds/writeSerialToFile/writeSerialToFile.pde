//Processing sketch to store data passed from Arudino through serial port to an output file

String outFileName="C:/Users/xrayh/Desktop/Arduino_Outputs/testFile.txt";


import processing.serial.*;
Serial myPort;
PrintWriter output;

void setup()
{
  size(480, 200);
  textSize(24);
  
  output = createWriter(outFileName);
  
  String portName = Serial.list()[0];
  
  println("PortName= " + portName);
  myPort = new Serial(this, portName, 115200); //Last param, the BAUD value must match that in Arduino sketch
  
  println("Beginning run... select the plot and hit UP to end");
}


void draw()
{
  while (myPort.available() > 0)
  {
    String str = myPort.readString();
    if (str != null)
    {
       output.println(str); //Write arduino output to file
     
       //Extract temperatures from file
       if (str.startsWith("Run"))
       {
         String strings[] = str.split("\n");
         if (strings.length > 4)
         {
           try {
             String temp1Str = strings[1].split("=")[1];
             float temp1 = Float.parseFloat(temp1Str);
             String temp2Str = strings[2].split("=")[1];
             float temp2 = Float.parseFloat(temp2Str);
             String temp3Str = strings[3].split("=")[1];
             float temp3 = Float.parseFloat(temp3Str);
             String temp4Str = strings[4].split("=")[1];
             float temp4 = Float.parseFloat(temp4Str);
             println(temp1Str + " " + temp2Str + " " + temp3Str + " " + temp4Str);
             
             //Change color of rectangle based on temperature thresholds
             if (temp1 < 22.6)
               fill(0, 256, 0);
             else
               fill(256, 0, 0);
             rect(0, 0, 120, 200);
             fill(0);
             text("Temp 1", 20, 20);
             text(temp1Str, 20, 100);
             
             if (temp2 < 30)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(120, 0, 240, 200);
             fill(0);
             text("Temp 2", 140, 20);
             text(temp2Str, 140, 100);
             
             if (temp3 < 22.6)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(240, 0, 360, 200);
             fill(0);
             text("Temp 3", 260, 20);
             text(temp3Str, 260, 100);
             
             if (temp4 < 22.6)
               fill(0,256,0);
             else
               fill(256, 0, 0);
             rect(360, 0, 480, 200);
             fill(0);
             text("Temp 4", 380, 20);
             text(temp4Str, 380, 100);
             
  
           }
           catch(Exception e)
           {
             continue;
           }
         }
       }
    }
   }
}

void keyPressed()
{
  if (key == CODED)
  {
   if (keyCode == UP)
   {
     output.flush();
     output.close();
     println("Run has ended");
     println("Output file has been saved to " + outFileName);
   }
  }
  
}