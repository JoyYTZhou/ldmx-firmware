// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGUTILS_HH__
#define __LDMX_BUILDER_CLIENT_CFGUTILS_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgUtils.hh
  \brief  Configuration file parsing utilities
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
 * 2019.04.11 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "CfgLine.hh"
#include "CfgFile.hh"
#include "CfgParameters.hh"


#include <string.h>
#include <sstream>
#include <iostream>
#include <iomanip>



/* ---------------------------------------------------------------------- *//*!

  \brief Classifies characters
                                                                          */
/* ---------------------------------------------------------------------- */
enum CharType
{
   Invalid       = -2, /*!< Anything other than one of the 3 following    */
   Special       = -1, /*!< Other than whitespace or alpha-numeric        */
   Whitespace    =  0, /*!< Any whitespace character                      */
   AlphaNumeric  =  1  /*!< Any alpha-numeric                             */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
/* Incapsulate testing of character type                                  */
/* ---------------------------------------------------------------------- */
static inline bool isAlphaNumeric (int c) { return c >  0; }
static inline bool isWhitespace   (int c) { return c == 0; }
static inline bool isSpecial      (int c) { return c <  0;}


/* ---------------------------------------------------------------------- *//*!
                                                                          */


/* ---------------------------------------------------------------------- *//*!

  \brief  Removes redundant whitespace in a line.  
  \return The number of characters in the reduced line

  \param[   out]  dst           The reduced line
  \param[in:out]  prvCharacter  The class of the previous character
  \param[    in]  src           The source string to process
  \param[    in]  n             Number of characters in \a src
                                                                          */
/* ---------------------------------------------------------------------- */
inline static int process_string (char         *dst, 
                                  int *prvCharacter, 
                                  char const   *src,
                                  int n)
{
   char     *d = dst;
   int prvChar = *prvCharacter;
   int curChar;

   while (--n >= 0)
   {
      char in = *src++;

      /// std::cout << "Character = " << in  << std::endl;


      // ------------------------------
      // Classify the current character
      // ------------------------------
      if      (isspace (in) ) curChar =   Whitespace;
      else if (in == '='    ) curChar =      Special;
      else if (isprint (in) ) curChar = AlphaNumeric;
      else                    curChar =      Invalid;

      if      (isAlphaNumeric (prvChar))
      { 
         // Always store when previous character was alpha-numeric
         *d++ = in;
      }

      else if (isWhitespace (prvChar) )
      {
         // Previous and current are whitespace, ignore the current
         if (isWhitespace (curChar))   { /* ignore  */  }

         // Replace the previous whitespace with the special character
         else if (isSpecial (curChar)) { /* replace */ d[-1] = in; } 

         // If alpha-numeric, just store
         else                          { /* add new */ *d++  = in; }
      }

      else
      {
         // Previous was 'special, ignore whitespace
         if (isWhitespace (curChar) )  { /* ignore  */ }

         // Current is alpha-numeric or special, store
         else                          { /* add new */ *d++  = in; }
      }


      // Current character now becomes the previous character
      prvChar = curChar;
   }


   /// std::cout <<" Len = " << d - dst << std::endl;
  *prvCharacter = curChar;
   return d - dst;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Removes redundant whitespace in a line.  
  \return The number of characters in the reduced line

  \param[   out]  dst           The reduced line
  \param[in:out]  prvCharacter  The class of the previous character
  \param[    in]  src           The NUL-terminated source string to process

                                                                          */
/* ---------------------------------------------------------------------- */
inline static int process_string (char *dst, int *prvChar, char const *src)
{
   int len = strlen (src);
   len = process_string (dst, prvChar, src, len);
   return len;
}
/* ---------------------------------------------------------------------- */


   

/* ---------------------------------------------------------------------- *//*!

  \brief  Gets the next line and prepares it for parsing
  \return The number of characters in the line

  \param[out]    line Filled in with the line to parse
  \param[ in] lineLen The length of the line
  \param[ in] stream The stream to parse
                                                                          */
/* ---------------------------------------------------------------------- */
inline static int getLine (char                *line, 
                           int               lineLen,
                           CfgFile          *cfgFile,
                           std::stringstream *stream)
{
   char orgLine[128];
   int orgLineNum __attribute__ ((unused)) = cfgFile->m_lineCnt;
   int  n = 0;


   // Initial prvChar = AlphaNumeric, this is a ruse to keep the first char
   int prvChar = AlphaNumeric;

   while (1)
   {

      if (stream->eof ()) 
      {
         return 0;
      }

      // -------------------------------------------
      // Add this line to list of lines in this file
      // -------------------------------------------
      CfgLine *cfgLine __attribute ((unused)) = cfgFile->addLine   ();
      

      stream->getline (orgLine, sizeof (orgLine));
      int orgLen  = stream->gcount ();



      /// std::cout << "getLine:org " << std::setw(3) << cfgLine->getNumber ()
      ///          << ' ' << orgLine << " len = " << orgLen << std::endl;


      // Does this line contain a comment character
      char *p = strchr (orgLine, '#');


      // If no comment character
      if (p == nullptr)
      {
         // Check if continuation line
         if (orgLen > 1)
         {
            char last = orgLine[orgLen - 2];


            /// std::cout << "Last printing char = " << last << std::endl;
            if (last == '\\')
            {

               n += process_string (line + n, &prvChar, orgLine, orgLen - 2);
               continue;
            }
            else
            {
               n += process_string (line + n, &prvChar, orgLine);

               break;
            }
         }
         else
         {
            continue;
         }
      }
      else
      {
         n = process_string (line, &prvChar, orgLine, p - orgLine);
         if (n) break;
         else   continue;
      }
   }

   line[n] = 0;
   /// std::cout << "getLine:wrk " << std::setw(3) << orgLineNum
   ///           << ' ' << line << " len = " << n << std::endl;

   return n;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Get the next token

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getToken (char **str)
{
   static char const *delimiters = " =\t";

   /// std::cout << "getToken::rest <" << *str << '>' << std::endl;
   char const *token = strtok_r (*str, delimiters, str);
   /// if (token) std::cout << "getToken::token <" << token << '>' << std::endl;

   return token;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the next name

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getName (char **str)
{
   static char const *delimiters = ".";

   /// std::cout << "getName:rest   <" << *str << '>' << std::endl;
   char const *token = strtok_r (*str, delimiters, str);
   /// if (token) std::cout << "getName::token <" << token << '>' << std::endl;

   return token;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Get the field name

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getField (char **str)
{
   static char const *delimiters = " =";

   /// std::cout << "getField:rest   <" << *str << '>' << std::endl;
   char const *token = strtok_r (*str, delimiters, str);
   /// if (token )std::cout << "getField::token <" << token << '>' << std::endl;

   return token;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the value name

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getValue (char **str)
{
   static char const *delimiters = " \n";

   /// std::cout << "getValue <" << *str << '>' << std::endl;
   char const *token = strtok_r (*str, delimiters, str);
   /// if (token) std::cout << "getValue <" << token << '>' << std::endl;

   return token;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Get the value name

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getValue (char **str, CfgParameters const &parameters)
{
   char const *token = getValue (str);

   /// std::cout << "Org value <" << token << '>' << std::endl;


   // Translate if symbolic
   while (1)
   {
      auto it = parameters.find (token);
      if (it != parameters.end ())
      {
         token = it->second.c_str ();
         /// std::cout << "New value <" << token << '>' << std::endl;
      }
      else 
      {
         break;
      }
   }


   return token;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Get the value name

  \param[in] str  Pointer to the C-strng
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char const *getValueComma (char **str, bool *last)
{
   static char const *delimiters = ", \t";

   char *p = strpbrk (*str, delimiters);
   if (p)
   {
      char const *token = *str;
      *last = isspace (*p) ? true : false;
      *p    = 0;
      *str = p + 1;

      /// std::cout << "getValueComma <" << *str << '>' << std::endl;
      /// if (token) std::cout << "getValueComma <" << token << '>' << std::endl;

      return token;
   }
   else
   {
      /// std::cout << "getValueComma <" << *str << '>' << std::endl;
      *last  = true;
      return nullptr;
   }
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Compare two strings 
  \retval  true, The two strings are equal
  \retval false, The two strings are not equal

  \param[in] str1  The first  of the two strings to compare
  \param[in] str2  The second of the two strings to compare

  \par
   This implementation uses a case blind compare
                                                                          */
/* ---------------------------------------------------------------------- */
static inline bool compare (char const *str1, char const *str2)
{
   int status = strcasecmp (str1, str2);
   return status == 0 ? true : false;
}
/* ====================================================================== */

#endif
