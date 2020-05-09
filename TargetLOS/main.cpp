
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "kernel.h"
#include "DTEDFileManager.h"

Aircraft_Data Data;
DTEDFileManager manager;
bool computeSlantRange = false;
bool computeVisibility = false;
bool elevationDataLoaded = false;

//Utility for converting degrees mintes seconds into decimal degress.
float DMS_to_DecimalDegrees(int degree, int minute, float second)
{
	if (degree < 0)
	{
		return (abs(degree) + (minute / 60.0) + (second / 3600.0)) * -1;
	}
	return degree + (minute / 60.0) + (second / 3600.0);
}

//Reads in the user inputs from a file. Since there are a lot of inputs, a
//file seemed like the best option. Returns true on successful read.
bool readInputs()
{
	ifstream file;
	file.open("inputs.txt");

	if (file.is_open())
	{
		string name;
		int lat_deg, lat_min;
		float lat_sec;
		int lon_deg, lon_min;
		float lon_sec;

		//Read in the numeric inputs from the file, skipping the labels
		getline(file, name, '='); file >> Data.A_yaw;
		getline(file, name, '='); file >> Data.A_pitch;
		getline(file, name, '='); file >> Data.A_roll;
		getline(file, name, '='); file >> Data.A_azimuth;
		getline(file, name, '='); file >> Data.A_elevation;
		getline(file, name, '='); file >> lat_deg;
		getline(file, name, '='); file >> lat_min;
		getline(file, name, '='); file >> lat_sec;
		getline(file, name, '='); file >> lon_deg;
		getline(file, name, '='); file >> lon_min;
		getline(file, name, '='); file >> lon_sec;
		getline(file, name, '='); file >> Data.A_altitude;
		getline(file, name, '='); file >> computeSlantRange;
		getline(file, name, '='); file >> computeVisibility;

		char lat_h = (lat_deg < 0) ? 'S' : 'N', lon_h = (lon_deg < 0) ? 'W' : 'E';
		
		printf("\n=== Aircraft Info ===\n");
		printf("Coordinates - Latitude: %d %d' %.2f\" %c | Longitude: %d %d' %.2f\" %c\n", lat_deg, lat_min, lat_sec, lat_h, lon_deg, lon_min, lon_sec, lon_h);
		printf("Altitude: %.2fft\n", Data.A_altitude);
		printf("Orientation in Degrees - Yaw: %.2f | Pitch: %.2f | Roll: %.2f\n", Data.A_yaw, Data.A_pitch, Data.A_roll);
		printf("LOS Direction in Degrees - Azimuth Angle: %.2f | Depression Angle: %.2f\n", Data.A_azimuth, Data.A_elevation);

		//Convert the inputs to what the progmra expects
		Data.A_yaw       *= DEG_TO_RAD;
		Data.A_pitch     *= DEG_TO_RAD;
		Data.A_roll      *= DEG_TO_RAD;
		Data.A_azimuth   *= DEG_TO_RAD;
		Data.A_elevation *= DEG_TO_RAD;
		Data.A_latitude   = DEG_TO_RAD * DMS_to_DecimalDegrees(lat_deg, lat_min, lat_sec);
		Data.A_longitude  = DEG_TO_RAD * DMS_to_DecimalDegrees(lon_deg, lon_min, lon_sec);
		Data.A_altitude  *= FEET_TO_METER;

		file.close();
		return true;
	}

	printf("ERROR: Could not open file: inputs.txt\n");
	return false;
}

int  main(int argc, char** argv)
{
	char c = 1;
	while (c != 'q')
	{
		//Read program inputs from the inputs.txt file
		if (readInputs())
		{
			Data.RN = WGS84_A / sqrt(1 - WGS84_E2 * sin(Data.A_latitude) * sin(Data.A_latitude));
			Data.RM = Data.RN * (1 - WGS84_E2) / (1 - WGS84_E2 * sin(Data.A_latitude) * sin(Data.A_latitude));

			//Rotate the LOS vector frame of reference from the aircraft body to Earth
			calculateABC(&Data);
			printf("LOS Rotation in Degrees - Yaw: %.2f | Pitch: %.2f | Roll %.2f\n\n", Data.LOS_Rotation[0] * RAD_TO_DEG, Data.LOS_Rotation[1] * RAD_TO_DEG, Data.LOS_Rotation[2] * RAD_TO_DEG);

			//Read in the elevation data from the file if it hasn't been read in yet
			if (elevationDataLoaded == false)
			{
				//Selected an arbitrary location in the Grand Canyon
				if (manager.GetDTEDFileData("GrandCanyon-36N-113W.DT0"))
				{
					//Display some elevation
					manager.PrintFileData(50);

					//Allocate and load device memory
					LoadElevationData(&(manager.data));

					elevationDataLoaded = true;
				}
			}

			if (elevationDataLoaded)
			{
				//Try different Target Locations by changing only the altitude
				//Change the aircraft parameters above to adjust the situation.
				if (computeSlantRange)
				{
					CalcTargetLocation(&Data);
				}

				//Try doing a scan to see what locations are visible.
				if (computeVisibility)
				{
					CalcAreaVisibility(&Data, &(manager.data));
				}
			}
		}

		printf("\n\nChange the program parameters in inputs.txt and press ENTER to run again or enter 'q' to terminate.\n");
		c = getchar();
		printf("\n==========================================================\n\n");
	}

	return EXIT_SUCCESS;
}
