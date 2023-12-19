using System;
using System.Threading;
using System.Net;
using System.Net.Sockets;

namespace spisocket
{
    public class SocketListener : TcpListener
    {
        public SocketListener(int portNumber) : base(IPAddress.Any, portNumber)
        {

        }





    }
}
