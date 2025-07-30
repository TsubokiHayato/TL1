import math
from multiprocessing import context
import bpy
import bpy_extras

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
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"  
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext = ".scene"

    def write_and_print(self, file, string):
        print(string)
        file.write(string + "\n")

    def execute(self, context):
        print("シーン情報をExportします")

        #ファイルに出力
        self.export()
      
        print("シーン情報をExportしました")
        self.report({'INFO'}, "Scene export completed")
        return {'FINISHED'}
    
    def export(self):
        print("シーン情報出力開始... %r" % self.filepath)

        with open(self.filepath, "wt", encoding='utf-8') as file:
            self.write_and_print(file, "SCENE")

            for obj in bpy.context.scene.objects:

                #親オブジェクトがあるものはスキップ(代わりに親から呼び出すから)
                if(obj.parent):
                    continue
                self.parse_scene_recursive(file, obj, 0)

    def parse_scene_recursive(self,file,object,level):
        """シーン解析用再帰関数"""

        #深さ分インデントする(タブを挿入)
        indent =   ''
        for i in range(level):
            indent += "\t"

        #オブジェクト名読み込み
        self.write_and_print(file,indent + object.type)
        trans,rot,scale = object.matrix_local.decompose()
        #回転をQuternionからEuler(三軸での回転角)に転換
        rot = rot.to_euler()
        #ラジアンから度数法に変換
        rot.x=math.degrees(rot.x)
        rot.y=math.degrees(rot.y)
        rot.z=math.degrees(rot.z)
        #トランスフォーム情報を表示
        self.write_and_print(file,indent + "Trans(%f,%f,%f)" % (trans.x,trans.y,trans.z))
        self.write_and_print(file,indent + "Rot(%f,%f,%f)" % (rot.x,rot.y,rot.z))
        self.write_and_print(file,indent + "Scale(%f,%f,%f)" % (scale.x,scale.y,scale.z))
        if "file_name" in object:
            self.write_and_print(file,indent + "Name %s" % object["file_name"])
        self.write_and_print(file,indent + "END")
        self.write_and_print(file,'')
        
        # 子オブジェクトを再帰的に処理
        for child in object.children:
            self.parse_scene_recursive(file, child, level + 1)

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


class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "FileName 追加"
    bl_description = "['file_name']カスタムプロパティを追加します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
       
        context.object["file_name"] = ""
        return {'FINISHED'}

# ======================================================
# プロパティパネル定義
# =====================================================


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

# =====================================================
# 登録・解除処理
# =====================================================

classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name
    
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

