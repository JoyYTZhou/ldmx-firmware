// -*-Mode: C++;-*-


#ifndef LDMX_BUILDER_CLIENT_CFGFILE_HH__
#define LDMX_BUILDER_CLIENT_CFGFILE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   CfgFile.cc
  \brief  Contains the description the configuration file itself.  Its 
          main use is for handling included files and error reporting
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
 * 2021.04.27 jjr Correct error is pass in non-absolute file path into
 *                'compose' and there is no parent (i.e. its the top-level
 *                file.  'compose' was not protected against a null pointer.
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.04 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <string>
#include <vector>
#include <sstream>
#include <iostream>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

class CfgLine;

/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Description of all the lines in a file
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgFile 
{
public:
   CfgFile (char const *filename);
   CfgFile (char const *filename, 
            CfgFile     *parent);

private:
   std::string compose (char const      *filename,
                        std::string const *parent);

public:
   bool        open ();
   CfgFile  *parent ();
   CfgLine *addLine ();

   bool       parse ();


public:
   CfgFile       *m_parent; /*!< The parent file                          */
   std::string  m_filename; /*!< The name of this file                    */
   std::vector<CfgLine> 
                    m_lines; /*!< The lines in this file                  */
   int            m_lineCnt; /*!< NUmber of lines so far                  */
   std::stringstream
                   m_stream; /*!< The file's contents                     */
};
/* ====================================================================== */







/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */

#include "CfgLine.hh"

/* ---------------------------------------------------------------------- */
inline CfgFile::CfgFile (char const *filename) :
   CfgFile (filename, nullptr)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline std::string CfgFile::compose (char const      *filename, 
                                     std::string const *parent)
{
   // Check if already an absolute file path or no parent
   if (filename[0] == '/' || parent == nullptr)
   {
      return std::string (filename);
   }

      
   size_t dirLen = parent->find_last_of ('/');
   return std::string (parent->substr (0, dirLen+1) + filename);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline CfgFile::CfgFile (char const *filename, CfgFile *parent) :
   m_parent     (parent),
   m_filename   (std::move (compose (filename,
                                     parent ? &parent->m_filename : nullptr))),
   m_lines            (),
   m_lineCnt         (0),
   m_stream           ()
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline bool CfgFile::open ()
{
   std::ifstream in;
   in.open (m_filename);
   std::cout << "Opening file: " << m_filename << std::endl;
   if (in.good ())
   {
      // Copy the file into a stringstream
      std::cout << "Open succeeded" << std::endl;
      m_stream << in.rdbuf ();
      
      return false;
   }
   else
   {
      std::cout << "Open failed" << std::endl;
      return true;
   }
}
/* ---------------------------------------------------------------------- */ 



/* ---------------------------------------------------------------------- */
inline   CfgFile *CfgFile::parent ()
{
   return m_parent;
}
/* ---------------------------------------------------------------------- */
            


/* ---------------------------------------------------------------------- */
inline CfgLine *CfgFile::addLine ()
{
   int lineNum = m_lineCnt;
   m_lines.push_back (std::move(CfgLine (this, m_stream.tellg (), lineNum)));
   m_lineCnt   = lineNum + 1;
   return &m_lines[lineNum];
}
/* ---------------------------------------------------------------------- */

#endif
