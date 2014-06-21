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


bl_info = {
    "name": "Candide WireFrameModel format (.wfm)",
    "author": "Jared Sanson",
    "version": (0, 2),
    "blender": (2, 57, 0),
    "location": "File > Import-Export > Wireframe Model (.wfm) ",
    "description": "Import-Export Candide-3 Wireframe Model",
    "warning": "",
    "category": "Import-Export"}

if "bpy" in locals():
    import imp
    if "import_wfm" in locals():
        imp.reload(import_wfm)
    if "export_wfm" in locals():
        imp.reload(export_wfm)
else:
    import bpy

from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper


class WfmImporter(bpy.types.Operator):
    """Load WFM triangle mesh data"""
    bl_idname = "import_mesh.wfm"
    bl_label = "Import WFM"
    bl_options = {'UNDO'}

    filepath = StringProperty(
            subtype='FILE_PATH',
            )
    filter_glob = StringProperty(default="*.wfm", options={'HIDDEN'})

    def execute(self, context):
        from . import import_wfm
        import_wfm.read(self.filepath)
        return {'FINISHED'}

    def invoke(self, context, event):
        wm = context.window_manager
        wm.fileselect_add(self)
        return {'RUNNING_MODAL'}


class WfmExporter(bpy.types.Operator, ExportHelper):
    """Save WFM triangle mesh data"""
    bl_idname = "export_mesh.wfm"
    bl_label = "Export WFM"

    filename_ext = ".wfm"
    filter_glob = StringProperty(default="*.wfm", options={'HIDDEN'})

    apply_modifiers = BoolProperty(
            name="Apply Modifiers",
            description="Use transformed mesh data from each object",
            default=True,
            )

    def execute(self, context):
        from . import export_wfm
        export_wfm.write(self.filepath,
                         self.apply_modifiers)

        return {'FINISHED'}


def menu_import(self, context):
    self.layout.operator(WfmImporter.bl_idname, text="Wireframe Model (.wfm)")


def menu_export(self, context):
    self.layout.operator(WfmExporter.bl_idname, text="Wireframe Model (.wfm)")


def register():
    bpy.utils.register_module(__name__)

    bpy.types.INFO_MT_file_import.append(menu_import)
    bpy.types.INFO_MT_file_export.append(menu_export)


def unregister():
    bpy.utils.unregister_module(__name__)

    bpy.types.INFO_MT_file_import.remove(menu_import)
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()
