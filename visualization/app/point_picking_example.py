import pyvista as pv

mesh = pv.Sphere()

pl = pv.Plotter(notebook=0)
pl.add_mesh(mesh)

# Enable mouse tracking - access `pl.mouse_position`
pl.track_mouse_position()

# Enable point picking to improve picking results
pl.enable_point_picking(show_message=False)

def on_event(*args):
    if pl.mouse_position is None:
        return
    x, y = pl.mouse_position
    pl.add_text(f'{x}, {y}', name='mouse_position_text')
    position = pl.pick_mouse_position()
    print(position)
    pl.add_point_labels([position], ['Mouse',], name='mouse_position_label', pickable=False)
    
# Timer callback - not a flushed out API and poor performance
# pl.iren.add_observer("TimerEvent", on_event)
# duration = 100  # milliseconds
# pl.iren.create_timer(duration, repeating=True)

# Mouse event callback, cleaner with better performance
pl.iren.add_observer("MouseMoveEvent", on_event)

pl.show()