
import bpy
import bpy_extras.io_utils
import json
import math

#オペレータ　シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"  
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext = ".json"

    def write_and_print(self, file, string):
        print(string)
        file.write(string + "\n")

    def execute(self, context):
        print("シーン情報をExportします")

        #ファイルに出力
        self.export_json()
      
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

    def export_json(self):
        """シーン情報をJSON形式で出力"""
        
        #保持する情報をまとめるdict
        json_object_root=dict()

        #ノード名
        json_object_root["name"]= "scene"
        #オブジェクトリストを作成
        json_object_root["objects"] = list()

        for object in bpy.context.scene.objects:
          
          #親オブジェクトがあるものはスキップ(代わりに親から呼び出すから)
          if(object.parent):
            continue
          
          #シーン直下のオブジェクトをルートノード(深さ0)とし、再帰関数で走査
          self.parse_scene_recursive_json(json_object_root["objects"], object, 0)

        #オブジェクトをJSON文字列にエンコード(改行とインデントを付けて)
        json_text=json.dumps(json_object_root, ensure_ascii=False, cls=json.JSONEncoder, indent=4)
        #コンソールに出力
        print(json_text)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的に閉じる
        with open(self.filepath, "wt", encoding='utf-8') as file:
            #ファイルに書き出し
            file.write(json_text)

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
        #カスタムプロパティの出力
        # "file_name"と"collider"が存在する場合はそれぞれ出力
        if "file_name" in object:
            self.write_and_print(file,indent + "Name %s" % object["file_name"])
        if "collider" in object:
            self.write_and_print(file,indent + "collider %s" % object["collider"])
            temp_str =indent +"collider_center %f %f %f"
            temp_str%= (object["collider_center"][0],
                        object["collider_center"][1],
                        object["collider_center"][2])
            self.write_and_print(file, temp_str)
            temp_str =indent +"collider_size %f %f %f"
            temp_str%= (object["collider_size"][0],
                        object["collider_size"][1],
                        object["collider_size"][2])
            self.write_and_print(file, temp_str)
        self.write_and_print(file,indent + "END")
        self.write_and_print(file,'')

        # 子オブジェクトを再帰的に処理
        for child in object.children:
            self.parse_scene_recursive(file, child, level + 1)
    
    def parse_scene_recursive_json(self, data_parent, object, level):
        """シーン解析用再帰関数( JSON形式)"""

        #オブジェクト情報を保持するdict
        json_object_child = dict()
        #ノード名
        json_object_child["name"] = object.name
        #オブジェクトの種類
        json_object_child["type"] = object.type

        #オブジェクトのローカルトランスフォームから
        #平行移動、回転、スケールを取得
        trans,rot,scale = object.matrix_local.decompose()
        #回転をQuternionからEuler(三軸での回転
        rot = rot.to_euler()
        #ラジアンから度数法に変換
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)
        #トランスフォーム情報をディクショナリに格納
        transform =dict()
        transform["translation"] = (trans.x, trans.y, trans.z)
        transform["rotation"] = (rot.x, rot.y, rot.z)
        transform["scale"] = (scale.x, scale.y, scale.z)
        #まとめて１個分のオブジェクト情報として格納
        json_object_child["transform"] = transform

        if"file_name" in object:
            #カスタムプロパティ"file_name"があれば追加
            json_object_child["file_name"] = object["file_name"]

        if "collider" in object:
            collider= dict()
            collider["type"] = object["collider"]
            collider["center"] = object["collider_center"].to_list()
            collider["size"] = object["collider_size"].to_list()

            json_object_child["collider"] = collider

        data_parent.append(json_object_child)

        #子ノードがあれば
        if len(object.children) > 0:
            json_object_child["children"] = list()

            #子ノードへ進む(深さ1増やす)
            for child in object.children:
                self.parse_scene_recursive_json(json_object_child["children"], child, level + 1)


