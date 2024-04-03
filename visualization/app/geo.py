from OCP.TopAbs import TopAbs_Orientation
from OCP.BRep import BRep_Tool
from OCP.TopLoc import TopLoc_Location

def tesselate_face_list(faces):
    vertices = []
    triangles = []
    offset = 0

    for f in faces:
        loc = TopLoc_Location()
        poly = BRep_Tool.Triangulation_s(f.wrapped, loc)
        Trsf = loc.Transformation()
        reverse = (
            True
            if f.wrapped.Orientation() == TopAbs_Orientation.TopAbs_REVERSED
            else False
        )

        # add vertices
        vertices.append([
            (v.X(), v.Y(), v.Z())
            for v in (
                poly.Node(i).Transformed(Trsf) for i in range(1, poly.NbNodes() + 1)
            )
        ])

        # add triangles
        triangles.append([
            (
                t.Value(1) + offset - 1,
                t.Value(3) + offset - 1,
                t.Value(2) + offset - 1,
            )
            if reverse
            else (
                t.Value(1) + offset - 1,
                t.Value(2) + offset - 1,
                t.Value(3) + offset - 1,
            )
            for t in poly.Triangles()
        ])

    return vertices, triangles