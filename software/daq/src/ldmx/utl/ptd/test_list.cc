#include "ldmx/utl/ListInterlocked.hh"
#include "ldmx/utl/Timestamp.hh"
#include "ldmx/utl/Timeout.hh"
#include "ldmx/utl/Timespec.hh"

#include <time.h>
#include <stdio.h>


/* ---------------------------------------------------------------------- *//*!

  \brief Defines the node to be put on the list
                                                                          */
/* ---------------------------------------------------------------------- */
class TestNode : public ldmx::utl::ListNode
{
public:
   TestNode () { return; }
   TestNode (int id) :
      m_id (id)
   {
      return;
   }

public:
   int m_id;
};
/* ---------------------------------------------------------------------- */


static void add (ldmx::utl::ListInterlocked *li, 
                 TestNode                *nodes,
                 int                          n, 
                 int                         id);

   
/* ---------------------------------------------------------------------- *//*!

   \brief Tests the interlocked list

   \param[in] argc  The command line argument count
   \param[in] argv  The command line argument vector
                                                                          */
/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   ldmx::utl::ListInterlocked li;

   TestNode nodes[10];


   add (&li, nodes, 1, 0);
   for (int idx = 0; idx < 100; )
   {
      ldmx::utl::Timeout     begin (0);
      ldmx::utl::Timeout        to (begin, 2*ldmx::utl::Timestamp::OneSecond);

      TestNode    *node = reinterpret_cast<decltype(node)>(li.removeW (to));
      ldmx::utl::Timespec<CLOCK_MONOTONIC> end (0);

      int elapsed = end.elapsedFrom (begin);

      int id = node ? node->m_id : -1;

      printf ("Node[%2d:%2d] @ %6ld.%09ld  elapsed = %10d\n", 
              idx, id, (long int)end.tv_sec, end.tv_nsec, elapsed);


      // ---------------------------------
      // If timed out, add some more nodes
      // ---------------------------------
      if (node == 0)
      {
         add (&li, nodes, 5, idx);
      }
      else
      {
         idx++;
      }
   }      
   
   return 0;
}
/* ---------------------------------------------------------------------- */ 



/* ---------------------------------------------------------------------- *//*!

  \brief Adds some tests nodes to the list

  \param[in]    li  The target interlocked list
  \param[in] nodes  The set of source nodes
  \param[in]     n  The number of nodes to add
  \param[in]    id  An identifying node number
                                                                          */
/* ---------------------------------------------------------------------- */
static void add (ldmx::utl::ListInterlocked *li,
                 TestNode                *nodes,
                 int                          n, 
                 int                         id)
{
   while (n-- > 0)
   {
      nodes->m_id = id++;
      li->insert (nodes);

      nodes += 1;
   }
   // --------------------------------------------------------------


   return;
}
/* ---------------------------------------------------------------------- */
