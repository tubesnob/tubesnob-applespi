using System;
using System.Threading;
using System.IO;
using System.Net.Sockets;
using System.Linq;
using System.Collections.Generic;
using System.Collections;

namespace spisocket
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

            byte[] receiveBuffer = new byte[0x8000];

            using (Stream sstream = _tcpClient.GetStream())
            {
                while (true)
                {


                    if (!_tcpClient.Connected) break;

                    int bytesAvailable = Math.Min(_tcpClient.Available, 0x3FF);
                    if (bytesAvailable > 0) {

                        sstream.Read(receiveBuffer, 0, bytesAvailable);

                        //string receivedData = System.Text.ASCIIEncoding.ASCII.GetString(receiveBuffer, 0, bytesAvailable);

                        //byte[] sendBuffer = System.Text.ASCIIEncoding.ASCII.GetBytes(
                        //    $"I got this data : {receivedData}"
                        //);

                        //sstream.Write(sendBuffer, 0, sendBuffer.Length);

                        //byte firstItem = receiveBuffer[0];
                        //for (int checkPos = 0; checkPos < bytesAvailable; checkPos++) {
                        //
                        //}

                        bool goodCheck = receiveBuffer.Take(bytesAvailable).All(p => ( p >= 65 && p <= 95 ));


                        Console.WriteLine($"Received {bytesAvailable} characters... Good={goodCheck}");

                    }

                    Thread.Sleep(10);

                }

}
        }



    }
}
