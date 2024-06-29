/**
 * Socket extension sendto selftest
 */

#include <sourcemod>
#include <socket>
#include <bytebuffer>

#pragma newdecls required
#pragma semicolon 1

ByteBuffer writer;
ByteBuffer reader;

public Plugin myinfo =
{
	name		= "udp socket selftest",
	author		= "Player",
	description = "testing",
	version		= "1.0.0",
	url			= ""
};

public void OnPluginStart()
{
	Socket socket = new Socket(SOCKET_UDP, OnSocketError);
	socket.Bind("0.0.0.0", 11223);
	SocketConnect(socket, OnSocketConnect, OnSocketReceive, OnSocketDisconnect, "127.0.0.1", 11223);
}

void OnSocketConnect(Socket socket, any arg)
{

	PrintToServer("OnSocketConnect");
	writer.Reset();

	writer.WriteBool(true);
	writer.WriteUInt8(255);
	writer.WriteUInt16(65535);
	writer.WriteInt32(123456);
	writer.WriteFloat(3.1415926);
	writer.WriteString("abc123");

	socket.Send(writer.bytes, writer.size);
}

void OnSocketReceive(Socket socket, char[] receiveData, const int dataSize, any arg)
{
	static char sTemp[64];
	PrintToServer("OnSocketReceive: received %d bytes", dataSize);
	reader.Reset();
	reader.LoadBytes(receiveData, 0, dataSize);

	PrintToServer("ReadBool: %i", reader.ReadBool());
	PrintToServer("ReadUInt8: %i", reader.ReadUInt8());
	PrintToServer("ReadUInt16: %i", reader.ReadUInt16());
	PrintToServer("ReadInt32: %i", reader.ReadInt32());
	PrintToServer("ReadFloat: %f", reader.ReadFloat());
	reader.ReadString(sTemp, sizeof(sTemp));
	PrintToServer("ReadString: %s", sTemp);
}

void OnSocketError(Socket socket, const int errorType, const int errorNum, any arg)
{
	LogError("socket error %d (errno %d)", errorType, errorNum);
	CloseHandle(socket);
}

void OnSocketDisconnect(Socket socket, any arg)
{
	PrintToServer("OnSocketDisconnect");
	CloseHandle(socket);
}
