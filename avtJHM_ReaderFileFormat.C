/*****************************************************************************
*
* Copyright (c) 2000 - 2015, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                            avtJHM_ReaderFileFormat.C                           //
// ************************************************************************* //

#include <avtJHM_ReaderFileFormat.h>

#include <string>
#include <fstream>
#include <algorithm>

#include <vtkCellType.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <avtDatabaseMetaData.h>

#include <DBOptionsAttributes.h>
#include <Expression.h>

#include <InvalidVariableException.h>
#include <InvalidDBTypeException.h>

#include <iostream>
using namespace std;

using     std::string;
using     std::ifstream;

// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat constructor
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

avtJHM_ReaderFileFormat::avtJHM_ReaderFileFormat(const char *filename)
    : avtSTSDFileFormat (filename)
{
	elem_filename = filename;
	int ext = elem_filename.rfind('.');
	if (ext == string::npos) {
		EXCEPTION1(InvalidDBTypeException, ("Can't find model name in " + elem_filename).c_str());
		return;
	}
	pts_filename = elem_filename.substr(0,ext) + ".pts";
	lon_filename = elem_filename.substr(0,ext) + ".lon";
	ifstream lon_infile (lon_filename.c_str());
	if (!lon_infile.is_open ())
	{
		nvectors = 0;
		return;
	}
	string header;
	getline (lon_infile, header);
	nvectors = -1;
	lon_header = false;
	if (header.size() == 1) {
		if (header == "1") {
			nvectors = 1;
		} else if (header == "2") {
			nvectors = 2;
		} else {
			EXCEPTION1(InvalidDBTypeException, ("Incorrect lon header in '" + lon_filename + "'. Expected '1' or '2'.").c_str());
			return;
		}
		lon_header = true;
	}

	stringstream ss (header);
	float in;
	ss >> in >> in >> in;
	if (ss.eof()) {
		nvectors = 1;
	} else {
		nvectors = 2;
	}
}


// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat::FreeUpResources
//
//  Purpose:
//      When VisIt is done focusing on a particular timestep, it asks that
//      timestep to free up any resources (memory, file descriptors) that
//      it has associated with it.  This method is the mechanism for doing
//      that.
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

void
avtJHM_ReaderFileFormat::FreeUpResources(void)
{
}


// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat::PopulateDatabaseMetaData
//
//  Purpose:
//      This database meta-data object is like a table of contents for the
//      file.  By populating it, you are telling the rest of VisIt what
//      information it can request from you.
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

void
avtJHM_ReaderFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md)
{
    //
    // CODE TO ADD A MESH
    //
    // string meshname = ...
    //
    // AVT_RECTILINEAR_MESH, AVT_CURVILINEAR_MESH, AVT_UNSTRUCTURED_MESH,
    // AVT_POINT_MESH, AVT_SURFACE_MESH, AVT_UNKNOWN_MESH
    // avtMeshType mt = AVT_RECTILINEAR_MESH;
    //
    // int nblocks = 1;  <-- this must be 1 for STSD
    // int block_origin = 0;
    // int spatial_dimension = 2;
    // int topological_dimension = 2;
    // double *extents = NULL;
    //
    // Here's the call that tells the meta-data object that we have a mesh:
    //
    // AddMeshToMetaData(md, meshname, mt, extents, nblocks, block_origin,
    //                   spatial_dimension, topological_dimension);
    //
//

	avtMeshMetaData *mmd = new avtMeshMetaData;
	mmd->name = "volume mesh";
	mmd->spatialDimension = 3;
	mmd->topologicalDimension = 3;
	mmd->meshType = AVT_UNSTRUCTURED_MESH;
	mmd->numBlocks = 1;
	md->Add(mmd);

    //
    // CODE TO ADD A SCALAR VARIABLE
    //
    // string mesh_for_this_var = meshname; // ??? -- could be multiple meshes
    // string varname = ...
    //
    // AVT_NODECENT, AVT_ZONECENT, AVT_UNKNOWN_CENT
    // avtCentering cent = AVT_NODECENT;
    //
    //
    // Here's the call that tells the meta-data object that we have a var:
    //
    // AddScalarVarToMetaData(md, varname, mesh_for_this_var, cent);
    //

	avtScalarMetaData *smd = new avtScalarMetaData;
	smd->name = "region number";
	smd->meshName = "mesh";
	smd->centering = AVT_ZONECENT;
	smd->hasUnits = false;
	md->Add(smd);

    //
    // CODE TO ADD A VECTOR VARIABLE
    //
    // string mesh_for_this_var = meshname; // ??? -- could be multiple meshes
    // string varname = ...
    // int vector_dim = 2;
    //
    // AVT_NODECENT, AVT_ZONECENT, AVT_UNKNOWN_CENT
    // avtCentering cent = AVT_NODECENT;
    //
    //
    // Here's the call that tells the meta-data object that we have a var:
    //
    // AddVectorVarToMetaData(md, varname, mesh_for_this_var, cent,vector_dim);
    //

    //
    // CODE TO ADD A TENSOR VARIABLE
    //
    // string mesh_for_this_var = meshname; // ??? -- could be multiple meshes
    // string varname = ...
    // int tensor_dim = 9;
    //
    // AVT_NODECENT, AVT_ZONECENT, AVT_UNKNOWN_CENT
    // avtCentering cent = AVT_NODECENT;
    //
    //
    // Here's the call that tells the meta-data object that we have a var:
    //
    // AddTensorVarToMetaData(md, varname, mesh_for_this_var, cent,tensor_dim);
    //

	if (nvectors >= 1)
	{
		avtVectorMetaData *vmd = new avtVectorMetaData;
		vmd->name = "Fiber";
		vmd->meshName = "volume mesh";
		vmd->centering = AVT_ZONECENT;
		vmd->hasUnits = false;
		vmd->varDim = 3;
		md->Add(vmd);
	}

	if (nvectors == 2)
	{
		avtVectorMetaData *vmd = new avtVectorMetaData;
		vmd->name = "Transverse";
		vmd->meshName = "volume mesh";
		vmd->centering = AVT_ZONECENT;
		vmd->hasUnits = false;
		vmd->varDim = 3;
		md->Add(vmd);

		vmd = new avtVectorMetaData;
		vmd->name = "Sheet";
		vmd->meshName = "volume mesh";
		vmd->centering = AVT_ZONECENT;
		vmd->hasUnits = false;
		vmd->varDim = 3;
		md->Add(vmd);
	}
    
	//
    // CODE TO ADD A MATERIAL
    //
    // string mesh_for_mat = meshname; // ??? -- could be multiple meshes
    // string matname = ...
    // int nmats = ...;
    // vector<string> mnames;
    // for (int i = 0 ; i < nmats ; i++)
    // {
    //     char str[32];
    //     sprintf(str, "mat%d", i);
    //     -- or -- 
    //     strcpy(str, "Aluminum");
    //     mnames.push_back(str);
    // }
    // 
    // Here's the call that tells the meta-data object that we have a mat:
    //
    // AddMaterialToMetaData(md, matname, mesh_for_mat, nmats, mnames);
    //
    //
    // Here's the way to add expressions:
    //Expression momentum_expr;
    //momentum_expr.SetName("momentum");
    //momentum_expr.SetDefinition("{u, v}");
    //momentum_expr.SetType(Expression::VectorMeshVar);
    //md->AddExpression(&momentum_expr);
    //Expression KineticEnergy_expr;
    //KineticEnergy_expr.SetName("KineticEnergy");
    //KineticEnergy_expr.SetDefinition("0.5*(momentum*momentum)/(rho*rho)");
    //KineticEnergy_expr.SetType(Expression::ScalarMeshVar);
    //md->AddExpression(&KineticEnergy_expr);
}


// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat::GetMesh
//
//  Purpose:
//      Gets the mesh associated with this file.  The mesh is returned as a
//      derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
//      vtkUnstructuredGrid, etc).
//
//  Arguments:
//      meshname    The name of the mesh of interest.  This can be ignored if
//                  there is only one mesh.
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

int getCellType (const char *meshname, string fileCellType)
{
	if (fileCellType == "Hx")
		return VTK_HEXAHEDRON;
	else if (fileCellType == "Py")
		return VTK_PYRAMID;
	else if (fileCellType == "Pr")
		return VTK_WEDGE;
	else if (fileCellType == "Tt")
		return VTK_TETRA;
	EXCEPTION1 (InvalidVariableException, meshname);
	return -1;
}

vtkDataSet *
avtJHM_ReaderFileFormat::GetMesh(const char *meshname)
{
	if (strcmp (meshname, "volume mesh"))
	{
		EXCEPTION1 (InvalidVariableException, meshname);
		return 0;
	}

	int nnodes = -1;
	ifstream pts_infile (pts_filename.c_str());
	pts_infile >> nnodes;

	vtkPoints *points = vtkPoints::New ();
	points->SetNumberOfPoints (nnodes);
	float *pts = (float *) points->GetVoidPointer (0);

	for (int i = 0; i < nnodes; ++ i)
	{
		pts_infile >> *pts++; // X-coord
		pts_infile >> *pts++; // Y-coord
		pts_infile >> *pts++; // Z-coord
	}
	
	pts_infile.close ();

	ncells = -1;
	ifstream elem_infile (elem_filename.c_str());
	elem_infile >> ncells;

	arr_region = vtkIntArray::New ();
	arr_region->SetNumberOfTuples (ncells);
	int *data_region = (int *) arr_region->GetVoidPointer(0);

	vtkIdType verts[8];
	int nverts = -1;
	vtkUnstructuredGrid *ugrid = vtkUnstructuredGrid::New ();
	ugrid->SetPoints (points);
	points->Delete ();
	ugrid->Allocate (ncells);

	for (int i = 0; i < ncells; ++ i)
	{
		string fileCellType;
		elem_infile >> fileCellType;
		int cellType = getCellType (meshname, fileCellType);
		if (cellType == VTK_VERTEX)
			nverts = 1;
		else if (cellType == VTK_LINE)
			nverts = 2;
		else if (cellType == VTK_TRIANGLE)
			nverts = 3;
		else if (cellType == VTK_QUAD)
			nverts = 4;
		else if (cellType == VTK_TETRA)
			nverts = 4;
		else if (cellType == VTK_PYRAMID)
			nverts = 5;
		else if (cellType == VTK_WEDGE)
			nverts = 6;
		else if (cellType == VTK_HEXAHEDRON)
			nverts = 8;
		else
		{
			ugrid->Delete ();
			elem_infile.close ();
			EXCEPTION1 (InvalidVariableException, meshname);
			return 0;
		}

		for (int j = 0; j < nverts; ++ j)
		{
			elem_infile >> verts[j];
		}
		
		elem_infile >> *data_region++;

		if (cellType == VTK_HEXAHEDRON)
		{
			std::swap (verts[5], verts[7]);
		}
		else if (cellType == VTK_WEDGE)
		{
			std::swap (verts[4], verts[5]);
		}
		ugrid->InsertNextCell (cellType, nverts, verts);
	}

	return ugrid;
}


// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat::GetVar
//
//  Purpose:
//      Gets a scalar variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      varname    The name of the variable requested.
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

vtkDataArray *
avtJHM_ReaderFileFormat::GetVar(const char *varname)
{
	if (strcmp (varname, "region number"))
	{
		EXCEPTION1(InvalidVariableException, varname);
		return 0;
	}

	return arr_region;
}


// ****************************************************************************
//  Method: avtJHM_ReaderFileFormat::GetVectorVar
//
//  Purpose:
//      Gets a vector variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      varname    The name of the variable requested.
//
//  Programmer: lindenr -- generated by xml2avt
//  Creation:   Wed Jul 8 18:53:14 PST 2015
//
// ****************************************************************************

vtkDataArray *
avtJHM_ReaderFileFormat::GetVectorVar(const char *varname)
{
	if (nvectors == 0)
	{
		EXCEPTION1(InvalidVariableException, varname);
		return 0;
	}
	int index = 0;
	if (strcmp (varname, "Fiber") == 0)
		index = 1;
	else if (strcmp (varname, "Transverse") == 0)
		index = 2;
	else if (strcmp (varname, "Sheet") == 0)
		index = 3;
	if ((nvectors == 1 && index != 1) ||
	    (nvectors == 2 && (index < 1 || index > 3)))
	{
		EXCEPTION1(InvalidVariableException, varname);
		return 0;
	}

	ifstream lon_infile (lon_filename.c_str());
	
	if (lon_header)
	{
		int header;
		lon_infile >> header;
	}

	vtkFloatArray *arr = vtkFloatArray::New ();
	arr->SetNumberOfComponents (3);
	arr->SetNumberOfTuples (ncells);

	float *data = (float *) arr->GetVoidPointer (0);
	float data_fiber[3], data_transverse[3];

	if (index != 3)
	{
		for (int i = 0; i < ncells; ++ i)
		{
			// read off one line
			lon_infile >> data_fiber[0];
			lon_infile >> data_fiber[1];
			lon_infile >> data_fiber[2];

			if (nvectors == 2)
			{
				lon_infile >> data_transverse[0];
				lon_infile >> data_transverse[1];
				lon_infile >> data_transverse[2];
			}

			// write coords to data
			if (index == 1)
			{
				*data++ = data_fiber[0];
				*data++ = data_fiber[1];
				*data++ = data_fiber[2];
			}
			else if (index == 2)
			{
				*data++ = data_transverse[0];
				*data++ = data_transverse[1];
				*data++ = data_transverse[2];
			}
			else if (index == 3)
			{
				*data++ = data_fiber[1]*data_transverse[2] - data_fiber[2]*data_transverse[1];
				*data++ = data_fiber[2]*data_transverse[0] - data_fiber[0]*data_transverse[2];
				*data++ = data_fiber[0]*data_transverse[1] - data_fiber[1]*data_transverse[0];
			}
		}
	}

	return arr;

    // vtkFloatArray *rv = vtkFloatArray::New();
    // int ucomps = (ncomps == 2 ? 3 : ncomps);
    // rv->SetNumberOfComponents(ucomps);
    // rv->SetNumberOfTuples(ntuples);
    // float *one_entry = new float[ucomps];
    // for (int i = 0 ; i < ntuples ; i++)
    // {
    //      int j;
    //      for (j = 0 ; j < ncomps ; j++)
    //           one_entry[j] = ...
    //      for (j = ncomps ; j < ucomps ; j++)
    //           one_entry[j] = 0.;
    //      rv->SetTuple(i, one_entry); 
    // }
    //
    // delete [] one_entry;
    // return rv;
    //
}
