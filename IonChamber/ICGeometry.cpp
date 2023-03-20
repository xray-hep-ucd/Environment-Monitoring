//Implementation of ICGeometry class. See header file for general documentation


#include "ICGeometry.h"

//C++ includes
#include <cassert>

//ROOT includes
#include "TCanvas.h"

//Default constructor. Uses 1 pt per mm via delegating constructor call
//Note this syntax is only valid in c++11 and later
ICGeometry::ICGeometry() : ICGeometry(1){}

//Standard constructor
ICGeometry::ICGeometry(int ptsPerMM)
{
    this->ptsPerMM = ptsPerMM;
    this->nX = this->outerWidth * ptsPerMM;
    this->nY = this->outerLength * ptsPerMM;
    this->nZ = this->outerHeight * ptsPerMM;
    
    int geometry[this->nX][this->nY][this->nZ];
    this->geometry = geometry;

    this->checkDimConsistency();
    this->initializeMaterials(ptsPerMM);
}

//Copy constructor
ICGeometry::ICGeometry(ICGeometry& that)
{
    this->ptsPerMM = that.ptsPerMM;
    this->nX = that.nX;
    this->nY = that.nY;
    this->nZ = that.nZ;
    //Not safe to just point this->geometry to that.geometry so copy values manually
    int geometry[this->.nX][this->nY][this->nZ];
    for (int x=0; x < this->nX; x++)
    {
        for (int y=0; y < this->nY; y++)
        {
            for (int z=0; z < this->nZ; z++)
            {
                geometry[x][y][z] = that.geometry[x][y][z];
            }
        }
    }
    this->geometry = geometry;
}

//Set of asserts checking that the z values of electrodes fall within the inner height of the IC and are ordered correctly
void ICGeometry::checkDimConsistency()
{
    assert(this->innerStartZ <= this->hv_zMin);
    assert(this->hv_zMin < this->hv_zMax);
    assert(this->hv_zMax <= this->innerDeltaZ);
    assert(this->innerStartZ <= this->grnd_1_zMin);
    assert(this->grnd1_zMin < this->grnd1_zMax);
    assert(this->grnd1_zMax < this->grnd2_zMin);
    assert(this->grnd2_zMin < this->grnd2_zMax);
    assert(this->grnd2_zMax < this->grnd3_zMin);
    assert(this->grnd3_zMin < this->grnd3_zMax);
    assert(this->grnd3_zMax <= this->innerDeltaZ + this->innerStartZ);
}

//Fill the passed pointers with the codes being used for each material
void ICGeometry::getMaterialCodes(int* code_air, int* code_nylon, int* code_grnd, int* code_hv)
{
    code_air = this->code_air;
    code_nylon = this->code_nylon;
    code_grnd = this->code_grnd;
    code_hv = this->code_hv;
}

//Initialize this->geometry with the appropriate material codes 
void ICGeometry::initializeMaterials()
{
    int air, nylon, grnd, hv; //Material codes
    this->getMaterialCodes(air, nylon, grnd, hv);

    for (int xStep=0; xStep < this->nX; xStep++)
    {
        int x = xStep / this->ptsPerMM;
        for (int yStep=0; yStep < this->nY; yStep++)
        {
            int y = yStep / this->ptsPerMM;
            for (int zStep=0; zStep < this->nZ; zStep++)
            {
                int z = zStep / this->ptsPerMM;

                int theMaterial;
                if (x < innerStartX || y < innerStartY || z < innerStartZ) //Lower x,y,z nylon
                    theMaterial = nylon;
                else if (x > innerStartX+innerDeltaX || y > innerStartY+innerDeltaY || z > innerStartZ + innerDeltaZ) //Higher x,y,z nigher
                    theMaterial = nylon;
                else if (z >= hv_zMin && z <= hv_zMax && y == innerStartY) //HV electrode 
                    theMaterial = hv;
                else if (y == (innerStartY + innerDeltaY) && ((z >= grnd1_zMin && z <= grnd1_zMax) || (z >= grnd2_zMin && z <= grnd2_zMax) || (z >= grnd3_zMin && z <= grnd3_zMax)))
                    theMaterial = grnd; //Ground/readout electrodes
                else
                    theMaterial = air;

                this->geometry[xStep][yStep][zStep] = theMaterial;
            }
        }
    }
}


/*Displays a 2D slice of the geometry specified by dim2 vs dim1 at dim3=atDim3
    TString dim1 : the dimension to be plotted on the x-axis of the plot. Must be one of{"x", "X", "y", "Y", "z", "Z"} 
    TString dim2 : the dimension to be plotted on the y-axis of the plot. Must be one of{"x", "X", "y", "Y", "z", "Z"} and different from dim1
    int atDim3   : the value of the third dimension (not dim1 or dim2) where the slice should be evaluated
    TString drawOption : An optional argument for TH2F->Draw() plotting of the slice. Default option is COLZ
*/
void ICGeometry::displayGeom(TString dim1, TString dim2, int atDim3, TString drawOption = "COLZ")
{
    //Graphics settings
    gStyle->SetOptStat(0);
    TCanvas* canv = new TCanvas("geomCanv", "IC Geometry Viewer", 800, 800);

    dim1 = dim1.ToUpper():
    dim2 = dim2.ToUpper():
    
    TH2F* hist;
    int xDimN, yDimN, zDimN;
    if (dim1 == "X" && dim2 == "Y") 
    {
        hist = TH2F("icGeom", "IC Geometry Slice at z=" + atDim3, this->nX, 0, this->outerDeltaX, this->nY, 0, this->outerDeltaY);
        xDimN = 1;
        yDimN = 2;
        zDimN = 3;
    }
    else if (dim1 == "Y" && dim2 == "X")
    {
        hist = TH2F("icGeom", "IC Geometry Slice at z=" + atDim3, this->nY, 0, this->outerDeltaY, this->nX, 0, this->outerDeltaX);
        xDimN = 2;
        yDimN = 1;
        zDimN = 3;
    }
    else if(dim1 == "X" && dim2 == "Z")
    {
        hist = TH2F("icGeom", "IC Geometry Slice at y=" + atDim3, this->nX, 0, this->outerDeltaX, this->nZ, 0, this->outerDeltaZ);
        xDimN = 1;
        yDimN = 3;
        zDimN = 2;
    }
    else if (dim1 == "Z" && dim2 == "X")
    {
        hist = TH2F("icGeom", "IC Geometry Slice at y=" + atDim3, this->nZ, 0, this->outerDeltaZ, this->X, 0, this->outerDeltaX);
        xDimN = 2;
        yDimN = 3;
        zDimN = 1;
    }
    else if (dim1 == "Y" && dim2 == "Z")
    {
        hist = TH2F("icGeom", "IC Geometry Slice at x=" + atDim3, this->nY, 0, this->outerDeltaY, this->nZ, 0, this->outerDeltaZ);
        xDimN = 3;
        yDimN = 1;
        zDimN = 2;
    }
    else if (dim1 == "Z" && dim2 == "Y")
    {
        hist = TH2F("icGeom", "IC Geometry Slice at x=" + atDim3, this->nZ, 0, this->outerDeltaZ, this->nY, 0, this->outerDeltaY);
        xDimN = 3;
        yDimN = 2;
        zDimN = 1;
    }
    else
    {
        cout << "Invalid dimensions, must be X, Y, or Z"
        throw std:invalid_argument; 
    }
    

    //Fill based on the chosen dimension
    if (zDimN == 3) //XY or YX
    {
        int zStep = atDim3 * this->ptsPerMM;
        for (int xStep = 0; xStep < this->nX; xStep++)
        {
            for (int yStep = 0; yStep < this->nY; yStep++)
            {
                if (xDimN == 1)
                    hist->SetBinContent(xStep, yStep, this->geometry[xStep][yStep][zStep]);
                else
                    hist->SetBinContent(yStep, xStep, this->geometry[xStep][yStep][zStep]);
            }
        }
    }
    else if (yDimN == 3) //XZ or ZX
    {
        int yStep = atDim3 * this->ptsPerMM;
        for (int xStep = 0; xStep < this->nX; xStep++)
        {
            for (int zStep = 0; zStep < this->nZ; zStep++)
            {
                if (xDimN == 1)
                    hist->SetBinContent(xStep, zStep, this->geometry[xStep][yStep][zStep]);
                else
                    hist->SetBinContent(zStep, xStep, this->geometry[xStep][yStep][zStep]);
            }
        }
    }
    else //YZ OR ZY
    {
        int xStep = atDim3 * this->ptsPerMM;
        for (int yStep = 0; yStep < this->nY; yStep++)
        {
            for (int zStep = 0; zStep < this->nZ; zStep++)
            {
                if (yDimN == 1)
                    hist->SetBinContent(yStep, zStep, this->geometry[xStep][yStep][zStep]);
                else
                    hist->SetBinContent(zStep, yStep, this->geometry[xStep][yStep][zStep]);
            }
        }
    }
    
    canv->cd();
    hist->Draw(drawOption);

}