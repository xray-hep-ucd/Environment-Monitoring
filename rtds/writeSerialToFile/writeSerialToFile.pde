//Processing sketch to store data passed from Arudino through serial port to an output file

String outFileName="C:/Users/xrayh/Desktop/Arduino_Outputs/testFile.txt";

import processing.serial.*;
Serial myPort;
PrintWriter output;

void setup()
{
  size(480, 480);
  
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
     output.println(str);
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
