// -*-Mode: C++;-*-


#ifndef LDMX_BUILDER_CLIENT_CFGLINE_HH__
#define LDMX_BUILDER_CLIENT_CFGLINE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   CfgParser.cc
  \brief  Contains the description of one line from a configuration file
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.04 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <string>
#include <iostream>


/* ====================================================================== */
/* FORWARD REFERENCE                                                      */
/* ---------------------------------------------------------------------- */

class CfgFile;

/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION: Line parsing context                                       */
/* ---------------------------------------------------------------------- *//*!

  \brief Description of one line

  \par
   Each line contains 
      -# a backpointer to the file containing it
      -# the stream position of the start of this line within that file
      -# the line number of the line with that file
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgLine
{
public:
   CfgLine (CfgFile        *cfgFile, 
            std::streampos position,
            int             lineNum);

public:
   int             getNumber () const;
   void              getLine (char *line, int len);
   CfgFile const *getCfgFile () const;
   void               report ();


   CfgFile        *m_cfgFile;  /*!< The containing file                   */
   std::streampos m_position;  /*!< The stream offset within the file     */
   int             m_lineNum;  /*!< The line number within the file       */
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */

#include "CfgFile.hh"


/* ---------------------------------------------------------------------- *//*!

  \brief Constructor

  \param[in]  cfgFile  The file this line is a member of
  \param[in] position  Beginning position in the string stream of this line
  \param[in]  lineNum  The line number with the file of this line

                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgLine::CfgLine (CfgFile        *cfgFile, 
                         std::streampos position, 
                         int             lineNum) :
   m_cfgFile   (cfgFile),
   m_position (position),
   m_lineNum  ( lineNum)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the line number associated with this line
  \return The line number associated with this line
                                                                          */
/* ---------------------------------------------------------------------- */
inline int CfgLine::getNumber () const
{
   return m_lineNum;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the file that this line is in
  \return A pointer to the CfgFile
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgFile const *CfgLine::getCfgFile () const
{
   return m_cfgFile;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Fills in a copy of this line

  \param[out] line Buffer to hold the contents of the line
  \param[ in]  len Maximum number of characters of \a line
                                                                          */
/* ---------------------------------------------------------------------- */
inline void CfgLine::getLine (char *line, int len)
{
   auto &stream = m_cfgFile->m_stream;
   auto     cur = stream.tellg ();
   stream.seekg (m_position);
   stream.getline (line, len);
   stream.seekg (cur);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void CfgLine::report ()
{
   char line[128];
   getLine (line, sizeof (line));
   
   std::cerr << m_cfgFile->m_filename << ";" << m_lineNum << std::endl
             << "  " << line << std::endl;
   
}
/*====================================================================== */


#endif
