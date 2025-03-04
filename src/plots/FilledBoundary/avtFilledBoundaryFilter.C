// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                          avtFilledBoundaryFilter.C                        //
// ************************************************************************* // 

#include <avtFilledBoundaryFilter.h>

#include <visit-config.h> // For LIB_VERSION_LE

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkGeometryFilter.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <avtDataAttributes.h>
#include <avtOriginatingSource.h>

#include <DebugStream.h>
#include <ImproperUseException.h>

#include <string>
#include <vector>

using std::string;
using std::vector;



// ****************************************************************************
//  Method: avtFilledBoundaryFilter constructor
//
//  Arguments:
//
//  Programmer: Kathleen Bonnell 
//  Creation:   November 10, 2004 
//
//  Modifications:
//
// ****************************************************************************

avtFilledBoundaryFilter::avtFilledBoundaryFilter()
{
    keepNodeZone = false; 
}


// ****************************************************************************
//  Method: avtFilledBoundaryFilter::SetPlotAtts
//
//  Purpose:    Sets the FilledBoundaryAttributes needed for this filter.
//
//  Programmer: Jeremy Meredith
//  Creation:   May  9, 2003
//
//  Note:  taken almost verbatim from the Subset plot
//
// ****************************************************************************

void
avtFilledBoundaryFilter::SetPlotAtts(const FilledBoundaryAttributes *atts)
{
    plotAtts = *atts;
}


// ****************************************************************************
//  Method: avtFilledBoundaryFilter::ExecuteDataTree
//
//  Purpose:    Break up the dataset into a collection of datasets, one
//              per boundary.
//
//  Programmer: Jeremy Meredith
//  Creation:   May  9, 2003
//
//  Note:  taken almost verbatim from the Subset plot
//
//  Modifications:
//    Kathleen Bonnell, Mon Sep  8 13:22:14 PDT 2003
//    Added test for NO cells for early return. 
//
//    Hank Childs, Thu Feb 21 16:38:36 PST 2008
//    Initialize nSelectedBoundaries, in case there is a parsing problem.
//
//    Jeremy Meredith, Fri Apr 30 17:08:32 EDT 2010
//    We can actually get here without polydata, in the event of a clean
//    data set in wireframe mode.  We just convert to polydata first now,
//    which may not work perfectly or optimally in the general case, but
//    is acceptable for how it's being used at the moment.
//
//    Eric Brugger, Tue Aug 19 10:15:20 PDT 2014
//    Modified the class to work with avtDataRepresentation.
//
// ****************************************************************************

avtDataTree_p
avtFilledBoundaryFilter::ExecuteDataTree(avtDataRepresentation *in_dr)
{
    //
    // Get the VTK data set, the domain number, and the label.
    //
    vtkDataSet *in_ds = in_dr->GetDataVTK();
    int domain = in_dr->GetDomain();
    std::string label = in_dr->GetLabel();

    if (in_ds == NULL || in_ds->GetNumberOfPoints() == 0 ||
        in_ds->GetNumberOfCells() == 0)
    {
        return NULL;
    }

    vector<string> labels;
    int            nDataSets = 0;
    vtkDataSet   **out_ds = NULL;

    vtkDataArray *boundaryArray = in_ds->GetCellData()->GetArray("avtSubsets");

    bool splitMats = plotAtts.GetDrawInternal();
    if (boundaryArray && !splitMats)
    {
        if (label.find(";") == string::npos)
        {
            debug1 << "POSSIBLE ERROR CONDITION:  " << endl;
            debug1 << "    avtFilledBoundaryFilter encountered a label ("
                   << label.c_str() << ")" << endl;
            debug1 << "    that cannot be parsed correctly.  This can happen "
                   << "if" << endl;
            debug1 << "    another filter has over-written the boundary labels "
                   << "in" << endl;
            debug1 << "    its output data tree.  avtFilledBoundaryFilter is returning"
                   << endl;
            debug1 << "    an empty data tree." << endl;
            avtDataTree_p rv = new avtDataTree();
            return rv;
        }

        //
        // Break up the dataset into a collection of datasets, one
        // per boundary.
        //
        int *boundaryList = ((vtkIntArray*)boundaryArray)->GetPointer(0);

        vtkGeometryFilter *geom = NULL;
        if (in_ds->GetDataObjectType() != VTK_POLY_DATA)
        {
            geom = vtkGeometryFilter::New();
            geom->SetInputData(in_ds);
            geom->Update();
            in_ds = geom->GetOutput();
        }

        vtkPolyData *in_pd = (vtkPolyData *)in_ds;
        int ntotalcells = in_pd->GetNumberOfCells();

        vtkCellData *in_CD = in_ds->GetCellData();

        //
        // Determine the total number of boundarys, the number that
        // were selected, and the labels for the boundarys.
        //
        char *cLabelStorage = new char[label.length()+1];
        strcpy(cLabelStorage, label.c_str());
        char *cLabel = cLabelStorage;

        int nSelectedBoundaries = 0;
        sscanf(cLabel, "%d", &nSelectedBoundaries);
        cLabel = strchr(cLabel, ';') + 1;

        int i, *selectedBoundaries = new int[nSelectedBoundaries];
        char **selectedBoundaryNames = new char*[nSelectedBoundaries];
        for (i = 0; i < nSelectedBoundaries; i++)
        {
            sscanf(cLabel, "%d", &selectedBoundaries[i]);
            cLabel = strchr(cLabel, ';') + 1;
            selectedBoundaryNames[i] = cLabel;
            cLabel = strchr(cLabel, ';');
            cLabel[0] = '\0';
            cLabel = cLabel + 1;
        }

        int maxBoundary = selectedBoundaries[0];
        for (i = 1; i < nSelectedBoundaries; i++)
        {
            maxBoundary = selectedBoundaries[i] > maxBoundary ?
                        selectedBoundaries[i] : maxBoundary;
        }

        //
        // Count the number of cells of each boundary.
        //
        int *boundaryCounts = new int[maxBoundary+1];
        for (int s = 0; s < maxBoundary + 1; s++)
        {
            boundaryCounts[s] = 0;
        }
        for (i = 0; i < ntotalcells; i++)
        {
            boundaryCounts[boundaryList[i]]++;
        }

        //
        // Create a dataset for each boundary.
        //
        out_ds = new vtkDataSet *[nSelectedBoundaries];

        //
        // The following call is a workaround for a VTK bug.  It turns
        // out that when GetCellType if called for the first time for a
        // PolyData it calls its BuildCells method which causes the iterator
        // used by InitTraversal and GetNextCell to be put at the end of
        // the list.
        //
        in_pd->GetCellType(0);
        
        for (i = 0; i < nSelectedBoundaries; i++)
        {
            int s = selectedBoundaries[i];

            if (boundaryCounts[s] > 0)
            {
                // Create a new polydata
                vtkPolyData *out_pd = vtkPolyData::New();

                // Copy the points and point data.
                out_pd->SetPoints(in_pd->GetPoints());
                out_pd->GetPointData()->PassData(in_pd->GetPointData());

                // Prepare cell data for copy. 
                vtkCellData *out_CD = out_pd->GetCellData();
                out_CD->CopyAllocate(in_CD, boundaryCounts[s]);

                // insert the cells
                out_pd->Allocate(boundaryCounts[s]);

                vtkIdType npts;
#if LIB_VERSION_LE(VTK,8,1,0)
                vtkIdType *pts;
#else
                const vtkIdType *pts;
#endif
                int numNewCells = 0;
                for (int j = 0; j < ntotalcells; j++)
                {
                    if (boundaryList[j] == s)
                    {
                        in_pd->GetCellPoints(j, npts, pts);
                        out_pd->InsertNextCell(in_pd->GetCellType(j),
                                               npts, pts);
                        out_CD->CopyData(in_CD, j, numNewCells++); 
                    }
                }
                out_CD->RemoveArray("avtSubsets");
                labels.push_back(selectedBoundaryNames[i]);
                out_ds[nDataSets] = out_pd;
                nDataSets++;
            }
        }

        delete [] boundaryCounts;
        delete [] selectedBoundaryNames;
        delete [] selectedBoundaries;
        delete [] cLabelStorage;

        if (geom)
            geom->Delete();
    }
    else
    {
        //
        // The dataset represents a single boundary, so just turn it into
        // a data tree.
        //
        labels.push_back(label);

        out_ds = new vtkDataSet *[1];
        out_ds[0] = in_ds;
        out_ds[0]->Register(NULL);  // This makes it symmetric with the 'if'
                                    // case so we can delete it blindly later.

        nDataSets = 1;
    }

    if (nDataSets == 0)
    {
        delete [] out_ds;

        return NULL;
    }

    avtDataTree_p outDT = new avtDataTree(nDataSets, out_ds, domain, labels);

    for (int i = 0 ; i < nDataSets ; i++)
    {
        if (out_ds[i] != NULL)
        {
            out_ds[i]->Delete();
        }
    }
    delete [] out_ds;

    return outDT;
}


// ****************************************************************************
//  Method: avtFilledBoundaryFilter::UpdateDataObjectInfo
//
//  Purpose:  Retrieves the boundary names from the plot attributes and
//            sets them as labels in the data attributes. 
//
//  Programmer: Jeremy Meredith
//  Creation:   May  9, 2003
//
//  Note:  taken almost verbatim from the Subset plot
//
//  Modifications:
//    Kathleen Bonnell, Fri Nov 12 10:51:59 PST 2004
//    Added call to SetKeepNodeZoneArrays.
//
// ****************************************************************************

void
avtFilledBoundaryFilter::UpdateDataObjectInfo(void)
{
    avtDataAttributes &outAtts = GetOutput()->GetInfo().GetAttributes();
    outAtts.SetLabels(plotAtts.GetBoundaryNames());
    outAtts.SetKeepNodeZoneArrays(keepNodeZone);
}

 
// ****************************************************************************
//  Method: avtFilledBoundaryFilter::ModifyContract
//
//  Purpose:  Turn on domain labels in the data spec if needed.
//
//  Programmer: Jeremy Meredith
//  Creation:   June 13, 2003
//
//  Modifications:
//    
//    Hank Childs, Wed Aug 13 07:55:35 PDT 2003
//    Explicitly tell the data specification when we want MIR.
//
//    Kathleen Bonnell, Fri Nov 12 10:51:59 PST 2004
//    If working with a point mesh (topodim == 0), then determine if a point
//    size var secondary variable needs to be added to the pipeline, and
//    whether or not we need to keep Node and Zone numbers around. 
//
//    Kathleen Bonnell, Tue Jul 14 13:42:37 PDT 2009
//    Added test for MayRequireNodes for turning Node numbers on.
//
//    Hank Childs, Thu Aug 26 22:23:26 PDT 2010
//    Calculate the extents of the scaling variable.
//
//    Kathleen Biagas, Thu Dec 15 16:29:23 PST 2016
//    Remove if test for Material, as this plot only supports materials.
//
// ****************************************************************************

avtContract_p
avtFilledBoundaryFilter::ModifyContract(avtContract_p spec)
{
    spec->GetDataRequest()->ForceMaterialInterfaceReconstructionOn();
    if (plotAtts.GetDrawInternal())
    {
        spec->GetDataRequest()->TurnInternalSurfacesOn();
    }
    if (plotAtts.GetCleanZonesOnly())
    {
        spec->GetDataRequest()->SetNeedCleanZonesOnly(true);
    }

    if (GetInput()->GetInfo().GetAttributes().GetTopologicalDimension() == 0)
    {
        string pointVar = plotAtts.GetPointSizeVar();
        avtDataRequest_p dataRequest = spec->GetDataRequest();

        //
        // Find out if we REALLY need to add the secondary variable.
        //
        if (plotAtts.GetPointSizeVarEnabled() && 
            pointVar != "default" &&
            pointVar != "\0" &&
            pointVar != dataRequest->GetVariable() &&
            !dataRequest->HasSecondaryVariable(pointVar.c_str()))
        {
            spec->GetDataRequest()->AddSecondaryVariable(pointVar.c_str());
            spec->SetCalculateVariableExtents(pointVar, true);
        }

        if (spec->GetDataRequest()->MayRequireZones() ||
            spec->GetDataRequest()->MayRequireNodes())
        {
            keepNodeZone = true;
            spec->GetDataRequest()->TurnNodeNumbersOn();
        }
        else
        {
            keepNodeZone = false;
        }
    }

    return spec;
}

// ****************************************************************************
//  Method: avtFilledBoundaryFilter::PostExcecute
//
//  Purpose:  
//    Sets the output's label attributes to reflect what is currently
//    present in the tree.  
//
//  Programmer: Jeremy Meredith
//  Creation:   May  9, 2003
//
//  Note:  taken almost verbatim from the Subset plot
//
//  Modifications:
//    
// ****************************************************************************

void
avtFilledBoundaryFilter::PostExecute(void)
{
    vector <string> treeLabels;
    GetDataTree()->GetAllUniqueLabels(treeLabels);
    GetOutput()->GetInfo().GetAttributes().SetLabels(treeLabels);
}
