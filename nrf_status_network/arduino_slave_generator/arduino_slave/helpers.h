//Board package
//DHT110000000000000002
byte boardPack[32] = { 0x44, 0x48, 0x54, 0x31, 0x31, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2 };

//The Sensors pack
byte Sensors[2][32] = 
{
	{ 0x54, 0x65, 0x6D, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x4, 0x0, 0x0, 0x0 },
	{ 0x48, 0x75, 0x6D, 0x69, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x4, 0x0, 0x1, 0x0 }

};
unsigned char nDataS[2] = { 4, 4};