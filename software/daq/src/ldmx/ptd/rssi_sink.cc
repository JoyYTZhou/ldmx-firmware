#include <rogue/protocols/udp/Core.h>
#include <rogue/protocols/udp/Client.h>

#include <rogue/protocols/rssi/Client.h>
#include <rogue/protocols/rssi/Transport.h>
#include <rogue/protocols/rssi/Application.h>
#include <rogue/protocols/packetizer/CoreV2.h>
#include <rogue/protocols/packetizer/Transport.h>
#include <rogue/protocols/packetizer/Application.h>

#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <rogue/interfaces/stream/Buffer.h>
#include <rogue/Helpers.h>

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>


//! Receive slave data, count frames and total bytes for example purposes.
class TestSink : public rogue::interfaces::stream::Slave {

   public:

      uint32_t rxCount;
      uint64_t rxBytes;
      uint32_t rxLast;

      TestSink() {
         rxCount = 0; // Total frames
         rxBytes = 0; // Total bytes
         rxLast  = 0; // Last frame size
      }

      void acceptFrame (std::shared_ptr<rogue::interfaces::stream::Frame> frame )
      {
         uint32_t nbytes = frame->getPayload();
         rxLast   = nbytes;
         rxBytes += nbytes;
         rxCount++;

         printf ("In acceptFrame\n");

         // Iterators to start and end of frame
         /// rogue::interfaces::stream::Frame::iterator iter = frame->beginRead();
         /// rogue::interfaces::stream::Frame::iterator  end = frame->endRead();

         rogue::interfaces::stream::FrameIterator iter = frame->begin();
         rogue::interfaces::stream::FrameIterator  end = frame->end();

         
         // Example destination for data copy
         uint8_t *buff = (uint8_t *)malloc (nbytes);
         uint8_t  *dst = buff;

         //Iterate through contigous buffers
         while ( iter != end ) {

            //  Get contigous size
            auto size = iter.remBuffer ();

            // Get the data pointer from current position
            auto *src = iter.ptr ();

            // Copy some data
            memcpy(dst, src, size);

            // Update destination pointer and source iterator
            dst  += size;
            iter += size;
         }
      }
};


int main (int argc, char **argv) {
   struct timeval last;
   struct timeval curr;
   struct timeval diff;
   double   timeDiff;
   uint64_t lastBytes;
   uint64_t diffBytes;
   double bw;

   // Create the UDP client, jumbo = true
   rogue::protocols::udp::ClientPtr udp  = rogue::protocols::udp::Client::create("127.0.0.1",8192,true);
   udp->setRxBufferCount(16); // Make enough room for 16 outstanding buffers

   // RSSI
   rogue::protocols::rssi::ClientPtr rssi = rogue::protocols::rssi::Client::create(udp->maxPayload());

   // Packetizer, ibCrc = false, obCrc = true
   rogue::protocols::packetizer::CoreV2Ptr pack = rogue::protocols::packetizer::CoreV2::create(false,true,true);

   // Connect the RSSI engine to the UDP client
   rogueStreamConnectBiDir(udp,rssi->transport());

   // Connect the RSSI engine to the packetizer
   rogueStreamConnectBiDir(rssi->application(), pack->transport());

   // Create a test sink and connect to channel 1 of the packetizer
   std::shared_ptr<TestSink> sink = std::make_shared<TestSink>();
   rogueStreamConnect(pack->application(0),sink);

   // Start the rssi link
   rssi->start();

   // Loop forever showing counts
   lastBytes = 0;
   gettimeofday(&last,NULL);

   while(1) {
      sleep(10);
      gettimeofday(&curr,NULL);

      timersub(&curr,&last,&diff);

      diffBytes = sink->rxBytes - lastBytes;
      lastBytes = sink->rxBytes;

      timeDiff = (double)diff.tv_sec + ((double)diff.tv_usec / 1e6);
      bw = (((float)diffBytes * 8.0) / timeDiff) / 1e9;

      gettimeofday(&last,NULL);

      printf("RSSI = %i. RxLast=%i, RxCount=%i, RxTotal=%li, Bw=%f, DropRssi=%i, DropPack=%i\n",rssi->getOpen(),sink->rxLast,sink->rxCount,sink->rxBytes,bw,rssi->getDropCount(),pack->getDropCount());
   }
}
