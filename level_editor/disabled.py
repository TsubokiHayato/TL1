import bpy

# =====================================================
# オペレータ　無効オプション
# =====================================================
class MYADDON_OT_disabled(bpy.types.Operator):
    """This operator is disabled and does nothing."""
  
    bl_options = {'REGISTER', 'UNDO'}

    bl_idname = "myaddon.myaddon_ot_disabled"
    bl_label = "Disabled 追加"
    bl_description = "['Disabled']カスタムプロパティを追加します"
    

    def execute(self, context):
       #無効オプションをtrueに設定
        if "Disabled" not in context.object:
            context.object["Disabled"] = True
        else:
            context.object["Disabled"] = not context.object["Disabled"]
        
        return {'FINISHED'}

   

class MYADDON_PT_disabled_panel(bpy.types.Panel):
    """Disabled Panel"""
    bl_label = "Disabled"
    bl_idname = "MYADDON_PT_disabled_panel"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        
        #パネルに項目を追加
        #無効オプションをtrueに設定

        if "Disabled" in context.object:
            self.layout.prop(context.object, '["Disabled"]', text=self.bl_label)
        else:
            self.layout.operator(MYADDON_OT_disabled.bl_idname, text="Disabled Operator")
        
    
    
