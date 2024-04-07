import numpy as np

from OCP.TopAbs import TopAbs_Orientation
from OCP.BRep import BRep_Tool
from OCP.TopLoc import TopLoc_Location

def tesselate_face(f):
    loc = TopLoc_Location()
    poly = BRep_Tool.Triangulation_s(f.wrapped, loc)
    Trsf = loc.Transformation()
    reverse = (
        True
        if f.wrapped.Orientation() == TopAbs_Orientation.TopAbs_REVERSED
        else False
    )

    # add vertices
    vertices = [
        (v.X(), v.Y(), v.Z())
        for v in (
            poly.Node(i).Transformed(Trsf) for i in range(1, poly.NbNodes() + 1)
        )
    ]

    # add triangles
    triangles = [
        (
            t.Value(1) + - 1,
            t.Value(3) + - 1,
            t.Value(2) + - 1,
        )
        if reverse
        else (
            t.Value(1) + - 1,
            t.Value(2) + - 1,
            t.Value(3) + - 1,
        )
        for t in poly.Triangles()
    ]

    return np.array(vertices), np.array(triangles, dtype="int")