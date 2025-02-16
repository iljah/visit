// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                              avtVariableMapper.C                          //
// ************************************************************************* //

#include <avtVariableMapper.h>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>

#include <avtTransparencyActor.h>


#include <BadIndexException.h>
#include <ImproperUseException.h>


// ****************************************************************************
//  Method: avtVariableMapper constructor
//
//  Programmer: Hank Childs
//  Creation:   December 27, 2000
//
//  Modifications:
//
//    Kathleen Bonnell, Thu Mar 15 19:15:10 PST 2001
//    Removed unused 'hidden' member.  Functionality resides in
//    avtDrawable and avtActor.
//
//    Kathleen Bonnell, Mon Apr  2 18:30:33 PDT 2001 
//    Add intialization of new member lut. 
//
//    Kathleen Bonnell, Mon Jun 25 12:45:06 PDT 2001 
//    Add intialization of lineWidth, stipplePattern. 
//
//    Kathleen Bonnell, Sat Aug 18 18:09:04 PDT 2001
//    Use enum types from LineAttributes.h to ensure proper
//    line width and style are sent down to vtk.
//
//    Kathleen Bonnell, Thu Oct  4 16:28:16 PDT 2001 
//    Initialize limitsMode. 
//
//    Kathleen Bonnell, Tue Dec  3 16:14:25 PST 2002 
//    Re-initialize lineWidth based on new LineAttributes. 
//
//    Brad Whitlock, Mon Sep 18 11:28:35 PDT 2006
//    Added colorTexturingFlag.
//
//    Brad Whitlock, Tue Apr 24 16:01:09 PST 2007
//    Added colorTexturingFlagAllowed.
//
// ****************************************************************************

avtVariableMapper::avtVariableMapper()
{
    setMin = setMax = false;
    lighting = true;
    opacity = 1.;
    lut = NULL; 
    lineWidth = LW_0;
    limitsMode = 0;  // use original data extents
    colorTexturingFlag = true;
    colorTexturingFlagAllowed = true;
}


// ****************************************************************************
//  Method: avtVariableMapper destructor
//
//  Programmer: Kathleen Bonnell 
//  Creation:   March 29, 2001 
//
// ****************************************************************************

avtVariableMapper::~avtVariableMapper()
{
    // nothing here 
}


// ****************************************************************************
//  Method: avtVariableMapper::CustomizeMappers
//
//  Purpose:
//      A hook from the base class that allows the variable mapper to force
//      the vtk mappers to be the same as its state.
//
//  Programmer: Hank Childs
//  Creation:   December 27, 2000
//
//  Modifications:
//
//    Kathleen Bonnell, Thu Mar 15 19:15:10 PST 2001
//    Removed calls to 'Hide' and 'Unhide' methods. Functionality resides in
//    avtDrawable and avtActor.  Added call to SetLineWidth method.
//
//    Hank Childs, Fri Mar 23 17:35:17 PST 2001
//    Fix memory leak.
//
//    Kathleen Bonnell, Mon Apr  2 18:30:33 PDT 2001 
//    Moved creation of default LUT to constructor.
//
//    Kathleen Bonnell, Thu Jun 21 17:33:08 PDT 2001 
//    Add call to set actors' stipple pattern
//
//    Kathleen Bonnell, Sat Aug 18 18:09:04 PDT 2001 
//    Use enum types from LineAttributes in order to ensure that
//    proper pattern is sent down to vtk. 
//
//    Brad Whitlock, Fri Aug 25 10:41:25 PDT 2006
//    Added code to enable color texturing in the mapper.
//
//    Hank Childs, Thu Dec 28 11:08:51 PST 2006
//    Tell our mapper whether or not the scene is 3D.  (This is needed so
//    the rectilinear grid mapper can decide whether or not to do lighting.)
//
//    Kathleen Biagas, Tue Jul 12 13:34:19 MST 2016
//    Removed vtkVisItDataSetMapper.
//
// ****************************************************************************

void
avtVariableMapper::CustomizeMappers(void)
{
    if (setMin)
    {
        SetMin(min);
    }
    else
    {
        SetMinOff();
    }

    if (setMax)
    {
        SetMax(max);
    }
    else
    {
        SetMaxOff();
    }

    //
    // It is probable that the mappers defaults did not actually get set, so
    // explicitly do that here.
    //
    SetMappersMinMax();

    if (lighting)
    {
        TurnLightingOn();
    }
    else
    {
        TurnLightingOff();
    }

    SetOpacity(opacity);

    for (int i = 0; i < nMappers; i++)
    {
        if (mappers[i] != NULL)
        {
            mappers[i]->SetLookupTable(lut);

            // Turn on color texturing in the mapper.
            mappers[i]->SetInterpolateScalarsBeforeMapping(colorTexturingFlag?1:0);
        }
        if (actors[i] != NULL)
        {
            vtkProperty *prop = actors[i]->GetProperty();
            prop->SetLineWidth(LineWidth2Int(lineWidth));
        }
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::SetMin
//
//  Purpose:
//      Sets the plotter's scalar min.
//
//  Arguments:
//      minArg      The new minimum.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2000
//
// ****************************************************************************

void
avtVariableMapper::SetMin(double minArg)
{
    if (setMin == true && min == minArg)
    {
        return;
    }

    min    = minArg;
    setMin = true;

    SetMappersMinMax();
}


// ****************************************************************************
//  Method: avtVariableMapper::SetMinOff
//
//  Purpose:
//      Sets the bounds for the scalar's min to be off.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2000
//
// ****************************************************************************

void
avtVariableMapper::SetMinOff(void)
{
    if (setMin == false)
    {
        return;
    }
    setMin = false;

    SetMappersMinMax();
}


// ****************************************************************************
//  Method: avtVariableMapper::SetMax
//
//  Purpose:
//      Sets the plotter's scalar max.
//
//  Arguments:
//      maxArg      The new maximum.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2000
//
// ****************************************************************************

void
avtVariableMapper::SetMax(double maxArg)
{
    if (setMax == true && max == maxArg)
    {
        return;
    }

    max    = maxArg;
    setMax = true;

    SetMappersMinMax();
}


// ****************************************************************************
//  Method: avtVariableMapper::SetMaxOff
//
//  Purpose:
//      Sets the bounds for the scalar's max to be off.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2000
//
// ****************************************************************************

void
avtVariableMapper::SetMaxOff(void)
{
    if (setMax == false)
    {
        return;
    }
    setMax = false;

    SetMappersMinMax();
}


// ****************************************************************************
//  Method: avtVariableMapper::SetMappersMinMax
//
//  Purpose:
//      Sets the mappers min/max.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2000
//
//  Modifications:
//
//    Kathleen Bonnell, Mon Jul 23 17:47:36 PDT 2001
//    Added test for constant range, in order to tweak the range a little
//    so that VTK will render the plot appropriately.
//
//    Kathleen Bonnell, Thu Oct  4 16:28:16 PDT 2001 
//    Added logic for retrieving the correct range based on value of
//    limitsMode. 
// 
//    Kathleen Bonnell, Mon Jul  1 15:20:03 PDT 2002  
//    Tweak the range in the opposite direction if constant and user 
//    specified a max value to use. 
//    
//    Hank Childs, Mon Jul  8 09:56:41 PDT 2002
//    Add support for transparency.
//
//    Kathleen Bonnell, Wed Mar 19 14:31:42 PST 2003    
//    Removed test for constant range, no longer a problem for vtk. 
//
// ****************************************************************************

void
avtVariableMapper::SetMappersMinMax(void)
{
    if (mappers == NULL)
    {
        //
        // This happens when SetMin is called before the mappers are
        // initialized.
        //
        return;
    }

    double mmin = 0.;
    double mmax = 0.;

    if (limitsMode == 1 ) // use current plot extents
    {
        GetCurrentRange(mmin, mmax);
    }
    else  // use either original data extents or user-specified limits
    {
        GetRange(mmin, mmax);
    }

    if (mmax < mmin)
        mmin = mmax = 0.;
    for (int i = 0 ; i < nMappers ; i++)
    {
        if (mappers[i] != NULL)
        {
            mappers[i]->SetScalarRange(mmin, mmax);
        }
    }

    if (transparencyActor != NULL)
    {
        transparencyActor->InputWasModified(transparencyIndex);
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::GetRange
//
//  Purpose:
//      Gets the range of the variable mapper, taking into account artificial
//      limits.
//
//  Arguments:
//      rmin    The minimum in the range.
//      rmax    The maximum in the range.
//
//  Returns:    True if the extents were found, false otherwise.
//
//  Programmer: Hank Childs
//  Creation:   December 28, 2000
//
//  Modifications:
//
//    Hank Childs, Fri Jan  5 11:10:17 PST 2001
//    Use input to get data extents.
//
//    Hank Childs, Sat Apr  7 21:18:53 PDT 2001
//    Use GetDataRange to account for missing data.
//
//    Hank Childs, Fri Apr 20 14:00:26 PDT 2001
//    Use base class method because GetDataRange was removed.
//
//    Kathleen Bonnell, Tue Dec 18 08:58:09 PST 2001 
//    rmax was not being set if mappers == null (rmin set twice). 
//
//    Hank Childs, Thu Oct 10 08:25:18 PDT 2002
//    Added a return value.
//
// ****************************************************************************

bool
avtVariableMapper::GetRange(double &rmin, double &rmax)
{
    if (mappers == NULL)
    {
        //
        // We have been asked for the range before the input has been set.
        //
        rmin = 0.;
        rmax = 1.;
        return false;
    }

    double de[2];
    bool gotExtents = avtMapper::GetRange(de[0], de[1]);

    rmin = (setMin ? min : de[0]);
    rmax = (setMax ? max : de[1]);

    return gotExtents;
}


// ****************************************************************************
//  Method: avtVariableMapper::TurnLightingOn
//
//  Purpose:
//      Turns lighting on.
//
//  Programmer: Hank Childs
//  Creation:   December 27, 2000
//
//  Modifications:
//    Kathleen Bonnell, Sat Oct 19 15:08:41 PDT 2002 
//    Only use lighting for surface representations.
//
//    Hank Childs, Sun May  6 09:52:06 PDT 2007
//    This method was being called multiple times and trampling previous 
//    ambient settings.  Add some awareness of that mode.
//
// ****************************************************************************

void
avtVariableMapper::TurnLightingOn(void)
{
    lighting = true;

    if (actors == NULL)
    {
        //
        // This happens when TurnLightingOn is called before the input is set.
        //
        return;
    }

    for (int i = 0 ; i < nMappers ; i++)
    {
        if (actors[i] != NULL)
        {
            vtkProperty *prop = actors[i]->GetProperty();
            if (prop->GetRepresentation() == VTK_SURFACE)
            {
                // This method can get called multiple times.  If there are
                // multiple lights, then blindly calling SetAmbient to 0.0 may
                // turn off an ambient light that augments a normal light.  So
                // only set the default lighting attributes if we know we are
                // in "unlit" mode, which would mean diffuse would be not 1.0.
                if (prop->GetDiffuse() == 1.0)
                    ; // no-op, since lighting is already on and we don't want
                      // to kill ambient lights, as per above.
                else if (prop->GetDiffuse() != 1.0)
                {
                    prop->SetAmbient(0.0);
                    prop->SetDiffuse(1.0);
                }
            }
            else 
            {
                prop->SetAmbient(1.0);
                prop->SetDiffuse(0.0);
            }
        }
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::TurnLightingOff
//
//  Purpose:
//      Turns lighting off.
//
//  Programmer: Hank Childs
//  Creation:   December 27, 2000
//
//  Modifications:
//
//    Kathleen Bonnell, Tue Apr 24 10:56:08 PDT 2001
//    Set Ambient from 0 to 1, diffuse from 1 to 0.
//
// ****************************************************************************

void
avtVariableMapper::TurnLightingOff(void)
{
    lighting = false;

    if (actors == NULL)
    {
        //
        // This happens when TurnLightingOff is called before the input is set.
        //
        return;
    }

    for (int i = 0 ; i < nMappers ; i++)
    {
        if (actors[i] != NULL)
        {
            vtkProperty *prop = actors[i]->GetProperty();
            prop->SetAmbient(1.0);
            prop->SetDiffuse(0.0);
        }
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::SetOpacity
//
//  Purpose:
//      Sets the opacity of the actors.
//
//  Arguments:
//      opac    The new opacity.
//
//  Programmer: Hank Childs
//  Creation:   December 27, 2000
//
//  Modifications:
//
//    Hank Childs, Mon Jul  8 09:56:41 PDT 2002
//    Add support for transparency.
//
//    Mark C. Miller, Thu Jan 20 22:27:39 PST 2005
//    Passed opacity to InputWasModified
//
// ****************************************************************************

void
avtVariableMapper::SetOpacity(double opac)
{
    if (opac < 0. || opac > 1.)
    {
        EXCEPTION0(ImproperUseException);
    }
    opacity = opac;

    if (actors == NULL)
    {
        //
        // This happens when SetOpacity is called before the input is set.
        //
        return;
    }

    for (int i = 0 ; i < nMappers ; i++)
    {
        if (actors[i] != NULL)
        {
            vtkProperty *prop = actors[i]->GetProperty();
            prop->SetOpacity(opacity);
        }
    }

    if (transparencyActor != NULL)
    {
        transparencyActor->InputWasModified(transparencyIndex, opac);
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::SetLUT
//
//  Purpose:
//    Causes the mapper to use the specified lookup table. 
//
//  Arguments:
//    lut    The new value for the lookup table. 
//
//  Notes:
//    This method must be called after the call to CustomizeMappers, otherwise
//    the default LUT set there overrides this setup.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   March 08, 2001 
//
//  Modifications:
//
//    Kathleen Bonnell, Mon Apr  2 18:30:33 PDT 2001
//    Set the stored LUT to equal the passed LUT.
//
//    Kathleen Bonnell, Mon Aug 20 17:53:30 PDT 2001
//    Removed parameter 'mode' and removed logic dealing with it. 
//
//    Kathleen Bonnell, Fri Aug 31 10:36:49 PDT 2001 
//    Changed name from SetLUT to SetLookupTable for consistency with
//    other mapprs. 
//
//    Hank Childs, Mon Jul  8 09:56:41 PDT 2002
//    Add support for transparency.
//
// ****************************************************************************

void
avtVariableMapper::SetLookupTable(vtkLookupTable *LUT)
{
    if (lut == LUT)
    {
        // no need to change anything
        return;
    }

    lut = LUT;

    for (int i = 0; i < nMappers; i++)
    {
        if (mappers[i] != NULL)
        {
            mappers[i]->SetLookupTable(lut);
        }
    }

    if (transparencyActor != NULL)
    {
        transparencyActor->InputWasModified(transparencyIndex);
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::SetLineWidth
//
//  Purpose:
//      Sets the line width of for all the actors of plot.
//
//  Arguments:
//      w        The new line width
//
//  Programmer:  Kathleen Bonnell 
//  Creation:    March 15, 2001 
//
//  Modifications:
//
//    Kathleen Bonnell, Sat Aug 18 18:09:04 PDT 2001
//    Use enum types from LineAttributes.h to ensure proper
//    line width and style are sent down to vtk.
//
// ****************************************************************************

void
avtVariableMapper::SetLineWidth(_LineWidth lw)
{
    lineWidth = lw; 

    if ( actors == NULL )
    {
        // this occurs when this method called before input is set.
        return;
    }

    for (int i = 0 ; i < nMappers ; i++)
    {
        if (actors[i] != NULL)
        {
            actors[i]->GetProperty()->SetLineWidth(LineWidth2Int(lineWidth));
        }
    }
}


// ****************************************************************************
//  Method: avtVariableMapper::GetCurrentRange
//
//  Purpose:
//      Gets the current range of the variable mapper. 
//
//  Arguments:
//      rmin          The minimum in the range.
//      rmax          The maximum in the range.
//
//  Returns:    True if the extents were found, false otherwise.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   October 3, 2001
//
//    Kathleen Bonnell, Tue Dec 18 08:58:09 PST 2001 
//    rmax was not being set if mappers == null (rmin set twice). 
//
//    Kathleen Bonnell, Wed May 29 13:10:22 PDT 2002         
//    Allow this method to take into consideration user-defined min/max. 
//
//    Hank Childs, Thu Oct 10 08:25:18 PDT 2002
//    Added a return value.
//
// ****************************************************************************

bool
avtVariableMapper::GetCurrentRange(double &rmin, double &rmax)
{
    if (mappers == NULL)
    {
        //
        // We have been asked for the range before the input has been set.
        //
        rmin = 0.;
        rmax = 1.;
        return false;
    }

    double de[2];
    bool rv = avtMapper::GetCurrentRange(de[0], de[1]);

    rmin = (setMin ? min : de[0]);
    rmax = (setMax ? max : de[1]);

    return rv;
}


// ****************************************************************************
//  Method: avtVariableMapper::SetLimitsMode
//
//  Purpose:
//    Sets the limits mode, which specifies which type of limits to use.
//
//  Arguments:
//    lm        The new limits mode.
//                0: Use Original Data limits
//                1: Use Current Plot limits
//                2: Use user-specified limits. 
//
//  Programmer: Kathleen Bonnell
//  Creation:   October 3, 2001
//
// ****************************************************************************

void
avtVariableMapper::SetLimitsMode(const int lm)
{
    if (lm == limitsMode)
    {
        return;
    }

    limitsMode = lm;

    SetMappersMinMax();
}


// ****************************************************************************
//  Method: avtVariableMapper::GetLighting
//
//  Purpose:
//    Returns the lighting state. 
//
//  Returns:
//    True if lighting is ON, false otherwise.
//
//  Programmer: Kathleen Bonnell
//  Creation:   August 13, 2002 
//
// ****************************************************************************

bool
avtVariableMapper::GetLighting()
{
    return lighting;
}

// ****************************************************************************
// Method: avtVariableMapper::SetColorTexturingFlagAllowed
//
// Purpose: 
//   Sets whether we're allowed to change the color texturing flag.
//
// Arguments:
//   val : True if we can use color texturing and we're allowed to change the
//         flag for color texturing.
//
// Programmer: Brad Whitlock
// Creation:   Tue Apr 24 16:01:51 PST 2007
//
// Modifications:
//   
// ****************************************************************************

void
avtVariableMapper::SetColorTexturingFlagAllowed(bool val)
{
    colorTexturingFlagAllowed = val;
    if(!val)
        colorTexturingFlag = false;
}

// ****************************************************************************
// Method: avtVariableMapper::SetColorTexturingFlag
//
// Purpose: 
//   Sets the color texturing flag in each mapper.
//
// Arguments:
//   val : The new color texturing flag value.
//
// Programmer: Brad Whitlock
// Creation:   Mon Sep 18 11:32:24 PDT 2006
//
// Modifications:
//   Brad Whitlock, Tue Apr 24 16:02:43 PST 2007
//   Return if setting the color texturing flag is not allowed.
//
//   Brad Whitlock, Mon Aug  8 16:40:38 PDT 2011
//   Set the color texturing flag another way.
//
// ****************************************************************************

void
avtVariableMapper::SetColorTexturingFlag(bool val)
{
    if (mappers == NULL || !colorTexturingFlagAllowed)
        return;

    colorTexturingFlag = val;
    for (int i = 0 ; i < nMappers ; i++)
    {
        if (mappers[i] != NULL)
            mappers[i]->SetInterpolateScalarsBeforeMapping(colorTexturingFlag?1:0);
    }
}
