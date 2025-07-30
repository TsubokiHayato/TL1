bl_info = {
    "name": "レベルエディタ",
    "author": "tubo haya",
    "version": (1, 0),
    "blender": (2, 80, 0),
    "location": "ビュー3D > サイドバー > レベルエディタ",
    "description": "シーンを出力し、オブジェクトにカスタムプロパティを設定するレベルエディタ",
    "category": "Object",
}

import bpy
import os
import json
from bpy.props import StringProperty, EnumProperty, BoolProperty
from bpy.types import Operator, Panel, PropertyGroup

#-- -- -- -- -- -- -- -- -- -- -- -- --
#カスタムプロパティ定義
#-- -- -- -- -- -- -- -- -- -- -- -- --
class LevelEditorProperties(PropertyGroup):
    object_type: EnumProperty(
        name="タイプ",
        description="オブジェクトの種類",
        items=[
            ('None', "なし", ""),
            ('Player', "プレイヤー", ""),
            ('Enemy', "エネミー", ""),
            ('Block', "ブロック", ""),
            ('Goal', "ゴール", ""),
        ],
        default='None'
    )

    collider_enabled: BoolProperty(
        name="コライダー表示",
        description="コライダーを表示するかどうか",
        default=False
    )

#-- -- -- -- -- -- -- -- -- -- -- -- --
#オブジェクト出力処理
#-- -- -- -- -- -- -- -- -- -- -- -- --
def export_scene(filepath):
    data = []
    for obj in bpy.context.scene.objects:
        if obj.type != 'MESH':
            continue
        props = obj.level_editor_props
        if props.object_type == 'None':
            continue
        entry = {
            "name": obj.name,
            "type": props.object_type,
            "position": list(obj.location),
            "rotation": list(obj.rotation_euler),
            "scale": list(obj.scale)
        }
        data.append(entry)

    with open(filepath, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

#-- -- -- -- -- -- -- -- -- -- -- -- --
#オペレーター：出力
#-- -- -- -- -- -- -- -- -- -- -- -- --
class LEVEL_EDITOR_OT_ExportScene(Operator):
    bl_idname = "level_editor.export_scene"
    bl_label = "シーンを出力"
    bl_description = "シーン内のオブジェクト情報をJSON形式で出力"

    filepath: StringProperty(subtype="FILE_PATH")

    def execute(self, context):
        export_scene(self.filepath)
        self.report({'INFO'}, f"出力成功: {self.filepath}")
        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

#-- -- -- -- -- -- -- -- -- -- -- -- --
#パネル UI
#-- -- -- -- -- -- -- -- -- -- -- -- --
class LEVEL_EDITOR_PT_Panel(Panel):
    bl_label = "レベルエディタ"
    bl_idname = "LEVEL_EDITOR_PT_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'レベルエディタ'

    def draw(self, context):
        layout = self.layout
        obj = context.object
        scene = context.scene

        if obj:
            layout.label(text="選択中オブジェクト:")
            layout.prop(obj.level_editor_props, "object_type")

        layout.separator()
        layout.operator("level_editor.export_scene", text="シーンを出力")

        layout.separator()
        layout.prop(scene.level_editor_props, "collider_enabled", text="コライダーを表示")

#-- -- -- -- -- -- -- -- -- -- -- -- --
#コライダー描画ハンドラ
#-- -- -- -- -- -- -- -- -- -- -- -- --
def draw_collider(self, context):
    if not context.scene.level_editor_props.collider_enabled:
        return
    for obj in context.scene.objects:
        if obj.type == 'MESH' and obj.level_editor_props.object_type in ['Block', 'Enemy', 'Goal']:
            bbox = [obj.matrix_world @ bpy.mathutils.Vector(corner) for corner in obj.bound_box]
            self.draw_box(bbox)

def draw_box(points):
    import gpu
    from gpu_extras.batch import batch_for_shader

    shader = gpu.shader.from_builtin('3D_UNIFORM_COLOR')
    shader.bind()
    shader.uniform_float("color", (1.0, 0.0, 0.0, 1.0))

    edges = [
        (0,1),(1,2),(2,3),(3,0),
        (4,5),(5,6),(6,7),(7,4),
        (0,4),(1,5),(2,6),(3,7)
    ]
    vertices = [points[i] for edge in edges for i in edge]
    batch = batch_for_shader(shader, 'LINES', {"pos": vertices})
    batch.draw(shader)

def register_draw_handler():
    global _draw_handler
    _draw_handler = bpy.types.SpaceView3D.draw_handler_add(draw_collider, (None, bpy.context), 'WINDOW', 'POST_VIEW')

def unregister_draw_handler():
    global _draw_handler
    if _draw_handler:
        bpy.types.SpaceView3D.draw_handler_remove(_draw_handler, 'WINDOW')
        _draw_handler = None

_draw_handler = None

#-- -- -- -- -- -- -- -- -- -- -- -- --
#登録 / 解除
#-- -- -- -- -- -- -- -- -- -- -- -- --
classes = (
    LevelEditorProperties,
    LEVEL_EDITOR_OT_ExportScene,
    LEVEL_EDITOR_PT_Panel,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.Object.level_editor_props = bpy.props.PointerProperty(type=LevelEditorProperties)
    bpy.types.Scene.level_editor_props = bpy.props.PointerProperty(type=LevelEditorProperties)
    register_draw_handler()

def unregister():
    unregister_draw_handler()
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    del bpy.types.Object.level_editor_props
    del bpy.types.Scene.level_editor_props

if __name__ == "__main__":
    register()
