bl_info = {
	"name": "Import cheapest route path",
	"blender": (2, 80, 0),
	"category": "Import-Export"
}

import bpy
import json

def read_some_data(context, filepath):
	with open(filepath, 'r', encoding='utf-8') as f:
		data = json.load(f)

	path = data['cheapest_route']['path']
	world_scale = [float(value) for value in data['cheapest_route']['world_scale'].split(' ')]
	w = data['cheapest_route']['domain_size']['width']
	h = data['cheapest_route']['domain_size']['height']

	curve = bpy.data.curves.new('curve', 'CURVE')
	spline = curve.splines.new(type='NURBS')

	# a spline point for each point
	spline.points.add(len(path['x'])-1)

	# assign the point coordinates to the spline points
	for p, x, y, z in zip(spline.points, path['x'], path['y'], path['z']):
		p.co = ((x - 0.5*w)*world_scale[0], -(y - 0.5*h)*world_scale[1], z*world_scale[2] , 1.0)

	# make a new object with the curve
	obj = bpy.data.objects.new('cheapest_route', curve)
	bpy.context.scene.collection.objects.link(obj)

	return {'FINISHED'}


from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

class CheapestRoutePathImport(Operator, ImportHelper):
	"""This appears in the tooltip of the operator and in the generated docs"""
	bl_idname = "cheapest_route_import.path"
	bl_label = "Import cheapest route path"

	# ImportHelper mixin class uses this
	filename_ext = ".json"

	filter_glob: StringProperty(
		default="*.json",
		options={'HIDDEN'},
		maxlen=255,  # Max internal buffer length, longer would be clamped.
	)

	def execute(self, context):
		return read_some_data(context, self.filepath)


# Only needed if you want to add into a dynamic menu.
def menu_func_import(self, context):
	self.layout.operator(CheapestRoutePathImport.bl_idname, text="Import cheapest route path")


# Register and add to the "file selector" menu (required to use F3 search "Text Import Operator" for quick access).
def register():
	bpy.utils.register_class(CheapestRoutePathImport)
	bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
	bpy.utils.unregister_class(CheapestRoutePathImport)
	bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
	register()
