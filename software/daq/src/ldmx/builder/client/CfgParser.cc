// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   CfgParser.cc
  \brief  Parses a configuration file
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
 * 2021.04.27 jjr Reference CfgParser.hh from public include directory
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.04 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/builder/client/CfgParser.hh"
#include "CfgUtils.hh"
#include "CfgLine.hh"
#include "CfgFile.hh"

#include "CfgContributor.hh"
#include "CfgCollection.hh"
#include "CfgParameters.hh"
#include "CfgConfiguration.hh"

#include "CfgObject.hh"
#include "CfgObjects.hh"


#include "ldmx/builder/client/Configuration.hh"

#include <string>
#include <string.h>
#include <ios>
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <map>



/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class CfgContext;
class CfgObjects;
class CfgParameters;
class CfgFile;
class CfgLine;
/* ====================================================================== */





/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static bool          processDefine (CfgContext      *context,
                                    char              **rest,
                                    CfgLine         *cfgLine);
static bool             transcribe (ldmx::builder::client::Configuration *cfg,
                                    CfgContext                     const *ctx);

/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ---------------------------------------------------------------------- */
CfgParser::CfgParser ()
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructor set the file name

  \param[in] filename The name of the configuration file
                                                                          */
/* ---------------------------------------------------------------------- */
CfgParser::CfgParser (char const *filename) :
   m_filename (filename)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief CfgParser destructor, basically closes the file

                                                                          */
/* ---------------------------------------------------------------------- */
CfgParser::~CfgParser ()
{
   m_stream.close ();
   m_filename = nullptr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Open the configuration file
                                                                          */
/* ---------------------------------------------------------------------- */
int CfgParser::open (const char *filename)
{
   m_filename = filename;
   m_stream.open (filename, std::ios_base::in);
   if (m_stream.good ())
   {
      return 0;
   }
   else
   {
      return 1;
   }
}
/* ======================================================================- */




/* ====================================================================== */
/* DEFINITION: File/line parsing context                                  */
/* ---------------------------------------------------------------------- *//*!

  \brief Description of all files
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgFiles : public std::vector<CfgFile>
{
   CfgFiles () :
      std::vector<CfgFile> ()
   {
      return;
   }
};
/* ---------------------------------------------------------------------- */





class CfgContributor;
class CfgParameter;
class CfgCollection;
class CfgOutput;





/* ---------------------------------------------------------------------- */
class CfgContext
{
public:
   CfgContext () { return; }

public:
   CfgParameters  m_parameters;
   CfgObjects        m_objects;
   std::string m_configuration;
};
/* ---------------------------------------------------------------------- */






   

/* ---------------------------------------------------------------------- *//*!

  \brief Parses the specified file, filling the configuration class

  \param[out]  cfg The configuration class to fill
                                                                          */
/* ---------------------------------------------------------------------- */
bool CfgParser::parse (ldmx::builder::client::Configuration *cfg)
{
   bool                     err;
   std::vector<CfgFile *> files;
   CfgContext           context;

   
   // -----------------------
   // Open the top level file
   // -----------------------
   CfgFile *cfgFile = new CfgFile (m_filename);
   err = cfgFile->open ();
   if (err)
   {
      std::cerr << "Could not open: " << m_filename << std::endl;
      return true;
   }


   // ----------------------------------------------
   // Add to the list of files and locate its stream
   // ----------------------------------------------
   files.push_back (cfgFile);
   std::stringstream *stream = &cfgFile->m_stream;


   // -----------------------
   // Loop over all the input
   // -----------------------
   while (1)
   {
      // -----------------------------------
      // Check if hit the end of this stream
      // -----------------------------------
      if (stream->eof ())
      {
         // Hit EOF, try going back to the parent file
         // and continue reading
         cfgFile = cfgFile->parent ();

         // No parent file, then that's it
         if (cfgFile == nullptr) 
         {
            break;
         }

         // Continue reading from previous place in the parent
         stream = &cfgFile->m_stream;
      }



      // -----------------------------------
      // Get a working copy of the next line
      // ----------------------------------- 
      char  line[1024];
      int lineNum = cfgFile->m_lineCnt;
      int n = getLine (line, sizeof (line), cfgFile, stream);
      if (n == 0) continue;

      // First line of the parsed line
      CfgLine *cfgLine = &cfgFile->m_lines[lineNum];

      // -----------------------
      // Start parsing this line
      // -----------------------
      char *p     = line;
      char **rest = &p;
      


      // Get the key word
      char const *verb = getToken (rest);
      char upverb[12];
      strcpy (upverb, verb);
      /// std::cout << "Verb <" << verb << '>' << std::endl;


      // --------------------------------------------------
      // INCLUDE FILE
      // --------------------------------------------------
      if (strcmp (verb, "Include") == 0)
      {
         char const *filename = getToken (rest);
         CfgFile  *newCfgFile = new CfgFile (filename, cfgFile);
         newCfgFile->open ();
         files.push_back (newCfgFile);
         cfgFile = newCfgFile;
         stream  = &cfgFile->m_stream;
      }


      // --------------------------------------------------
      // DEFINE
      // --------------------------------------------------
      else if (strcmp (verb, "Define") == 0)
      {
         err = processDefine (&context, rest, cfgLine);
         if (err) return true;
      }



      // --------------------------------------------------
      // SET
      // --------------------------------------------------
      else if (strcmp (verb, "Set") == 0)
      {
         char const *name  = getToken (rest);
         /// std::cout << "Set <" << name << '>' << std::endl;

         if (compare (name, "PARAMETER"))
         {
            char const *name  = getToken (rest);
            char const *value = getValue (rest, context.m_parameters);
            ///std::cout << "Set Parameter <" << name << "> = " << value << std::endl;
            context.m_parameters[name] = value;
         }
         else
         {
            char *tmp = const_cast<decltype(tmp)>(name);
            name = getName (&tmp);
            /// std::cout << "Set::variable <" << name << '>' << std::endl;      
            auto const  it    = context.m_objects.find (name);
            if (it != context.m_objects.end ())
            {
               CfgObject     *object = it->second.get ();
               char const *fieldName = getField (&tmp);
               bool err = object->set (fieldName, context.m_parameters, rest);
               if (err)
               {
                  std::cerr << "Error setting <" << name << '>' << std::endl;
                  cfgLine->report ();
                  return true;
               }
               /// std::cout << "Set::variable <" << name << "> was set" << std::endl;      
            }
            else
            {
               std::cerr << "CfgParser::error in Set, " 
                         << name << " not found" << std::endl;
               cfgLine->report ();
               return true;
            }
         }
      }
      
      else if (strcmp (verb, "Use") == 0)
      {
         char const *what = getToken (rest);
         /// std::cout << "Use " << what << '>' << std::endl;

         if (compare (what, "CONFIGURATION"))
         {
            char const *name = getToken (rest);
            /// std::cout << "Use configuration: <" << name << '>' << std::endl;
            context.m_configuration = name;
         }
         else
         {
            std::cerr << "CfgParser::error Use <" 
                      << what << "> not recognized" << std::endl;
            cfgLine->report ();
            return true;
         }
      }
      
      else
      {
         std::cerr << "CfgParser::error encountered unknonw command" 
                   << verb << std::endl;
         cfgLine->report ();
         return true;
      }
         
   }

   /// context.m_objects.print ();
   err = transcribe (cfg, &context);
   return false;
}
/* ---------------------------------------------------------------------- */



#include "ldmx/builder/Logging.hh"

/* ---------------------------------------------------------------------- *//*!

  \brief  Translate the logging level string to its proper enumerated value
  \return The logging level

  \param[in] levelStr The string to translate
                                                                          */
/* ---------------------------------------------------------------------- */
inline static ldmx::builder::Logging::Level 
getLevel (std::string const &levelStr)
{
   using Level = ldmx::builder::Logging::Level;
      
   char const *string = levelStr.c_str ();
   Level        level = compare (string,    "DEBUG") ? Level::Debug
                      : compare (string,     "INFO") ? Level::Info
                      : compare (string,  "WARNING") ? Level::Warning
                      : compare (string,    "ERROR") ? Level::Error
                      : compare (string, "CRITICAL") ? Level::Critical
                      : Level::None;
   return level;
}
/* ---------------------------------------------------------------------- */




inline static bool
       addContributor (std::vector<ldmx::builder::client::CfgContributor> &dstCtbs, 
                       CfgParameters const &prms,
                       CfgContributor const *ctb)
{
   /// ctb->print (ctb->m_name, 0);
   dstCtbs.emplace_back (*ctb);
   return false;
}



/* ---------------------------------------------------------------------- *//*!

  \brief  Adds a collection of contributors
  \return true on error

  \param[out]  dstCtbs   The vector of contributors to add to
  \param[ in]  objects   A map of the symbolic objects.  This is used to 
                         translate a collection member name to a contributor
  \param[ in] parameters A map of symbolic parameters (unused as of now)
  \param[ in] collection The collection of contributors
                                                                          */
/* ---------------------------------------------------------------------- */  
static bool addContributors (std::vector<ldmx::builder::client::CfgContributor>
                             &dstCtbs, 
                             CfgObjects    const &objects,
                             CfgParameters const &parameters,
                             CfgCollection const *collection)
{
   auto end = objects.cend ();

   // ------------------------------------
   // Process all member of the collection
   // ------------------------------------
   for (auto &name : collection->m_members)
   {
      /// std::cout << "CfgParser::transcribe Processing ctb " << name << std::endl;
      

      // -----------------------------------------
      // Look-up the name and check that it exists
      // -----------------------------------------
      auto it  = objects.find (name);
      if (it == end)
      {
         std::cerr << "CfgParser::transcribe Error ctb "
                   << name << " not found" << std::endl;
         return true;
      }
      

      // -------------------------------------------------------
      // Have a translation, process only if it is a contributor
      // -------------------------------------------------------
      CfgObject const *object = it->second.get ();
      if (object->m_type != CfgObject::Type::Contributor)
      {
         std::cerr << "CfgParser::transcribe Error ctb "
                   << name << " is a contributor type = " 
                   << static_cast<int>(object->m_type) << std::endl;
         return true;
      }
      

      // ---------------------------------------
      // Verified it is a contributor, so add it
      // ---------------------------------------
      bool err = addContributor (dstCtbs,
                                 parameters, 
                                 reinterpret_cast<CfgContributor const *>(object));
      if (err)
      {
         std::cerr << "CfgParser::transcribe Error adding ctb "
                   << name << std::endl;
         return true;
      }

   }
   return false;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
bool transcribe (ldmx::builder::client::Configuration *cfg,
                 CfgContext                     const *ctx)
{


   /// std::cout << std::endl << std::endl
   ///           << "Using configuration: " << ctx->m_configuration << std::endl;

   std::string const &cfgName  = ctx->m_configuration;
   auto                     it = ctx->m_objects.find(cfgName);
   auto                    end = ctx->m_objects.cend ();

   cfg->m_name = cfgName;

   // --------------------------------
   // Check if found the configuration
   // --------------------------------
   if (it == end)
   {
      std::cerr << "CfgParser::transcribe Error, configuration " 
                << cfgName << " not found" << std::endl;
      return true;
   }


   /// std::cout << "CfgParser::transcribe Processing configuration " 
   ///          << cfgName << std::endl; 
  


   // -----------------------------------------------------------------
   // Retrieve the named object and check it is a configuration objecct
   CfgObject const *object = it->second.get ();
   // -----------------------------------------------------------------
   if (object->m_type != CfgObject::Type::Configuration)
   {
      std::cerr << "CfgParser::transcribe Error, configuration " 
                << cfgName << " is not a configuration" << std::endl;
   }


   // -----------------------------------------------------------------
   // Extract the number of fragments to buffer and the reporting level
   // -----------------------------------------------------------------
   auto cfgObject    = reinterpret_cast<CfgConfiguration const *>(object);
   cfg->m_nfragments = strtoul  (cfgObject->m_nfragments.c_str(), 0, 0);
   cfg->m_level      = getLevel (cfgObject->m_loggingLevel);


   // ------------------------------
   // Get the vector of contributors
   // ------------------------------
   std::vector<std::string> const &ctbNames = cfgObject->m_contributors;
   {
      // ----------------------------
      // Process all the contributors
      // ----------------------------
      auto ctbNamesIt  = ctbNames.cbegin ();
      auto ctbNamesEnd = ctbNames.cend   ();
      while (ctbNamesIt != ctbNamesEnd)
      {
         std::string const &ctbName = *ctbNamesIt;

         /// std::cout << "CfgParser::transcribe Processing contributor " 
         ///           << ctbName << std::endl;


         // ---------------------------------------------
         // Look up this named object and check it exists
         // ---------------------------------------------
         auto  it = ctx->m_objects.find (ctbName);
         if (it == end)
         {
            std::cerr << "CfgParser::transcribe Error contributor " 
                      << ctbName << " not found" << std::endl;
            return true;
         }

         // ----------------------------------------
         // Get a pointer to the object
         // This could be either a contribution itself
         // or a collection of contributions
         // ------------------------------------------
         CfgObject const *ctbObject = it->second.get ();


         // --------------------------
         // Check if have a collection
         // --------------------------
         if (ctbObject->m_type == CfgObject::Type::Collection)
         {
            /// std::cout << "CfgParser::transcribe Process ctb collection "
            ///           << ctbName << std::endl;


           auto ctbCollection = reinterpret_cast<CfgCollection const *>(ctbObject);
           bool           err = addContributors (cfg->m_contributors,
                                                 ctx->m_objects,
                                                 ctx->m_parameters,
                                                 ctbCollection);

           if (err)
           {
              std::cerr << "CfgParser::transcribe Error adding ctb "
                        << ctbName << std::endl;
              return true;
           }

         }
         else if (ctbObject->m_type == CfgObject::Type::Contributor)
         {
            /// std::cout << "Adding contributor " << ctbObject->m_name << std::endl;

            bool err = addContributor (cfg->m_contributors,
                                       ctx->m_parameters, 
                                       reinterpret_cast<CfgContributor const *>
                                       (ctbObject));
            if (err)
            {
               std::cerr << "CfgParser::transcribe Error adding ctb "
                         << ctbName << std::endl;
               return true;
            }

         }
         else
         {
            std::cerr << "CfgParser::transcribe Error " << ctbName 
                      << "name " /// << ctbObject->m_name 
                      << " is not a collection or contributor type is "
                      << static_cast<int>(ctbObject->m_type)   << std::endl;
            return true;
         }

         ctbNamesIt++;
      }
   }


   return false;
}



/* ---------------------------------------------------------------------- *//*!
  
  \brief Process a define verb
  retval true, if the processing failed
  \retval false, if the prcessing succeeded

  \param[out]  contexto The parsing context
  \param[ in]     rest The rest of the define line to process
  \param[ in]  cfgLine The configuration line context, used for error 
                       reporting
                                                                          */
/* ---------------------------------------------------------------------- */
static bool processDefine (CfgContext    *context,
                           char            **rest,
                           CfgLine       *cfgLine)
{
   bool err;
   char const *objectType = getToken (rest);
   

   
   // ---------------------------
   // Process: Define Contributor
   // ---------------------------
   if (strcmp (objectType, "Contributor") == 0)
   {
      char const                              *name = getToken (rest);
      std::unique_ptr<CfgContributor> 
           contributor = std::make_unique<CfgContributor>(name, cfgLine);

      err = contributor->parse (rest, context->m_parameters);
      if (err)
      {
         std::cerr << "CfgParse::error in parsing Define Contribution"
                   << std::endl;
         contributor.reset ();  ///delete contributor;
         cfgLine->report ();
         return true;
      }

      context->m_objects[name]  = std::move (contributor);
   }



   // --------------------------
   // Process: Define Collection
   // --------------------------
   else if (strcmp (objectType, "Collection") == 0)
   {
      char const    *name       = getToken (rest);
      std::unique_ptr<CfgCollection> 
           collection = std::make_unique<CfgCollection> (name, cfgLine);

      err = collection->parse (rest, context->m_parameters);
      if (err)
      {
         std::cerr << "CfgParse::error in parsing Define Collection"
                   << std::endl;
         collection.reset (); ///delete collection;
         cfgLine->report ();
         return true;
      }
      
      context->m_objects[name]  = std::move (collection);
   }
   

   // ------------------------------
   // Process:: Define Configuration
   // ------------------------------
   else if (strcmp (objectType, "Configuration") == 0)
   {
      char const *name = getToken (rest);
      std::unique_ptr<CfgConfiguration>
         configuration = std::make_unique<CfgConfiguration> (name, cfgLine);

      err = configuration->parse (rest, context->m_parameters);

      if (err)
      {
         std::cerr << "CfgParse::error in parsing Define Configuration"
                   << std::endl;
         configuration.reset (); ///delete configuration;
         cfgLine->report ();
         return true;
      }
      
      context->m_objects[name]  = std::move (configuration);
      
      /// std::cout << "Configuration added object " << name  << std::endl;
   }


         

   // ---------------------------------
   // Process:: Unknown processing type
   // ---------------------------------
   else
   {
      std::cerr << "CfgParse::error unrecognized object " 
                << objectType << " @" << std::endl;
      cfgLine->report ();
      return true;
   }


   return false;
}
/* ---------------------------------------------------------------------- */




/* ====================================================================== */

