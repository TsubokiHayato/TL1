import bpy
# =====================================================
# アドオン情報
# =====================================================
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
# インポート
# =====================================================

# vertexを伸ばす
from .operators.stretch_vertex import MYADDON_OT_stretch_vertex
# sphereの作成
from .operators.create_ico_sphere import MYADDON_OT_create_ico_sphere
# シーンのエクスポート
from .operators.export_scene import MYADDON_OT_export_scene
# ファイル名の追加
from .operators.add_filename import MYADDON_OT_add_filename
# コライダーの追加
from .operators.add_collider import MYADDON_OT_add_collider
# コライダーパネル
from .panels.collider_panel import OBJECT_PT_collider
# ファイル名パネル
from .panels.file_name_panel import OBJECT_PT_file_name
# メニュー
from .menus.topbar_menu import TOPBAR_MT_my_menu
# 描画コライダー
from .utils.draw_collider import DrawCollider


# =====================================================
# 登録・解除処理
# =====================================================

classes = (
    # 各クラスをここに追加

    # 頂点を伸ばす
    MYADDON_OT_stretch_vertex,
    # IcoSphereを作成
    MYADDON_OT_create_ico_sphere,
    # シーンをエクスポート
    MYADDON_OT_export_scene,
    # メニュー
    TOPBAR_MT_my_menu,
    # ファイル名を追加
    MYADDON_OT_add_filename,
    # ファイル名パネル
    OBJECT_PT_file_name,
    # コライダーを追加
    OBJECT_PT_collider,
    # コライダーパネル
    MYADDON_OT_add_collider,
    
)

# =====================================================
# Blenderの登録・解除関数
# =====================================================

def register():

    #Blenderにクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)

    #メニューに項目を追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)

    #3Dビューに描画関数を追加
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider, (), 'WINDOW', 'POST_VIEW')


    print("レベルエディタが有効化されました。")


def unregister():
    #メニューから項目を削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)


    #3Dビューから描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle, 'WINDOW')

    #Blenderからクラスを解除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました。")


# =====================================================
# オプション: マニュアルボタン描画（未使用）
# =====================================================

def draw_menu_manual(self, context):
    # マニュアルボタンを描画
    self.layout.operator("wm.url_open_preset", text="Manual", icon="HELP")


# 実行テスト用（必要なら有効化）
# if __name__ == "__main__":
#     register()

