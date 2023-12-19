using System;
using System.Threading;
using System.Threading.Tasks;

namespace spidlhost
{
    class MainClass
    {
        public static void Main(string[] args)
        {
            SocketListener listener = new SocketListener(8888);

            while(true) {

                listener.Start(1);

                if (listener.Pending()) {
                    Task.Run(() =>
                    {
                        var thread = new SocketThread32(listener.AcceptTcpClient());
                        //var thread = new SocketThread(listener.AcceptTcpClient());
                        thread.DoSocketLoop();
                    });
                }

                System.Threading.Thread.Sleep(1000);
            }


        }
    }
}
