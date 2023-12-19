using System;
using System.Threading;
using System.IO;
using System.Net.Sockets;
using System.Linq;
using System.Collections.Generic;
using System.Collections;

namespace spidlhost
{
    public class SocketThread
    {
        TcpClient _tcpClient;
        public SocketThread(TcpClient tcpClient) {
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

                        string filePath = $"/users/smentzer/source/applespi/{commandArgument}";
                        if (!File.Exists(filePath))
                        {
                            Console.WriteLine($"File {filePath} doesn't exist");
                            return;
                        }

                        byte[] fileBytes = File.ReadAllBytes(filePath);
                        Console.WriteLine($"File is {fileBytes.Length} bytes");

                        ushort fileSize = (ushort)fileBytes.Length;
                        ushort numberOfPackets = (ushort)((fileSize / packetSize) + ((fileSize % packetSize) > 0 ? 1 : 0));
                        byte[] outputDataBuffer = new byte[packetSize];

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

                                    ushort packetNumberToSend = ushort.Parse(commandArgument);
                                    byte[] data = fileBytes.Skip(packetSize * (packetNumberToSend - 1)).Take(packetSize).ToArray();
                                    Array.Clear(outputDataBuffer, 0, outputDataBuffer.Length);
                                    Array.Copy(data, outputDataBuffer, data.Length);
                                    SendPacketData(writer, packetNumberToSend, numberOfPackets, (ushort)data.Length, outputDataBuffer);
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

        static void SendHeaderPacket(BinaryWriter writer, ushort totalNumberOfPackets, ushort totalFileSize)
        {
            byte[] buffer = new byte[512];
            writer.Write((ushort)0);
            writer.Write((ushort)totalNumberOfPackets);
            writer.Write((ushort)buffer.Length);
            writer.Write((ushort)0);
            writer.Write(buffer);
            writer.Flush();
        }

        static void SendPacketData(BinaryWriter writer, ushort packetNumber, ushort totalNumberOfPackets, ushort packetDataLength, byte[] packetData) {
            writer.Write((ushort)packetNumber);
            writer.Write((ushort)totalNumberOfPackets);
            writer.Write((ushort)packetDataLength);
            writer.Write((ushort)gen_crc16(packetData, (ushort) Math.Min(packetData.Length,packetDataLength)));
            writer.Write(packetData);
            writer.Flush();
        }


        static ushort gen_crc16(byte[] data, ushort size)
        {
            ushort crcout = 0;
            int bits_read = 0;
            int bit_flag = 0;
            int pos = 0;

            /* Sanity check: */
            if(data == null)
                return 0;

            while(size > 0)
            {
                bit_flag = crcout >> 15;

                /* Get next bit: */
                crcout <<= 1;
                    crcout |= (ushort)((data[pos] >> (7 - bits_read)) & 1);

                /* Increment bit counter: */
                bits_read++;
                if(bits_read > 7)
                {
                    bits_read = 0;
                    pos++;
                    size--;
                }

                /* Cycle check: */
                if(bit_flag!=0)
                    crcout ^= (ushort)0x8005;
            }

            return crcout;
        }


    }

}
