import bpy
import random
import math
import freestyle

# remove all
for o in bpy.data.objects:
    bpy.data.objects.remove(o)
for c in bpy.data.collections:
    bpy.data.collections.remove(c)

num_planes = 800
scene_size = 15.0
plane_size = 1

myseed = 5
random.seed(a = myseed)
noiser = freestyle.types.Noise(seed = myseed)

def random_cartesian(min, max):
    return tuple(
        random.uniform(min, max) for _ in range(3)
    )

col = bpy.data.collections.new("Random planes")
mat = bpy.data.materials.new(name = "Base")
bpy.context.scene.collection.children.link(col)


lai = 0
# random planes
num_planes = 0
while lai < 1.0:
    random_location = [
            random.uniform(-scene_size, scene_size),
            random.uniform(-scene_size, scene_size),
            random.uniform(-scene_size, scene_size)]
    noise_sample =  0.3 + noiser.turbulence3(
        v    = random_location,
        freq = 0.1,
        amp  = 1,
        oct  = 4
    )

    if random.random() > noise_sample:
        continue

    bpy.ops.mesh.primitive_plane_add(
        size     = plane_size,
        location = random_location,
        rotation = random_cartesian(0, 2 * math.pi)
    )
    num_planes += 1

    plane = bpy.context.active_object
    plane.data.materials.append(mat)
    col.objects.link(plane)

    lai = num_planes *  (plane_size ** 2) / (scene_size ** 2)
