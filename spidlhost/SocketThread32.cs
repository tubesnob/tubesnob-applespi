using System;
using System.Threading;
using System.IO;
using System.Net.Sockets;
using System.Linq;
using System.Collections.Generic;
using System.Collections;

namespace spidlhost
{
    public class SocketThread32
    {
        TcpClient _tcpClient;
        public SocketThread32(TcpClient tcpClient) {
            _tcpClient = tcpClient;
            _tcpClient.LingerState = new LingerOption(false, 0);
            _tcpClient.SendTimeout = 30000;
            _tcpClient.ReceiveTimeout = 30000;
            _tcpClient.NoDelay = true;
        }

        public void DoSocketLoop() {

            using (Stream sstream = _tcpClient.GetStream())
            {
                Console.WriteLine("Connected and we have a stream. Waiting for command and filename.");

                string  command;
                string  commandArgument;
                int     packetSize;

                using (BinaryWriter writer = new BinaryWriter(sstream, System.Text.Encoding.ASCII)) {
                    
                    using (StreamReader sr = new StreamReader(sstream, System.Text.Encoding.ASCII))
                    {
                        command = sr.ReadLine();
                        commandArgument = sr.ReadLine();
                        packetSize = int.Parse(sr.ReadLine());

                        Console.WriteLine($"Command = [{command}] FileName = [{commandArgument}] PacketSize = [{packetSize}]");

                        string filePath = $"/users/smentzer/source/tubesnob-applespi/spidl/bin/{commandArgument}";
                        if (!File.Exists(filePath))
                        {
                            Console.WriteLine($"File {filePath} doesn't exist");
                            return;
                        }

                        byte[] fileBytes = File.ReadAllBytes(filePath);
                        Console.WriteLine($"File is {fileBytes.Length}");

                        uint fileSize = (uint)fileBytes.Length;
                        uint numberOfPackets = (uint)((fileSize / packetSize) + ((fileSize % packetSize) > 0 ? 1 : 0));
                        byte[] outputDataBuffer = new byte[packetSize];

                        Console.WriteLine($"Number Of Packets is {numberOfPackets}");

                        // send the initial packet...
                        SendPacketData(writer, 0, numberOfPackets, fileSize, outputDataBuffer);

                        bool doLoop = true;
                        while(doLoop)
                        {
                            Console.WriteLine("Waiting for command from client");
                            command = sr.ReadLine();
                            Console.WriteLine($"Got a command : {command}");
                            commandArgument = sr.ReadLine();
                            switch (command.ToLower())
                            {
                                case "quit": doLoop = false; break;
                                case "sendpacket":
                                    Console.WriteLine($"Sending Packet {commandArgument}");

                                    uint packetNumberToSend = uint.Parse(commandArgument);
                                    byte[] data = fileBytes.Skip((int)(packetSize * (packetNumberToSend - 1))).Take((int)packetSize).ToArray();
                                    Array.Clear(outputDataBuffer, 0, outputDataBuffer.Length);
                                    Array.Copy(data, outputDataBuffer, data.Length);
                                    SendPacketData(writer, packetNumberToSend, numberOfPackets, (uint)data.Length, outputDataBuffer);
                                    break;
                            }
                        };

                        Console.WriteLine("out of the read loop");
                        writer.Flush();
                    }

                }


            }

            Console.WriteLine("Closing TcpClient");
            _tcpClient.Close();
        }

        static void SendPacketData(BinaryWriter writer, uint packetNumber, uint totalNumberOfPackets, uint packetDataLength, byte[] packetData) {
            writer.Write((UInt32)packetNumber);
            writer.Write((UInt32)totalNumberOfPackets);
            writer.Write((UInt32)packetDataLength);
            ushort crc = (ushort) new NuFXLib.CRC16().Nu_CalcCRC16((ushort)packetNumber, packetData, (ushort)Math.Min(packetData.Length, packetDataLength));
            writer.Write(crc);
            writer.Write(packetData);
            writer.Flush();
            Console.WriteLine($"Sending packet {packetNumber}/{totalNumberOfPackets} of length {packetDataLength} with crc {crc:X}");
        }

    }

}
