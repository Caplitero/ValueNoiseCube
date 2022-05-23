#pragma once
#include <iostream>

#define Pi 3.1415927f
class ValueNoise {



public:
	float* NoiseArray = nullptr;  // Data output array
	float  _noiseCancelling = 0.2f; // How much the scale will be reduces for the next octave
	int    Dimension = 1;    // The view Dimension 1D/2D  
	int    _octaves = 3;    // The number of divisions ( the more, the better)
	bool   _use_CosFunc = 0;    // Conver the function from linear to cos for more accuracy


private:
	float* SeedArray = nullptr;
	int    _sizeX = 0;   // Size of x axis array
	int    _sizeY = 0;   // Size of y axis array 
	int    _sizeZ = 0;   // Size of z axis array


	float convertToCos(float T)
	{
		return (1 - std::cos(T * Pi)) * 0.5f;
	}
	float LinearInterpolation(float v0, float v1, float w)
	{
		return v0 * (1.0f - w) + v1 * w;
	}
	float convertToCubic(float v0, float v1, float w)
	{
		return (v1 - v0) * (3.0f - w * 2.0f) * w * w + v0;
	}
	float convertToBiCubic(float v0, float v1, float w)
	{
		return (v1 - v0) * ((w * (w * 6.0f - 15.0f) + 10.0f) * w * w * w) + v0;
	}



	/* Linear Interpolation formula 1D:
	  y = y0*[1- (x-x0)/(x1-x0)] + y1*(x-x0)/(x1-x0)

   Where :
	  int x, float y are the current coordinates
	  int    Octaves represents how many times the noises will be corrected
	  int    NoiseReduction represents how fast the Pitch will de diminished
	  float  Scale  = the scale factor applied to the distance between y0 and y1
	  float  Noise  = last position of y
	  float* Output = where all the Noises are saved
	  float  Size   = lenght of array        (how many x's are used / usually the width of the screen)
	  int    Pitch  = size / (NoiseReduction*i)    (this will get how long is the jump between x's at the i'th octave)
	  int    x0     = (x  / Pitch ) * Pitch  (find the x where the line starts)
	  int    x1     = (x0 + Pitch ) % Size   [jump to the next x and see if it is found in the array
											 (if not , it will return to the first x in the array)]
	  float  y0     = NoiseSeed[x0]
	  float  y1     = NoiseSeed[x1]

	  x1-x0 as known in mathematics is the distance between two points on the ox Axis and equal to Pitch
	  so we can write (x-x0)/(x1-x0) as (x-x0)/Pitch and let's equal this to t
	  t = (x-x0)/Pitch

	In Conclusion , the formula can be simplified :
	  y = y0(1-t)+ y1*t ;



 */

	void Noise1D() {

		for (int x = 0; x < _sizeX; x++)
		{
			float Noise = 0.0f;
			float Scale = 2.0f;
			float fScaleAcc = 0.0f;

			for (int o = 0; o < _octaves; o++)
			{
				int   Pitch = _sizeX >> o;
				int   x0 = (x / Pitch) * Pitch;
				int   x1 = (x0 + Pitch) % _sizeX;
				float t = (float)(x - x0) / (float)Pitch;
				if (_use_CosFunc)t = convertToCos(t);

				float y0 = SeedArray[x0];
				float y1 = SeedArray[x1];

				float y = convertToBiCubic(y0, y1, t);


				Noise += y * Scale;

				fScaleAcc += Scale;
				Scale *= _noiseCancelling;
			}

			NoiseArray[x] = Noise / fScaleAcc;
		}

	}


	/*
 BiLinear Interpolation formula 2D:
	 z = (y1-y)/(y1-y0)*f(x,y1) + (y-y0)/(y1-y0)*f(x,y1)

 Where :
	 int x,y, float z are the current coordinates
	  int    Octaves represents how many times the noises will be corrected
	  int    NoiseReduction represents how fast the Pitch will de diminished
	  float  Scale  = the scale factor applied to the distance between (x0,y0) and (x1,y1)
	  float  Noise  = last position of (x,y)
	  float* Output = where all the Noises are saved
	  float  Size   = lenght of array
	  int    Pitch  = size / (NoiseReduction*i)

	  int    x0     = (x  / Pitch ) * Pitch  (find the x where the line starts)
	  int    x1     = (x0 + Pitch ) % Size   [jump to the next x and see if it is found in the array
											 (if not , it will return to the first x in the array)]
	  int    y0     = (y  / Pitch ) * Pitch  (find the y where the line starts)
	  int    y1     = (y0 + Pitch ) % Size   [jump to the next y and see if it is found in the array
											 (if not , it will return to the first y in the array)]

	  Just like in the 1D transformation T = (a-a0)/Pitch
	  thus TransfomX = TX = (x-x0)/Pitch and TransformY = TY = (y-y0)/Pitch

	  f(x,y0)=(x1-x)/(x1-x0)f(x0,y0) + (x-x0)/(x1-x0)f(x1,y0);
	  f(x,y1)=(x1-x)/(x1-x0)f(x0,y1) + (x-x0)/(x1-x0)f(x1,y1);
	  But (x1-x)/(x1-x0)=(x0-x0+x1-x)/(x1-x0)
						=(x1-x0)/(x1-x0)+(x0-x)/(x1-x0)
						=1+(-(x-x0)/(x1-x0))=1-TX
	  So f(x,y0)= Z1 = (1-TX)*f(x0,y0) + TX*f(x1,y0)
	 and f(x,y1)= Z2 = (1-TX)*f(x0,y1) + TX*f(x1,y1)



 In Conclusion , the formula can be simplified :
	 z = (y1-y)/(y1-y0)*X + TY*Y
	   = (1-TY)*X + TY*Y
	   = X- X*TY + TY*Y
	   = TY*(Y-X)+X


*/





	bool Noise2D()
	{

		for (int x = 0; x < _sizeX; x++)
			for (int y = 0; y < _sizeY; y++)
			{

				float Noise = 0.0f;
				float Scale = 0.25f;
				float fScaleAcc = 0.0f;

				for (int o = 0; o < _octaves; o++)
				{

					int   PitchX = _sizeX >> o;
					int   PitchY = _sizeY >> o;
					if (PitchX == 0 || PitchY == 0) return 0;
					int   x0 = (x / PitchX) * PitchX;
					int   y0 = (y / PitchY) * PitchY;

					int   x1 = (x0 + PitchX) % _sizeX;
					int   y1 = (y0 + PitchY) % _sizeY;


					float TX = (float)(x - x0) / (float)PitchX;
					float TY = (float)(y - y0) / (float)PitchY;

					if (_use_CosFunc) {
						TX = convertToCos(TX);
						TY = convertToCos(TY);
					}
					float z1 = SeedArray[y0 * _sizeX + x0];
					float z2 = SeedArray[y0 * _sizeX + x1];
					float z3 = SeedArray[y1 * _sizeX + x0];
					float z4 = SeedArray[y1 * _sizeX + x1];


					float X = convertToCubic(z1, z2, TX);
					float Y = convertToCubic(z3, z4, TX);

					Noise += (TY * (Y - X) + X) * Scale;


					fScaleAcc += Scale;
					Scale *= _noiseCancelling;
				}

				NoiseArray[y * _sizeX + x] = Noise / fScaleAcc;
			}

		return 1;
	}

	bool Noise3D()
	{
		for (int x = 0; x < _sizeX; x++)
			for (int y = 0; y < _sizeY; y++)
			for(int z=0;z<_sizeZ;z++)
			{

				float Noise = 0.0f;
				float Scale = 0.25f;
				float fScaleAcc = 0.0f;

				for (int o = 0; o < _octaves; o++)
				{

					int   PitchX = _sizeX >> o;
					int   PitchY = _sizeY >> o;
					int   PitchZ = _sizeZ >> o;
					if (PitchX == 0 || PitchY == 0) return 0;
					int   x0 = (x / PitchX) * PitchX;
					int   y0 = (y / PitchY) * PitchY;
					int   z0 = (z / PitchZ) * PitchZ;

					int   x1 = (x0 + PitchX) % _sizeX;
					int   y1 = (y0 + PitchY) % _sizeY;
					int   z1 = (z0 + PitchZ) % _sizeZ;

					float TX = (float)(x - x0) / (float)PitchX;
					float TY = (float)(y - y0) / (float)PitchY;
					float TZ = (float)(z - z0) / (float)PitchZ;


					if (_use_CosFunc) {
						TX = convertToCos(TX);
						TY = convertToCos(TY);
					}


					float C000 = SeedArray[x0 + _sizeX * y0 + z0 * _sizeX * _sizeY];
					float C001 = SeedArray[x0 + _sizeX * y0 + z1 * _sizeX * _sizeY];
					float C010 = SeedArray[x0 + _sizeX * y1 + z0 * _sizeX * _sizeY];
					float C011 = SeedArray[x0 + _sizeX * y1 + z1 * _sizeX * _sizeY];


					float C100 = SeedArray[x1 + _sizeX * y0 + z0 * _sizeX * _sizeY];
					float C101 = SeedArray[x1 + _sizeX * y0 + z1 * _sizeX * _sizeY];
					float C110 = SeedArray[x1 + _sizeX * y1 + z0 * _sizeX * _sizeY];
					float C111 = SeedArray[x1 + _sizeX * y1 + z1 * _sizeX * _sizeY];


					float C00 = LinearInterpolation(C000, C100, TX);
					float C01 = LinearInterpolation(C001, C101, TX);

					float C10 = LinearInterpolation(C010, C110, TX);
					float C11 = LinearInterpolation(C011, C111, TX);

					float C0 = LinearInterpolation(C00, C10, TY);
					float C1 = LinearInterpolation(C01, C11, TY);


					Noise += LinearInterpolation(C0,C1,TZ) * Scale;


					fScaleAcc += Scale;
					Scale *= _noiseCancelling;
				}

				NoiseArray[x+y*_sizeX+z*_sizeX*_sizeY] = Noise / fScaleAcc;
			}

		return 1;



	}



	float ValueNoise3DTransformation(int x, int y,int z)
	{


		float Noise = 0.0f;
		float Scale = 0.25f;
		float fScaleAcc = 0.0f;

		for (int o = 0; o < _octaves; o++)
		{

			int   PitchX = _sizeX >> o;
			int   PitchY = _sizeY >> o;
			int   PitchZ = _sizeZ >> o;
			
			int   x0 = (x / PitchX) * PitchX;
			int   y0 = (y / PitchY) * PitchY;
			int   z0 = (z / PitchZ) * PitchZ;

			int   x1 = (x0 + PitchX) % _sizeX;
			int   y1 = (y0 + PitchY) % _sizeY;
			int   z1 = (z0 + PitchZ) % _sizeZ;

			float TX = (float)(x - x0) / (float)PitchX;
			float TY = (float)(y - y0) / (float)PitchY;
			float TZ = (float)(z - z0) / (float)PitchZ;


			if (_use_CosFunc) {
				TX = convertToCos(TX);
				TY = convertToCos(TY);
			}


			float C000 = SeedArray[x0 + _sizeX * y0 + z0 * _sizeX * _sizeY];
			float C001 = SeedArray[x0 + _sizeX * y0 + z1 * _sizeX * _sizeY];
			float C010 = SeedArray[x0 + _sizeX * y1 + z0 * _sizeX * _sizeY];
			float C011 = SeedArray[x0 + _sizeX * y1 + z1 * _sizeX * _sizeY];


			float C100 = SeedArray[x1 + _sizeX * y0 + z0 * _sizeX * _sizeY];
			float C101 = SeedArray[x1 + _sizeX * y0 + z1 * _sizeX * _sizeY];
			float C110 = SeedArray[x1 + _sizeX * y1 + z0 * _sizeX * _sizeY];
			float C111 = SeedArray[x1 + _sizeX * y1 + z1 * _sizeX * _sizeY];


			float C00 = LinearInterpolation(C000, C100, TX);
			float C01 = LinearInterpolation(C001, C101, TX);

			float C10 = LinearInterpolation(C010, C110, TX);
			float C11 = LinearInterpolation(C011, C111, TX);

			float C0 = LinearInterpolation(C00, C10, TY);
			float C1 = LinearInterpolation(C01, C11, TY);


			Noise += LinearInterpolation(C0, C1, TZ) * Scale;


			fScaleAcc += Scale;
			Scale *= _noiseCancelling;
		}
		return Noise / fScaleAcc;

	}





public:

	float** output;


	void TransformCube3D()
	{   
		if (_octaves > 7) _octaves--;

		// Back and front
		for (int x = 0; x < _sizeX; x++)
			for (int y = 0; y < _sizeY; y++)
			{
				output[0][x + y * _sizeX] = ValueNoise3DTransformation(x, y, 0);
				output[1][(_sizeX - x -1) + (y-1) * _sizeX] = ValueNoise3DTransformation(x, y, _sizeZ - 1);
			}

		//Up and down
		for (int x = 0; x < _sizeX; x++)
			for (int z = 0; z < _sizeZ; z++)
			{

				output[2][x + z * _sizeX ] = ValueNoise3DTransformation(x, 0, z);
				output[3][x + z * _sizeX ] = ValueNoise3DTransformation(x, _sizeY - 1, z);
			}
		// Left and right
		for (int y = 0; y < _sizeY; y++)
			for (int z = 0; z < _sizeZ; z++)
			{
				output[4][_sizeX - z - 1 + y * _sizeX ] = ValueNoise3DTransformation(0, y, z);
				output[5][z+ y * _sizeX ] = ValueNoise3DTransformation(_sizeX-1, y, z);
			}

		

	}


	ValueNoise(int sizeX, int sizeY = 1,int sizeZ=1)
	{
		_sizeX = sizeX;
		_sizeY = sizeY;
		_sizeZ = sizeZ;
		
		output = new float* [6];
		for (int i = 0; i < 6; i++)
		{
			output[i] = new float[sizeX * sizeX];
		}

		SeedArray = new float[sizeX * sizeY*sizeZ];
		NoiseArray = new float[sizeX * sizeY*sizeZ];
		if (_sizeY > 1)
		{
			if (sizeZ > 1)Dimension = 3;
			else
				Dimension = 2;
		}

	}

	void gen_NewSeed() { for (int i = 0; i < _sizeX * _sizeY*_sizeZ; i++) SeedArray[i] = (float)std::rand() / (float)RAND_MAX; }

	void  transformSeed()
	{
		if (_octaves < 2)_octaves = 2;
		if (_noiseCancelling < 0.1) _noiseCancelling = 0.10f;

		if (Dimension == 1)
			Noise1D();
		else {
			if (Dimension == 3) { if (!Noise3D())_octaves--; }
			else
				if(!Noise2D())_octaves--;
		}
	}


	~ValueNoise()
	{
		delete SeedArray;
		delete NoiseArray;
		delete output;
	}

};
