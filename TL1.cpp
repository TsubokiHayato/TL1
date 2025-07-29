import math
import bpy
import os

# アドオン情報
bl_info = {
    "name": "レベルエディタ",
    "author": "Hayato Tsuboki",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "レベルエディタ",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}

# =====================================================
# オペレーター定義
# =====================================================

# 頂点を伸ばすオペレーター
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました。")
        return {'FINISHED'}


# ICO球を生成するオペレーター
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました。")
        return {'FINISHED'}

#オペレータ　シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_export_scene"  
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"

    def execute(self, context):
        print("シーン情報をExportします")

        # シーン内の全オブジェクトについて出力
        for obj in bpy.context.scene.objects:
            print(f"{obj.type} - {obj.name}")

            trans,rot,scale = obj.matrix_world.decompose()

            rot.x=math.degrees(rot.x)
            rot.y=math.degrees(rot.y)
            rot.z=math.degrees(rot.z)

            print("Trans(%f, %f, %f)"%(trans.x,trans.y,trans.z))
            print("Rot(%f, %f, %f)"%(rot.x,rot.y,rot.z))
            print("Scale(%f, %f, %f)"%(scale.x,scale.y,scale.z))

            if obj.parent:
                print("Parent: %s" % obj.parent.name)
            print()
        print("シーン情報をExportしました")
       # self.report({'INFO'}, "シーン情報をExportしました")
        self.report({'INFO'}, "Scene export completed")
        return {'FINISHED'}
# =====================================================
# メニュー定義
# =====================================================

# トップバー拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー by " + bl_info["author"]

    def draw(self, context):
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname,
                             text=MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname,
                             text=MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator(MYADDON_OT_export_scene.bl_idname,
                             text=MYADDON_OT_export_scene.bl_label)

    def submenu(self, context):
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)


# =====================================================
# 登録・解除処理
# =====================================================

classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    TOPBAR_MT_my_menu,
    
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    print("レベルエディタが有効化されました。")


def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました。")


# =====================================================
# オプション: マニュアルボタン描画（未使用）
# =====================================================

def draw_menu_manual(self, context):
    self.layout.operator("wm.url_open_preset", text="Manual", icon="HELP")


# 実行テスト用（必要なら有効化）
# if __name__ == "__main__":
#     register()
