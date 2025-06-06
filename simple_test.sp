#include <sourcemod>
#include <profiler>
#include <simple_bytebuffer>


#pragma newdecls required
#pragma semicolon 1

ByteBuffer bf;

public void OnPluginStart()
{
	float vec1[3], vec2[3];
	/////////////
	PrintToServer("========== Test Write ==========");
	bf.Reset();

	bf.WriteBool(true);
	bf.WriteUInt8(255);

	bf.WriteUInt16(111);
	bf.OverwriteUInt16(65535, bf.size - 2);	   // Test Rewrite

	bf.WriteInt32(111);
	bf.OverwriteInt32(2147483647, bf.size - 4);	   // Test Rewrite

	bf.WriteFloat(3.1415926);

	vec1 = { 0.1, 0.2, 0.3 };
	bf.WriteVector(vec1);

	bf.WriteString("");
	bf.WriteString("AAA");

	char a1[10] = "123456789";
	bf.LoadBytes(a1,.startPos = 0,.addSize = 10);

	PrintToServer("Total Write: %i bytes", bf.size);
	char sTemp[4096];
	BytesToHex(bf.bytes, bf.size, sTemp, sizeof(sTemp));
	PrintToServer("Print Hex: %s", sTemp);

	/////////////
	PrintToServer("========== Test Read ==========");
	PrintToServer("ReadBool: %i", bf.ReadBool());
	PrintToServer("ReadUInt8: %i", bf.ReadUInt8());
	PrintToServer("ReadUInt16: %i", bf.ReadUInt16());
	PrintToServer("ReadInt32: %i", bf.ReadInt32());
	PrintToServer("ReadFloat: %f", bf.ReadFloat());

	bf.ReadVector(vec2);
	PrintToServer("ReadVector: %f %f %f", vec2[0], vec2[1], vec2[2]);

	bf.ReadString(sTemp, sizeof(sTemp));
	PrintToServer("ReadString: %s", sTemp);

	bf.ReadString(sTemp, sizeof(sTemp));
	PrintToServer("ReadString: %s", sTemp);

	bf.ReadString(sTemp, sizeof(sTemp));
	PrintToServer("ReadString: %s", sTemp);

	TestDoBench();
}


void TestDoBench()	 
{
	char data[MAX_BYTE_BUFFER_SIZE];

	Handle hProf = CreateProfiler();
	float  min	 = 10.0, max, avg;
	int	   round = 20;
	// Running the test 20 times to gain min/max/avg
	for (int x = 0; x < round; x++)
	{
		StartProfiling(hProf);

		// Running 1000 iterations for our test
		for (int i = 0; i < 1000; i++)
		{
			
        	bf.Reset();
			bf.LoadBytes(data, 0, sizeof(data));
		}

		StopProfiling(hProf);

		float speed = GetProfilerTime(hProf);
		if (speed < min) min = speed;
		if (speed > max) max = speed;
		avg += speed;
		// PrintToChatAll("%i speed %f",x,speed);
	}

	// Display our minimum, maximum and average times.
	avg /= round;
	PrintToServer("Bench: Min %fms. Avg %fms. Max %fms", min, avg, max);
	PrintToChatAll("Bench: Min %fms. Avg %fms. Max %fms", min, avg, max);
	delete hProf;
}



stock void BytesToHex(const char[] bytes, int size, char[] dest, int destLen)
{
	if (destLen < size * 3 + 1)
	{
		ThrowError("dest buffer too small");
		return;
	}
	static char HEX_CHARS[] = "0123456789abcdef";
	int			index		= 0;

	for (int i = 0; i < size; i++)
	{
		// 获取当前字节的高四位和低四位
		int highNibble = (bytes[i] & 0xF0) >> 4;
		int lowNibble  = bytes[i] & 0x0F;

		// 将高低四位转换为对应的十六进制字符

		dest[index++]  = HEX_CHARS[highNibble];
		dest[index++]  = HEX_CHARS[lowNibble];
		dest[index++]  = 32;
	}

	// 添加字符串结束符
	dest[index] = '\0';

	// 返回十六进制字符串
}