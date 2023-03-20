//Class definition containing the geometry and material information of the ion chamber

/*
    Dimensions of the ion chamber are defined below under the private scope
    Note that here, (0,0,0) is defined as the outer corner of the top dielectric piece (closest to tube and away fromthe door)
    x is along the electrodes and y is parallel to the door.
    All dimensions are in mm.
    The primary deliverable of the class is the geometry 3D int array. At each index, an int corresponding
    to the material at that step is stored. Material codes are hardcoded below and can be retrieved via the the getMaterialCodes function.
    The number of points (array indices) in each dimension is calculated as the (dimension length) x nPtsPerMM provided to the constructor 
    and stored as nX, nY, nZ. Integer division is used in expressions like z_in_mm = step_in_z / ptsPerMM which dictates exact boundaries.
    Electrodes are currently assumed to be 1mm thick and the entire inner x length. Fields in electrodes are treated as constant, i.e.
    the HV electrode has voltage V =/= 0 at every step within it, not just at the surfaces.
*/


#ifndef ICGEOMETRY
#define ICGEOMETRY
#endif

#include "TString.h"

/*A class representing the material distribution of the ion chamber*/
class ICGeometry
{
    public:
    int*** geometry; //3D array storing material info.
    int nX;  //Length of first axis of geometry = int(nPtsPerMM * outerWidth)
    int nY;  //Lengh of second axis of geometry = int(nPtsPerMM * outerLength)
    int nZ;  //Lengh of third axis of geometry = int(nPtsPerMM * outerHeight)

    //Constructors and destructor
    ICGeometry(); //Default constructor will have 1 pt per mm
    ICGeometry(ICGeometry& that); 
    ICGeometry(int ptsPerMM); //Primary constuctor. ptsPerMM specifices number of steps in each dimension
    ~ICGeometry();
    
    void checkDimConsistency(); //Set of asserts to ensure electrode positions fall within innerHeight and ordering OK

    //Fill the passed pointers with the material codes used in geometry
    void getMaterialCodes(int* code_air, int* code_nylon, int* code_grnd, int* code_hv);
    //Displays the geometry on a ROOT TCanvas. Specify which dimensions to plot via booleans and a style via drawOption
    void displayGeom(bool x, bool y, bool z, TString drawOption); 


    private:

    //Dimensions of chamber
    //These are the deltas, i.e. top-bottom of the dielectric=outerHeight
    //Dielectric box dimensions are "outer", air inside chamber "inner"
    int outerDeltaZ = 40; //z
	int innerDeltaZ = 30; 
    int innerStartZ = 5;
	int outerDeltaX = 30; //x
	int innerDeltaX = 20;
    int innerStartX = 5;
	int outerDeltaY = 30; //y
	int innerDeltaY = 20;
    int innerStartY = 5;

	//Plate dimensions, z=0 defined as top of the top dielectic plate
	int hv_zMin = 5;
	int hv_zMax = 25;

    //Ground Plate 1 at top of IC (lowest z)
	int grnd1_zMin = 5;
	int grnd1_zMax = 10;

	int grnd2_zMin = 12;
	int grnd2_zMax = 17;

	int grnd3_zMin = 19;
	int grnd3_zMax = 24;

    //The values which will fill geometry corresponding to each material in the IC
    int code_grnd = 0
    int code_air = 1;
	int code_nylon = 2;
	int code_hv = 3;

    int ptsPerMM; // Number of points per mm in

    void initializeMaterials(); // Fill geometry with material codes

};