import bpy
import random
import math

num_planes = 100
scene_size = 15.0
plane_size = 0.5

def random_cartesian(min, max):
    return tuple(
        random.uniform(min, max) for _ in range(3)
    )

col = bpy.data.collections.new("Random planes")
mat = bpy.data.materials.new(name = "Base")
bpy.context.scene.collection.children.link(col)

# random planes
for i in range(num_planes):
    bpy.ops.mesh.primitive_plane_add(
        size=plane_size,
        location=(
            random.uniform(-scene_size, scene_size),
            random.uniform(-scene_size, scene_size),
            random.uniform(0, scene_size),
        ),
        rotation=random_cartesian(0, 2 * math.pi)
    )

    plane = bpy.context.active_object
    plane.data.materials.append(mat)
    col.objects.link(plane)

