# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8-80 compliant>

"""
This script imports Raw Triangle File format files to Blender.

The raw triangle format is very simple; it has no verts or faces lists.
It's just a simple ascii text file with the vertices of each triangle
listed on each line. In addition, a line with 12 values will be
imported as a quad. This may be in conflict with some other
applications, which use a raw format, but this is how it was
implemented back in blender 2.42.

Usage:
Execute this script from the "File->Import" menu and choose a Raw file to
open.

Notes:
Generates the standard verts and faces lists, but without duplicate
verts. Only *exact* duplicates are removed, there is no way to specify a
tolerance.
"""

import bpy


def getNextLine(fs):
    """
    Return the next non-blank non-comment line.
    Returns None if EOF.
    """
    while True:
        ln = fs.readline().decode('ascii', 'ignore').strip()
        if ln is None:  # EOF
            return None

        print(ln)
        if ln and not ln.startswith('#'):
            return ln


def readVertices(fs):
    """
    Read a chunk of vertices.
    Returns a list of (x,y,z) float tuples
    """
    nVerts = int(getNextLine(fs))
    verts = []
    for i in range(nVerts):
        ln = getNextLine(fs)
        x, y, z = tuple(float(a) for a in ln.split())
        verts.append((x, z, -y))
    return verts


def readFaces(fs):
    """
    Read a chunk of faces.
    Returns a list of (a,b,c) int tuples,
    where a,b,c are vertex indices.
    """
    nFaces = int(getNextLine(fs))
    faces = []
    for i in range(nFaces):
        ln = getNextLine(fs)
        faces.append(tuple(int(a) for a in ln.split()))
    return faces


def _readDeform(fs, defaultName):
    count = int(getNextLine(fs))
    for i in range(count):
        ln = getNextLine(fs).split()
        v = int(ln[0])
        x = float(ln[1])
        y = float(ln[2])
        z = float(ln[3])


def _readDeformations(fs, defaultName):
    count = int(getNextLine(fs))
    if count > 0:
        dv = []
        for i in range(count):
            dv.append(_readDeform(fs, defaultName))
    return dv


def readDynamicDeformations(fs):
    """
    Read a chunk of dynamic deformations (ie. action units).
    Returns a list of deformation objects.
    """
    _dynamicDeformations = _readDeformations(fs, "Action Unit")
    #_dynamicParams = [0 for x in range(len(_dynamicDeformations))]
    return _dynamicDeformations


def readStaticDeformations(fs):
    """
    Read a chunk of static deformations (ie. shape units).
    Returns a list of deformation objects.
    """
    _staticDeformations = _readDeformations(fs, "Shape Unit")
    #_staticParams = [0 for x in range(len(_staticParams))]
    return _staticDeformations


def readParams(fs):
    """
    Read a chunk of parameter values for dynamic/shape deformations.
    Returns a list of (int idx, float value) tuples,
    where idx is the index of the deformation, and value is the amount to deform by (-1 to +1).
    NOTE: NOT USED IN BLENDER IMPORT.
    """
    nParamsInFile = int(getNextLine(fs))
    dv = [0 for x in range(nParamsInFile)]
    for i in range(nParamsInFile):
        ln = getNextLine(fs).split()
        paramNo = int(ln[0])
        paramVal = float(ln[1])
        dv[paramNo] = paramVal
    return dv


def readTexCoords(fs):
    """
    Read a chunk of texture co-ordinates.
    Returns a list of (u,v) float tuples, and the filename. (uvcoords, filename)
    """
    nCoordsInFile = int(getNextLine(fs))
    if nCoordsInFile > 0:
        texCoords = []
        for i in range(nCoordsInFile):
            ln = getNextLine(fs)
            uv = tuple(float(a) for a in ln.split())
            texCoords.append(uv)
        texFilename = getNextLine(fs)
    return (texCoords, texFilename)


def readGlobal(fs):
    rotation = [float(a) for a in getNextLine(fs).split()]
    scale = [float(a) for a in getNextLine(fs).split()]
    translation = [float(a) for a in getNextLine(fs).split()]

    return (rotation, scale, translation)


def read(filepath):
    #convert the filename to an object name
    objName = bpy.path.display_name_from_filepath(filepath)

    print("Parsing WFM file")
    with open(filepath, "rb") as fs:
        print("\t... mesh data")
        verts = readVertices(fs)
        faces = readFaces(fs)

        print("\t... deform parameters")
        dynamicDeformations = readDynamicDeformations(fs)
        staticDeformations = readStaticDeformations(fs)
        dynamicParams = readParams(fs)
        staticParams = readParams(fs)

        print("\t... texture coords")
        texCoords, texFilename = readTexCoords(fs)
        print("\t... affine transform")
        rotation, scale, translation = readGlobal(fs)

    # Create mesh
    print("Creating mesh")
    mesh = bpy.data.meshes.new(objName)
    mesh.from_pydata(verts, [], faces)

    # Create scene object
    print("Creating object")
    scn = bpy.context.scene

    for o in scn.objects:
        o.select = False

    mesh.update()
    mesh.validate()

    nobj = bpy.data.objects.new(objName, mesh)

    #nobj.rotation_euler = rot

    scn.objects.link(nobj)
    nobj.select = True

    if scn.objects.active is None or scn.objects.active.mode == 'OBJECT':
        scn.objects.active = nobj


    # # Generate verts and faces lists, without duplicates
    # verts = []
    # coords = {}
    # index_tot = 0
    # faces_indices = []
    #
    # for f in faces:
    #     fi = []
    #     for i, v in enumerate(f):
    #         index = coords.get(v)
    #
    #         if index is None:
    #             index = coords[v] = index_tot
    #             index_tot += 1
    #             verts.append(v)
    #
    #         fi.append(index)
    #
    #     faces_indices.append(fi)
    #
    # mesh = bpy.data.meshes.new(objName)
    # mesh.from_pydata(verts, [], faces_indices)
    #
    # return mesh

