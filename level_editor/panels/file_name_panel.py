import bpy


from ..operators.add_filename import MYADDON_OT_add_filename

class OBJECT_PT_file_name(bpy.types.Panel):
    bl_label = "fileName"
    bl_idname = "OBJECT_PT_file_name"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
       
       #パネルに項目を追加
        if "file_name" in context.object:
            self.layout.prop(context.object, '["file_name"]', text=self.bl_label)
        else:
            self.layout.operator(MYADDON_OT_add_filename.bl_idname)
        
        
