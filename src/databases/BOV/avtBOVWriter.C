// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

// ************************************************************************* //
//                               avtBOVWriter.C                              //
// ************************************************************************* //

#include <avtBOVWriter.h>

#include <float.h>
#include <zlib.h>
#include <vector>

#include <visit-config.h>
#include <visitstream.h>

#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkRectilinearGrid.h>

#include <avtDatabaseMetaData.h>
#include <avtParallel.h>

#include <FileFunctions.h>

#include <InvalidDBTypeException.h>
#include <InvalidFilesException.h>
#include <ImproperUseException.h>

#include <avtBOVOptions.h>
#include <DBOptionsAttributes.h>

using     std::string;
using     std::vector;


// ****************************************************************************
//  Method: avtBOVWriter construtor
//
//  Purpose:
//      Initialize the option to compress the output files with gzip.
//
//  Programmer: Alister Maguire
//  Creation:   September 1, 2017
//
//  Modifications:
//
// ****************************************************************************

avtBOVWriter::avtBOVWriter(const DBOptionsAttributes *atts)
{
    switch(atts->GetEnum("Compression"))
    {
        case 0: gzCompress = false; break;
        case 1: gzCompress = true; break;
    }
}


// ****************************************************************************
//  Method: avtBOVWriter destructor
//
//  Purpose:
//      empty destructor.
//
//  Programmer: Alister Maguire
//  Creation:   September 1, 2017
//
//  Modifications:
//
// ****************************************************************************

avtBOVWriter::~avtBOVWriter()
{
    ;
}


// ****************************************************************************
//  Method: avtBOVWriter::OpenFile
//
//  Purpose:
//      Does no actual work.  Just records the stem name for the files.
//
//  Programmer: Hank Childs
//  Creation:   September 11, 2004
//
//  Modifications:
//    Jeremy Meredith/Hank Childs, Tue Mar 27 17:03:47 EDT 2007
//    Added numblocks to the OpenFile interface.
//
// ****************************************************************************

void
avtBOVWriter::OpenFile(const string &stemname, int nb)
{
    stem = stemname;
    nblocks = nb;
}


// ****************************************************************************
//  Method: avtBOVWriter::WriteHeaders
//
//  Purpose:
//      Writes out the BOV header file.
//
//  Programmer: Hank Childs
//  Creation:   September 11, 2004
//
//  Modifications:
//
//    Hank Childs, Wed Dec 22 09:16:53 PST 2004
//    Throw a more informative exception.
//
//    Hank Childs, Wed Mar 28 10:04:53 PDT 2007
//    Use the real number of blocks, not what is in the meta-data.  (There
//    may be SIL selection or there may be a resample operator.
//
//    Dave Pugmire, Thu May  7 12:59:09 EDT 2009
//    Allow multi-block export in parallel.
//
// ****************************************************************************

void
avtBOVWriter::WriteHeaders(const avtDatabaseMetaData *md,
                           const vector<string> &scalars,
                           const vector<string> &vectors,
                           const vector<string> &materials)
{
    if (shouldChangeTotalZones || shouldChangeChunks)
    {
        //We are running from visitconvert, multiblock is ok.
    }
    else if (nblocks != 1)
    {
        // We can only handle single block files from file export.
        EXCEPTION1(InvalidDBTypeException, 
                         "The BOV writer can only handle single block files.");
    }
   
    // 
    // Don't bother writing the header now.  We only support single block
    // datasets and we need to examine that dataset before we can write out
    // the header.  So just punt on writing the header and do it when we get
    // the single block.
    //
}


// ****************************************************************************
//  Function: ResampleGrid
//
//  Purpose:
//      Resamples a rectilinear grid onto another rectilinear grid.  Note that
//      this isn't a generally useful thing to do, but it is good for scaling
//      studies.
//
//  Arguments:
//      rgrid          The rectilinear grid we are sampling *from*.
//      ptr            The data defined on rgrid.
//      samples        The buffer to write the interpolated values into.
//      brick_bounds   The bounds of the brick we are supposed to interpolate
//                     onto.
//      brick_dims     The dimensions of the brick we are supposed to 
//                     interpolate onto.
//
//  Programmer: Hank Childs
//  Creation:   September 11, 2004
//
//  Modifications:
//  
//    Dave Pugmire, Mon Jun  2 09:33:26 EDT 2008
//    Handle data with more than one component.
//
//    Hank Childs, Tue May 12 02:21:19 PDT 2009
//    Fix problem with floating point precision inaccuracies leading to
//    access of uninitialized memory.
//
// ****************************************************************************

static void
ResampleGrid(vtkRectilinearGrid *rgrid, float *ptr, float *samples, int numComponents,
             float *brick_bounds, int *brick_dims, bool outputZonal)
{
    int  i, j, k;

    float x_step = (brick_bounds[1] - brick_bounds[0]) / (brick_dims[0]-1);
    float y_step = (brick_bounds[3] - brick_bounds[2]) / (brick_dims[1]-1);
    float z_step = (brick_bounds[5] - brick_bounds[4]) / (brick_dims[2]-1);

    if (outputZonal)
    {
        brick_bounds[0] += x_step/2.0;
        brick_dims[0]   -= 1;
        brick_bounds[2] += y_step/2.0;
        brick_dims[1]   -= 1;
        brick_bounds[4] += z_step/2.0;
        brick_dims[2]   -= 1;
    }
    int grid_dims[3];
    rgrid->GetDimensions(grid_dims);

    float *x_prop = new float[brick_dims[0]];
    int   *x_ind  = new int[brick_dims[0]];
    for (i = 0 ; i < brick_dims[0] ; i++)
    {
        float x = brick_bounds[0] + x_step*i;
        if (i == brick_dims[0]-1)
            x = brick_bounds[1];  // floating-point roundoff screws up <,>
        for (j = 0 ; j < grid_dims[0]-1 ; j++)
        {
            float x1 = rgrid->GetXCoordinates()->GetTuple1(j);
            float x2 = rgrid->GetXCoordinates()->GetTuple1(j+1);
            if ((x1 <= x && x <= x2) || (j == grid_dims[0]-2))
            {
                x_ind[i] = j;
                float dist = x2-x1;
                if (dist == 0)
                    x_prop[i] = 0.;
                else
                {
                    float offset = x-x1;
                    x_prop[i] = offset / dist;
                }
                break;
            }
        }
    }

    float *y_prop = new float[brick_dims[1]];
    int   *y_ind  = new int[brick_dims[1]];
    for (i = 0 ; i < brick_dims[1] ; i++)
    {
        float y = brick_bounds[2] + y_step*i;
        if (i == brick_dims[1]-1)
            y = brick_bounds[3];  // floating-point roundoff screws up <,>
        for (j = 0 ; j < grid_dims[1]-1 ; j++)
        {
            float y1 = rgrid->GetYCoordinates()->GetTuple1(j);
            float y2 = rgrid->GetYCoordinates()->GetTuple1(j+1);
            if ((y1 <= y && y <= y2) || (j == grid_dims[1]-2))
            {
                y_ind[i] = j;
                float dist = y2-y1;
                if (dist == 0)
                    y_prop[i] = 0.;
                else
                {
                    float offset = y-y1;
                    y_prop[i] = offset / dist;
                }
                break;
            }
        }
    }

    float *z_prop = new float[brick_dims[2]];
    int   *z_ind  = new int[brick_dims[2]];
    for (i = 0 ; i < brick_dims[2] ; i++)
    {
        float z = brick_bounds[4] + z_step*i;
        if (i == brick_dims[2]-1)
            z = brick_bounds[5];  // floating-point roundoff screws up <,>
        for (j = 0 ; j < grid_dims[2]-1 ; j++)
        {
            float z1 = rgrid->GetZCoordinates()->GetTuple1(j);
            float z2 = rgrid->GetZCoordinates()->GetTuple1(j+1);
            if ((z1 <= z && z <= z2) || (j == grid_dims[2]-2))
            {
                z_ind[i] = j;
                float dist = z2-z1;
                if (dist == 0)
                    z_prop[i] = 0.;
                else
                {
                    float offset = z-z1;
                    z_prop[i] = offset / dist;
                }
                break;
            }
        }
    }

    for (k = 0 ; k < brick_dims[2] ; k++)
    {
        for (j = 0 ; j < brick_dims[1] ; j++)
        {
            for (i = 0 ; i < brick_dims[0] ; i++)
            {
                for ( int c = 0; c < numComponents; c++)
                {
                    // Tri-linear interpolation.
                    float val = 0.;
                    for (int l = 0 ; l < 8 ; l++)
                    {
                        int i_ = x_ind[i] + (l & 1 ? 1 : 0);
                        int j_ = y_ind[j] + (l & 2 ? 1 : 0);
                        int k_ = z_ind[k] + (l & 4 ? 1 : 0);
                        float x_prop_ = (l & 1 ? x_prop[i] : 1.-x_prop[i]);
                        float y_prop_ = (l & 2 ? y_prop[j] : 1.-y_prop[j]);
                        float z_prop_ = (l & 4 ? z_prop[k] : 1.-z_prop[k]);
                        int pt = k_*grid_dims[1]*grid_dims[0]*numComponents +
                                 j_*grid_dims[0]*numComponents +
                                 i_*numComponents +
                                 c;
                        val += x_prop_*y_prop_*z_prop_*ptr[pt];
                    }
                    int pt = k*brick_dims[1]*brick_dims[0]*numComponents + j*brick_dims[0]*numComponents + i*numComponents +c;
                    samples[pt] = val;
                }
            }
        }
    }

    delete [] x_prop;
    delete [] x_ind;
    delete [] y_prop;
    delete [] y_ind;
    delete [] z_prop;
    delete [] z_ind;
}


// ****************************************************************************
//  Method: avtBOVWriter::WriteChunk
//
//  Purpose:
//      This writes out one chunk of an avtDataset.
//
//  Programmer: Hank Childs
//  Creation:   September 11, 2004
//
//  Modifications:
//
//    Hank Childs, Thu Oct 21 18:10:29 PDT 2004
//    Fix problem with writing out one block while resampling.
//
//    Brad Whitlock, Wed Nov 3 12:13:15 PDT 2004
//    Changed long long coding for Windows.
//
//    Hank Childs, Wed Dec 22 09:16:53 PST 2004
//    Throw a more informative exception.  Also support situations where the
//    number of chunks is specified, but not the number of zones ['5736].
//    Also quote the variable name so that we can maintain multiple word
//    variable names. ['5733]
//
//    Hank Childs, Fri Apr 21 16:53:27 PDT 2006
//    Fix cut-n-paste bug.
//
//    Dave Pugmire, Mon Jun  2 09:33:26 EDT 2008
//    Handle data with more than one component.
//
//    Jeremy Meredith, Thu Aug  7 15:55:00 EDT 2008
//    Use const char* for string literals.
//    Remove unnecessary format modifiers in sprintf that caused warnings.
//
//    Dave Pugmire (on behalf of Hank Childs), Thu May  7 12:59:09 EDT 2009
//    Allow parallel writing of BOV files. Also, change bov file formatting
//    to consider how many chunks are written.
//
//    Hank Childs, Mon May 11 23:21:35 CDT 2009
//    Only have processor 0 write the header file.
//
//    Hank Childs, Thu May 21 18:57:31 PDT 2009
//    Add a cast to make AIX happy.
//
//    Tom Fogal, Tue Jul 27 15:26:54 MDT 2010
//    Don't crash if we don't have a stem filename.
//
//    Hank Childs, Mon Dec 10 11:37:32 PST 2012
//    Add support for double precision.
//
//    Mark C. Miller, Sun Dec 16 17:34:17 PST 2012
//    Fix potential leak of ptr array on early returns.
//
//    Brad Whitlock, Fri Apr 12 15:42:36 PDT 2013
//    Use gzFile instead of void* for gz_handle.
//
//    Alister Maguire, Thu Sep  7 09:02:03 PDT 2017
//    Added an option for compressing the output files with
//    gzip.
//
//    Kathleen Biagas, Wed Nov 18 2020
//    Replace VISIT_LONG_LONG with long long.
//
//    Cyrus Harrison, Fri Jun 10 16:22:37 PDT 2022
//    Bug fix for relative paths appearing in bov data file entry.
//    Use `.bof` suffix for output data files.
//    Improve error messages.
//
//    Eric Brugger, Wed Jun 22 11:26:44 PDT 2022
//    Fixed a bug where the DATA_FILE was not set properly in the root
//    bov file when using gzip compression.
//
// ****************************************************************************

void
avtBOVWriter::WriteChunk(vtkDataSet *ds, int chunk)
{
    int   i;
    if (ds->GetDataObjectType() != VTK_RECTILINEAR_GRID)
    {
        EXCEPTION1(InvalidDBTypeException, 
                         "The BOV writer can only handle rectilinear grids.");
    }
    vtkRectilinearGrid *rgrid = (vtkRectilinearGrid *) ds;

    //
    // We are only set up to operate on one nodal array.  Which array we 
    // operate on should be set by the commanding program (ie convert).
    //
    vtkDataArray *arr = rgrid->GetPointData()->GetScalars();
    if (arr == NULL)
    {
        int nPtVars = rgrid->GetPointData()->GetNumberOfArrays();
        for (i = 0 ; nPtVars ; i++)
        {
            vtkDataArray *arr2 = rgrid->GetPointData()->GetArray(i);
            if (arr2->GetNumberOfComponents() == 1)
            {
                arr = arr2;
                break;
            }
            else if (arr2->GetNumberOfComponents() == 3)
            {
                arr = arr2;
                break;
            }
        }

        if (arr == NULL)
            EXCEPTION1(InvalidDBTypeException, 
                       "The BOV writer could not find a variable to write.  "
                       "This may be because it only supports nodal variables "
                       "and you wanted to write a zonal variable.");
    }

    float *ptr = NULL;
    bool deletePtr = false;
    if (arr->GetDataType() == VTK_FLOAT)
        ptr = (float *) arr->GetVoidPointer(0);
    else
    {
        int ncomps = arr->GetNumberOfComponents();
        int ntups  = arr->GetNumberOfTuples();
        int nvals = ncomps*ntups;
        ptr = new float[nvals];
        for (int i = 0 ; i < ntups ; i++)
            for (int j = 0 ; j < ncomps ; j++)
            {
                int index = ncomps*i+j;
                ptr[index] = (float) arr->GetComponent(i, j);
            }
        deletePtr = true;
    }

    std::string root_file = stem + ".bov";
    ofstream *ofile = NULL;
    if (PAR_Rank() == 0)
    {
        ofile = new ofstream(root_file.c_str());
        if(!ofile->is_open())
        {
            std::ostringstream err_msg;
            err_msg << "Could not open root bov file: '" << root_file << "'."
                       " Do you lack write access "
                       "on the destination filesystem?";
            EXCEPTION1(InvalidFilesException,
                       err_msg.str());
        }

        *ofile << "#BOV version: 1.0" << endl;
        *ofile << "# file written by VisIt conversion program " << endl;
    }

    int dims[3];
    rgrid->GetDimensions(dims);

    int nBricklets = 1;
    int brickletsPerX = 1;
    int brickletsPerY = 1;
    int brickletsPerZ = 1;
    int brickletNI = dims[0];
    int brickletNJ = dims[1];
    int brickletNK = dims[2];
    if (shouldChangeChunks)
    {
        double cubeRoot = pow(double(nTargetChunks), 0.3333);
        int approxCubeRoot = ((int) cubeRoot);
        if (approxCubeRoot*approxCubeRoot*approxCubeRoot != nTargetChunks)
            approxCubeRoot += 1;
        brickletsPerX = approxCubeRoot;
        brickletsPerY = approxCubeRoot;
        brickletsPerZ = approxCubeRoot;
        nBricklets = brickletsPerX*brickletsPerY*brickletsPerZ;
    }
    if (shouldChangeTotalZones || shouldChangeChunks)
    {
        // If we are changing the chunks, but not the zones, we should set the
        // target number of zones to be the current number of zones, so we
        // can then split that number into chunks.
        if (!shouldChangeTotalZones)
            targetTotalZones = dims[0]*dims[1]*dims[2];
        long long zonesPerBricklet = targetTotalZones / (long long) nBricklets;
        zonesPerBricklet += 1;
        double cubeRoot = pow( (double) zonesPerBricklet, 0.3333);
        int approxCubeRoot = ((int) cubeRoot) + 1;

        brickletNI = approxCubeRoot;
        brickletNJ = approxCubeRoot;
        brickletNK = approxCubeRoot;
    }

    // The output is always gzip compressed when using bricklets.
    int numDecimals = 4;
    if (nBricklets > 1)
    {
        numDecimals = (int)log10((double)nBricklets)+1;
        if (numDecimals < 4)
            numDecimals = 4;

        if (PAR_Rank() == 0)
        {
            // This needs to be the stem w/o any pre-fixed directories,
            // or else BOV header is bad
            char fname_data_cstr[1024];
            sprintf(fname_data_cstr, "%s_%%0.%dd.bof.gz", stem.c_str(), numDecimals);
            std::string fname_data = FileFunctions::Basename(std::string(fname_data_cstr));
            *ofile << "DATA_FILE: " << fname_data << endl;
        }
    }
    else
    {
        if (PAR_Rank() == 0)
        {
            // This needs to be the stem w/o any pre-fixed directories,
            // or else BOV header is bad
            std::string fname_data = FileFunctions::Basename(std::string(stem)) + ".bof";
            if (gzCompress)
            { 
                fname_data = fname_data + ".gz";
            }
            *ofile << "DATA_FILE: " << fname_data << endl;
        }
    }

    if (PAR_Rank() == 0)
    {
        if (shouldOutputZonal)
        {
            *ofile << "DATA SIZE: " << brickletsPerX*(brickletNI-1) << " "
                   << brickletsPerY*(brickletNJ-1) << " " << brickletsPerZ*(brickletNK-1)
                   << endl;
        }
        else
        {
            *ofile << "DATA SIZE: " << brickletsPerX*brickletNI << " "
                   << brickletsPerY*brickletNJ << " " << brickletsPerZ*brickletNK
                   << endl;
        }
    }

    if (nBricklets > 1)
        if (PAR_Rank() == 0)
        {
            if (shouldOutputZonal)
                *ofile << "DATA_BRICKLETS: " << brickletNI-1 << " " << brickletNJ-1
                       << " " << brickletNK-1 << endl;
            else
                *ofile << "DATA_BRICKLETS: " << brickletNI << " " << brickletNJ
                       << " " << brickletNK << endl;
        }

    if (PAR_Rank() == 0)
        *ofile << "DATA FORMAT: FLOATS" << endl;

    if (arr->GetNumberOfComponents() > 1)
        if (PAR_Rank() == 0)
            *ofile << "DATA_COMPONENTS: " << arr->GetNumberOfComponents() << endl;
    if (PAR_Rank() == 0)
        *ofile << "VARIABLE: \"" << arr->GetName()  << "\"" << endl;

    int nvals = dims[0]*dims[1]*dims[2]*arr->GetNumberOfComponents();

    if (arr->GetNumberOfComponents() == 1)
    {
        float max = -FLT_MAX;
        float min = +FLT_MAX;
        for (i = 0 ; i < nvals ; i++)
        {
            min = (min < ptr[i] ? min : ptr[i]);
            max = (max > ptr[i] ? max : ptr[i]);
        }
        if (PAR_Rank() == 0)
        {
            *ofile << "VARIABLE PALETTE MIN: " << min << endl;
            *ofile << "VARIABLE PALETTE MAX: " << max << endl;
        }
    }
     
    double bounds[6];
    rgrid->GetBounds(bounds);
 
    if (PAR_Rank() == 0)
    {
        *ofile << "BRICK ORIGIN: " << bounds[0] << " " << bounds[2] << " "
               << bounds[4] << endl;
        *ofile << "BRICK SIZE: " << bounds[1]-bounds[0] << " " 
               << bounds[3]-bounds[2] << " "
               << bounds[5]-bounds[4] << endl;
        *ofile << "BRICK X_AXIS: 1.000 0.000 0.000" << endl;
        *ofile << "BRICK Y_AXIS: 0.000 1.000 0.000" << endl;
        *ofile << "BRICK Z_AXIS: 0.000 0.000 1.000" << endl;
    }

    // 
    // The information below is good to have.  But if we write it out, then
    // we are breaking with the "standard" BOV format.  But if we are writing
    // out multiple bricks, then we are already breaking with the format, so
    // we may as well.
    //
#ifdef WORDS_BIGENDIAN
    const char *endian_str = "BIG";
#else
    const char *endian_str = "LITTLE";
#endif
    if (nBricklets > 1)
    {
        if (PAR_Rank() == 0)
        {
            *ofile << "DATA_ENDIAN: " << endian_str << endl;
            if (shouldOutputZonal)
                *ofile << "CENTERING: zonal" << endl;
            else
                *ofile << "CENTERING: nodal" << endl;
        }
    }
    if (nBricklets == 1 && brickletNI == dims[0] && brickletNJ == dims[1]
        && brickletNK == dims[2])
    {
        //
        // No resampling necessary.  Also, don't gzip, so we can stay 
        // compatible with the "standard" BOV format.
        //
        if(stem.empty())
        {
            if (deletePtr) delete [] ptr;
            EXCEPTION1(InvalidFilesException,
                       "Could not figure out stem filename.");
        }
        //
        // If the compress option is on, then output a compressed bov file
        //
        if (gzCompress)
        { 
            std::string fname = stem + ".bof.gz";
            gzFile gz_handle = gzopen(fname.c_str(), "w");

            if(gz_handle == NULL)
            {
                if (deletePtr) delete [] ptr;
                std::ostringstream err_msg;
                err_msg << "Could not open data file: '" << fname << "'."
                           " Do you lack write access "
                           "on the destination filesystem?";
                EXCEPTION1(InvalidFilesException,
                           err_msg.str());
            }

            gzwrite(gz_handle, ptr, nvals*sizeof(float));
            gzclose(gz_handle);
        }
        else
        {
            std::string fname = stem + ".bof";
            FILE *file_handle = fopen(fname.c_str(), "w");
            if(file_handle == NULL)
            {
                if (deletePtr) delete [] ptr;
                std::ostringstream err_msg;
                err_msg << "Could not open data file: '" << fname << "'."
                           " Do you lack write access "
                           "on the destination filesystem?";
                EXCEPTION1(InvalidFilesException,
                           err_msg.str());
            }
            else
            {
                fwrite(ptr, sizeof(float), nvals, file_handle);
                fclose(file_handle);
            }
        }
    }
    else
    {
        int vals_per_bricklet = brickletNI*brickletNJ*brickletNK*arr->GetNumberOfComponents();
        float *samples = new float[vals_per_bricklet];
        for (int i = 0 ; i < brickletsPerX ; i++)
        {
            for (int j = 0 ; j < brickletsPerY ; j++)
            {
                for (int k = 0 ; k < brickletsPerZ ; k++)
                {
                    int brick = k*brickletsPerY*brickletsPerX 
                              + j*brickletsPerX + i;
                    if ((brick % PAR_Size()) != PAR_Rank())
                        continue;
                    float brick_bounds[6];
                    float x_step = (bounds[1] - bounds[0]) / brickletsPerX;
                    brick_bounds[0] = bounds[0] + i*x_step;
                    brick_bounds[1] = bounds[0] + (i+1)*x_step;
                    float y_step = (bounds[3] - bounds[2]) / brickletsPerY;
                    brick_bounds[2] = bounds[2] + j*y_step;
                    brick_bounds[3] = bounds[2] + (j+1)*y_step;
                    float z_step = (bounds[5] - bounds[4]) / brickletsPerZ;
                    brick_bounds[4] = bounds[4] + k*z_step;
                    brick_bounds[5] = bounds[4] + (k+1)*z_step;
                    int brick_dims[3] = { brickletNI, brickletNJ, brickletNK };
                    ResampleGrid(rgrid, ptr, samples, arr->GetNumberOfComponents(),brick_bounds,brick_dims,shouldOutputZonal);
                    char str[1024];
                    if (nBricklets > 1)
                    {
                        char fmt[1024];
                        sprintf(fmt, "%s_%%0.%dd.bof.gz", stem.c_str(), numDecimals);
                        sprintf(str, fmt, brick);
                        gzFile gz_handle = gzopen(str, "w");
                        if(gz_handle == NULL)
                        {
                            if (deletePtr) delete [] ptr;
                            std::ostringstream err_msg;
                            err_msg << "Could not open data file: '" << str << "'."
                                       " Do you lack write access "
                                       "on the destination filesystem?";
                            EXCEPTION1(InvalidFilesException,
                                       err_msg.str());
                        }
                        gzwrite(gz_handle, samples, 
                                vals_per_bricklet*sizeof(float));
                        gzclose(gz_handle);
                    }
                    else if (gzCompress)
                    {
                        std::string fname =  stem + ".bof.gz";
                        gzFile gz_handle = gzopen(fname.c_str(), "w");
                        if(gz_handle == NULL)
                        {
                            if (deletePtr) delete [] ptr;
                            std::ostringstream err_msg;
                            err_msg << "Could not open data file: '" << fname << "'."
                                       " Do you lack write access "
                                       "on the destination filesystem?";
                            EXCEPTION1(InvalidFilesException,
                                       err_msg.str());
                        }

                        gzwrite(gz_handle, samples, vals_per_bricklet*sizeof(float));
                        gzclose(gz_handle);
                    }
                    else
                    {
                        std::string fname =  stem + ".bof";
                        FILE *file_handle = fopen(fname.c_str(), "w");
                        if(file_handle == NULL)
                        {
                            if (deletePtr) delete [] ptr;
                            std::ostringstream err_msg;
                            err_msg << "Could not open data file: '" << fname << "'."
                                       " Do you lack write access "
                                       "on the destination filesystem?";
                            EXCEPTION1(InvalidFilesException,
                                       err_msg.str());
                        }
                        fwrite(samples, sizeof(float), vals_per_bricklet, 
                               file_handle);
                        fclose(file_handle);
                    }
                }
            }
        }
        delete [] samples;
    }

    if (deletePtr)
       delete [] ptr;
}


// ****************************************************************************
//  Method: avtBOVWriter::CloseFile
//
//  Purpose:
//      Closes the file.  This does nothing in this case.
//
//  Programmer: Hank Childs
//  Creation:   September 11, 2004
//
// ****************************************************************************

void
avtBOVWriter::CloseFile(void)
{
    // Just needed to meet interface.
}


