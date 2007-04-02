// ************************************************************************* //
//                        avtVariableByZoneQuery.h                           //
// ************************************************************************* //

#ifndef AVT_VARIABLEBYZONE_QUERY_H
#define AVT_VARIABLEBYZONE_QUERY_H
#include <query_exports.h>

#include <avtPickByZoneQuery.h>

#include <PickAttributes.h>


class vtkDataSet;


// ****************************************************************************
//  Class: avtVariableByZoneQuery
//
//  Purpose:
//    A query that retrieves var information about a mesh given a 
//    particular domain and zone number.
//
//  Programmer: Kathleen Bonnell
//  Creation:   July 29, 2004
//
//  Modifications:
//    Kathleen Bonnell, Tue Nov  8 10:45:43 PST 2005
//    Added avtDataAttributes arg to Preparation.
//
// ****************************************************************************

class QUERY_API avtVariableByZoneQuery : public avtPickByZoneQuery
{
  public:
                              avtVariableByZoneQuery();
    virtual                  ~avtVariableByZoneQuery();


    virtual const char       *GetType(void)   { return "avtVariableByZoneQuery"; };
    virtual const char       *GetDescription(void)
                                 { return "Retrieving var information on mesh."; };

  protected:
    virtual void                    Preparation(const avtDataAttributes &);
    virtual void                    PostExecute(void);
};


#endif
